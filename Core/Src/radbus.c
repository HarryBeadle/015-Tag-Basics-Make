#include "radbus.h"

FDCAN_TxHeaderTypeDef rbTxHeader;
FDCAN_RxHeaderTypeDef rbRxHeader;
uint8_t rbRxData[8];
uint8_t rbTxData[8];

bool rbAddressClaimed;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef * hfdcan, uint32_t RxFifo0ITs)
{
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
        if (rbAddressClaimed == false) {
            rbClaimDeviceID(hfdcan, 1);
        }
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rbRxHeader, (uint8_t*) &rbRxData);
        rbHandleRxMessage(hfdcan);
  }
}

void rbTagAttach(FDCAN_HandleTypeDef *hfdcan, unsigned long tag_id)
{
    bzPlayTune(&bzAttachTune);
    rbTxHeader.Identifier = DEVICE_CAN_ID;
    rbTxHeader.IdType = FDCAN_EXTENDED_ID;
    rbTxHeader.DataLength = rbNBytes2DLC(7);
    rbTxData[0] = RB_FT_STATUS;
    rbTxData[1] = 0x00;
    rbTxData[2] = RB_TAGSTATUS_AT;
    rbTxData[3] = (uint8_t) (0xFF & tag_id >> 24);
    rbTxData[4] = (uint8_t) (0xFF & tag_id >> 16);
    rbTxData[5] = (uint8_t) (0xFF & tag_id >> 8);
    rbTxData[6] = (uint8_t) (0xFF & tag_id);
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &rbTxHeader, (uint8_t*) &rbTxData);
    globalLog.radbus.attach++;
    globalLog.tag.id = tag_id; // REMOVE WHEN RFID FW IS IMPLEMENTED!
    rgbYellow();
}

void rbTagDetach(FDCAN_HandleTypeDef *hfdcan, unsigned long tag_id)
{
    bzPlayTune(&bzDetachTune);
    globalLog.radbus.detach = SET;
    rbTxHeader.Identifier = DEVICE_CAN_ID;
    rbTxHeader.IdType = FDCAN_EXTENDED_ID;
    rbTxHeader.DataLength = rbNBytes2DLC(7);
    rbTxData[0] = RB_FT_STATUS;
    rbTxData[1] = 0x00;
    rbTxData[2] = RB_TAGSTATUS_DT;
    rbTxData[3] = (uint8_t) (0xFF & tag_id >> 24);
    rbTxData[4] = (uint8_t) (0xFF & tag_id >> 16);
    rbTxData[5] = (uint8_t) (0xFF & tag_id >> 8);
    rbTxData[6] = (uint8_t) (0xFF & tag_id);
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &rbTxHeader, (uint8_t*) &rbTxData);
    globalLog.radbus.detach++;
    rbAddressClaimed = true;
    rbTxHeader.Identifier = DEVICE_CAN_ID;
    rbTxHeader.IdType = FDCAN_EXTENDED_ID;
    rbTxHeader.DataLength = rbNBytes2DLC(2);
    rbTxData[0] = RB_FT_BROADCAST;
    rbTxData[1] = RB_BROADCAST_ESTOP;
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &rbTxHeader, (uint8_t*) &rbTxData);
    globalLog.radbus.estop++;
    rgbRed();
}

void rbClaimDeviceID(FDCAN_HandleTypeDef *hfdcan, unsigned int id)
{
    rbTxHeader.Identifier = DEVICE_CAN_ID;
    rbTxHeader.IdType = FDCAN_EXTENDED_ID;
    rbTxHeader.DataLength = rbNBytes2DLC(8);
    rbTxData[0] = RB_FT_STATUS;
    rbTxData[1] = 0xFF;
    rbTxData[2] = (uint8_t) (0xFF & DEVICE_UID >> 40);
    rbTxData[3] = (uint8_t) (0xFF & DEVICE_UID >> 32);
    rbTxData[4] = (uint8_t) (0xFF & DEVICE_UID >> 24);
    rbTxData[5] = (uint8_t) (0xFF & DEVICE_UID >> 16);
    rbTxData[6] = (uint8_t) (0xFF & DEVICE_UID >> 8);
    rbTxData[7] = (uint8_t) (0xFF & DEVICE_UID);
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &rbTxHeader, (uint8_t*) &rbTxData);
    rbAddressClaimed = true;
    globalLog.radbus.deviceIdClaimed = id;
}

void rbHandleRxMessage(FDCAN_HandleTypeDef *hfdcan)
{
    unsigned int id = rbRxHeader.Identifier;
    // unsigned int len = rbDLC2nBytes(rbRxHeader.DataLength);
    unsigned int frame_type = rbRxData[0];
    unsigned int frame_id = rbRxData[1];
    unsigned int device_type = rbRxData[1];
    unsigned int device_subtype = rbRxData[2];
    unsigned int device_id = rbRxData[3];
    switch (frame_type) {
        case RB_FT_SWSTATUSREQ:
            if (device_type == DEVICE_TYPE) {
                if (device_subtype == DEVICE_SUBTYPE) {
                    if (device_id == DEVICE_ID) {
                        globalLog.radbus.sw_request++;
                        rbTxHeader.DataLength = rbNBytes2DLC(5);
                        rbTxData[0] = RB_FT_STATUS;
                        rbTxData[1] = 0xFE;
                        rbTxData[2] = SW_V_MAJOR;
                        rbTxData[3] = SW_V_MINOR;
                        rbTxData[4] = SW_V_PATCH;
                        HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &rbTxHeader, (uint8_t*) &rbTxData);
                        globalLog.radbus.sw_response++;
                    }
                }
            }
            break;
        case RB_FT_STATUS: // Status Message
            switch (id) {
                case 0x1b030100: // Link
                    switch (frame_id) {
                        case 0x02: // Tag Acceptance Message
                            if (rbRxData[2] & RB_TAGSTATUS_TA) {
                                // Tag Accepted
                                rgbGreen();
                                globalLog.radbus.tag_accept++;
                                break; 
                            } else {
                                // Tag Declined
                                rgbRed();
                                globalLog.radbus.tag_decline++;
                                break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
            }
            break;
    }
}

unsigned int rbDLC2nBytes(unsigned int dlc)
{
    if (dlc == FDCAN_DLC_BYTES_0) return 0;
    if (dlc == FDCAN_DLC_BYTES_1) return 1;
    if (dlc == FDCAN_DLC_BYTES_2) return 2;
    if (dlc == FDCAN_DLC_BYTES_3) return 3;
    if (dlc == FDCAN_DLC_BYTES_4) return 4;
    if (dlc == FDCAN_DLC_BYTES_5) return 5;
    if (dlc == FDCAN_DLC_BYTES_6) return 6;
    if (dlc == FDCAN_DLC_BYTES_7) return 7;
    if (dlc == FDCAN_DLC_BYTES_8) return 8;
    if (dlc == FDCAN_DLC_BYTES_12) return 12;
    if (dlc == FDCAN_DLC_BYTES_16) return 16;
    if (dlc == FDCAN_DLC_BYTES_20) return 20;
    if (dlc == FDCAN_DLC_BYTES_24) return 24;
    if (dlc == FDCAN_DLC_BYTES_32) return 32;
    if (dlc == FDCAN_DLC_BYTES_48) return 48;
    if (dlc == FDCAN_DLC_BYTES_64) return 64;
    return 0;
}

unsigned int rbNBytes2DLC(unsigned int n_bytes)
{
    if (n_bytes <= 0) return FDCAN_DLC_BYTES_0;
    if (n_bytes <= 1) return FDCAN_DLC_BYTES_1;
    if (n_bytes <= 2) return FDCAN_DLC_BYTES_2;
    if (n_bytes <= 3) return FDCAN_DLC_BYTES_3;
    if (n_bytes <= 4) return FDCAN_DLC_BYTES_4;
    if (n_bytes <= 5) return FDCAN_DLC_BYTES_5;
    if (n_bytes <= 6) return FDCAN_DLC_BYTES_6;
    if (n_bytes <= 7) return FDCAN_DLC_BYTES_7;
    if (n_bytes <= 8) return FDCAN_DLC_BYTES_8;
    if (n_bytes <= 12) return FDCAN_DLC_BYTES_12;
    if (n_bytes <= 16) return FDCAN_DLC_BYTES_16;
    if (n_bytes <= 20) return FDCAN_DLC_BYTES_20;
    if (n_bytes <= 24) return FDCAN_DLC_BYTES_24;
    if (n_bytes <= 32) return FDCAN_DLC_BYTES_32;
    if (n_bytes <= 48) return FDCAN_DLC_BYTES_48;
    if (n_bytes <= 64) return FDCAN_DLC_BYTES_64;
    return 0;
}