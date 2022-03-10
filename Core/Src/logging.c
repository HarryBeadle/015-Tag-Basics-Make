#include "logging.h"

log_t globalLog;

char* logFormatString = "%02x %10s: %s\r\n";
char* logFormatStringNoNewLine = "%02x %10s: %s";

void logTick(void)
{
    int printed = 0;
    #ifdef LOG_RGB
    if (globalLog.rgb.green) {
        printf(logFormatString, globalLog.rgb.green, "RGB", "Green");
        globalLog.rgb.green = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.red) {
        printf(logFormatString, globalLog.rgb.red, "RGB", "Red");
        globalLog.rgb.red = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.blue) {
        printf(logFormatString, globalLog.rgb.blue, "RGB", "Blue");
        globalLog.rgb.blue = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.cyan) {
        printf(logFormatString, globalLog.rgb.cyan, "RGB", "Cyan");
        globalLog.rgb.cyan = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.yellow) {
        printf(logFormatString, globalLog.rgb.yellow, "RGB", "Yellow");
        globalLog.rgb.yellow = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.magenta) {
        printf(logFormatString, globalLog.rgb.magenta, "RGB", "Magenta");
        globalLog.rgb.magenta = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.black) {
        printf(logFormatString, globalLog.rgb.black, "RGB", "Black");
        globalLog.rgb.black = LOG_CLEAR;
        printed++;
    }
    if (globalLog.rgb.white) {
        printf(logFormatString, globalLog.rgb.white, "RGB", "White");
        globalLog.rgb.white = LOG_CLEAR;
        printed++;
    }
    #endif // LOG_RGB
    #ifdef LOG_RADBUS
    if (globalLog.radbus.deviceIdClaimed) {
        printf(logFormatStringNoNewLine, 0, "RADBUS", "Device ID Claimed: ");
        printf("%02x\r\n", globalLog.radbus.deviceIdClaimed);
        globalLog.radbus.deviceIdClaimed = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.attach) {
        printf(logFormatStringNoNewLine, globalLog.radbus.attach, "RADBUS", "TX Attached: ");
        printf("%08lX\r\n", globalLog.tag.id);
        globalLog.radbus.attach = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.detach) {
        printf(logFormatString, globalLog.radbus.detach, "RADBUS", "TX Detached");
        globalLog.radbus.detach = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.estop) {
        printf(logFormatString, globalLog.radbus.estop, "RADBUS", "TX Emergency Stop");
        globalLog.radbus.estop = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.sw_request) {
        printf(logFormatString, globalLog.radbus.sw_request, "RADBUS", "RX Software Request");
        globalLog.radbus.sw_request = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.sw_response) {
        printf(logFormatString, globalLog.radbus.sw_response, "RADBUS", "TX Software Response");
        globalLog.radbus.sw_response = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.tag_accept) {
        printf(logFormatString, globalLog.radbus.tag_accept, "RADBUS", "RX Tag Accepted");
        globalLog.radbus.tag_accept = LOG_CLEAR;
        printed++;
    }
    if (globalLog.radbus.tag_decline) {
        printf(logFormatString, globalLog.radbus.tag_decline, "RADBUS", "RX Tag Declined");
        globalLog.radbus.tag_decline = LOG_CLEAR;
        printed++;
    }
    #endif
    if (printed) {
        printf("--------------\r\n");
    }
}