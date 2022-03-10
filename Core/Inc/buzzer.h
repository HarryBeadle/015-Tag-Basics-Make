#ifndef BUZZER_H
#define BUZZER_H

#include <stdio.h>
#include <stdbool.h>

#include "stm32g0xx_hal.h"

#include "main.h"
#include "notes.h"

#define MAX_TUNE_SIZE 8

typedef enum {bzWaiting, bzPlaying, bzPausing} bzState_t;


typedef struct {
    unsigned int frequency;
    unsigned int duration;
    unsigned int pause;
} note_t;

typedef struct {
    note_t notes[MAX_TUNE_SIZE];
    unsigned int index;
    unsigned int size;
    bool played;
} tune_t;

typedef struct {
    bzState_t state;
    tune_t* tune;
} bzStatus_t;

extern bzStatus_t bzStatus;

extern tune_t bzAttachTune, bzDetachTune, bzDeclineTune;

int bzInit(void);
int bzPlayTune(tune_t* tune);
int bzPlayNote(note_t* note);
int bzPlayFrequency(unsigned int frequency);
int bzStartDurationTimer(void);
int bzStop(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif // BUZZER_H