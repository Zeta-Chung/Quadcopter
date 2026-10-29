#ifndef MOTOR_H
#define MOTOR_H
#include "TIM.h"

void APP_Servo_Init(void);
void APP_Motor_SetPulse(uint8_t id, uint16_t pulse);
void APP_Motor_SetAllPulse(uint16_t pulse1, uint16_t pulse2, uint16_t pulse3, uint16_t pulse4);
#endif
