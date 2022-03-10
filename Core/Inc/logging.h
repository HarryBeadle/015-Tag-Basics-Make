#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define LOG_SET 1
#define LOG_CLEAR 0

#define LOG_RGB
#define LOG_RADBUS
#define LOG_TAG

typedef struct {
    struct { // radbus
        int deviceIdClaimed;
        int attach;
        int detach;
        int estop;
        int sw_request;
        int sw_response;
        int tag_accept;
        int tag_decline;
    } radbus;
    struct { // tag
        long int id;
    } tag;
    struct { // rgb
        int green;
        int red;
        int blue;
        int cyan;
        int yellow;
        int magenta;
        int black;
        int white;
    } rgb;
} log_t;

extern log_t globalLog;

void logTick(void);

#endif // LOGGING_H