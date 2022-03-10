#ifndef RADBUS_H
#define RADBUS_H

#include <stdio.h>
#include <stdbool.h>

#include "stm32g0xx_hal.h"

#include "config.h"
#include "main.h"
#include "rgb.h"
#include "logging.h"
#include "buzzer.h"

// Frame Types
#define RB_FT_STATUS 0x01
#define RB_FT_CONTROL 0x02
#define RB_FT_BROADCAST 0x03
#define RB_FT_IDCHANGE 0x04
#define RB_FT_SUBTYPECHANGE 0x05
#define RB_FT_SWSTATUSREQ 0x06
#define RB_FT_SWUPDATEREQ 0x07
#define RB_FT_CONFIG 0x09

// Tag Status
#define RB_TAGSTATUS_AT 0b00000001
#define RB_TAGSTATUS_DT 0b00000010
#define RB_TAGSTATUS_ER 0b00000100
#define RB_TAGSTATUS_TA 0b00000001
#define RB_TAGSTATUS_TD 0b00000010

// Broadcast Messages
#define RB_BROADCAST_ESTOP 0x01

void rbHandleRxMessage(FDCAN_HandleTypeDef *hfdcan);
void rbClaimDeviceID(FDCAN_HandleTypeDef *hfdcan, unsigned int id);

void rbTagAttach(FDCAN_HandleTypeDef *hfdcan, unsigned long tag_id);
void rbTagDetach(FDCAN_HandleTypeDef *hfdcan, unsigned long tag_id);

// Interrupt Callbacks
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef * hfdcan, uint32_t RxFifo0ITs);

// Helper/Utility Functions
unsigned int rbDLC2nBytes(unsigned int dlc);
unsigned int rbNBytes2DLC(unsigned int n_bytes);

#endif // RADBUS_H