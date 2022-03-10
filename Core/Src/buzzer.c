#include "buzzer.h"

tune_t bzAttachTune, bzDetachTune, bzDeclineTune;

bzStatus_t bzStatus;

int bzInit(void)
{
    // Attach Tune
    bzAttachTune.notes[0].frequency = NOTE_B5;
    bzAttachTune.notes[0].duration = 80;
    bzAttachTune.notes[0].pause = 80;
    bzAttachTune.notes[1].frequency = NOTE_Db6;
    bzAttachTune.notes[1].duration = 80;
    bzAttachTune.notes[1].pause = 5;
    bzAttachTune.notes[2].frequency = NOTE_Gb6;
    bzAttachTune.notes[2].duration = 80;
    bzAttachTune.notes[2].pause = 40;
    bzAttachTune.index = 0;
    bzAttachTune.size = 3;
    bzAttachTune.played = false;
    // Decline Tune
    bzDeclineTune.notes[0].frequency = NOTE_G5;
    bzDeclineTune.notes[0].duration = 80;
    bzDeclineTune.notes[0].pause = 80;
    bzDeclineTune.notes[1].frequency = NOTE_G5;
    bzDeclineTune.notes[1].duration = 80;
    bzDeclineTune.notes[1].pause = 5;
    bzDeclineTune.notes[2].frequency = NOTE_G4;
    bzDeclineTune.notes[2].duration = 80;
    bzDeclineTune.notes[2].pause = 40;
    bzDeclineTune.index = 0;
    bzDeclineTune.size = 3;
    bzDeclineTune.played = false;
    // Detach Tune`
    bzDetachTune.notes[0].frequency = NOTE_G5;
    bzDetachTune.notes[0].duration = 80;
    bzDetachTune.notes[0].pause = 80;
    bzDetachTune.notes[1].frequency = NOTE_E5;
    bzDetachTune.notes[1].duration = 80;
    bzDetachTune.notes[1].pause = 5;
    bzDetachTune.notes[2].frequency = NOTE_C5;
    bzDetachTune.notes[2].duration = 80;
    bzDetachTune.notes[2].pause = 40;
    bzDetachTune.index = 0;
    bzDetachTune.size = 3;
    bzDetachTune.played = false;
    return 0;
}

int bzPlayNote(note_t *note)
{
    // Play Note
    int period = note->frequency;
    int compare = period / 2;
    __HAL_TIM_SET_AUTORELOAD(&htim17, period); // Set Period
    __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, compare); // 50% Duty
    HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1); // Start PWM
    // Set Duration
    __HAL_TIM_SET_AUTORELOAD(&htim16, note->duration);
    __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&htim16);
    return 0;
}

int bzPlayTune(tune_t *tune)
{
    bzStatus.state = bzPlaying;
    bzStatus.tune = tune;
    tune->index = 0;
    tune->played = false;
    bzPlayNote(&(tune->notes[0]));
    return 0;
}

int bzStop(void)
{
    HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
    return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim16) {
        HAL_TIM_Base_Stop_IT(&htim16);
        bzStop();
        switch (bzStatus.state) {
            case bzPlaying:
                bzStatus.state = bzPausing;
                __HAL_TIM_SET_AUTORELOAD(&htim16, bzStatus.tune->notes[bzStatus.tune->index].pause);
                __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
                HAL_TIM_Base_Start_IT(&htim16);
                break;
            case bzPausing:
                if (bzStatus.tune->index < bzStatus.tune->size) {
                    bzStatus.state = bzPlaying;
                    bzPlayNote(&(bzStatus.tune->notes[++bzStatus.tune->index]));
                } else {
                    bzStatus.state = bzWaiting;
                }
                break;
            default:
                break;
        }
    }
}