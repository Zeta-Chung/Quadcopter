#include "motor.h"
/***/
//@brief 设置舵机初始化
/***/
void APP_Motor_Init(void){
	APP_TIM2_PWM_Init();
}
/***/
//@brief 向电机写入新的脉宽
//@param id - 需要写入脉宽的PWM通道
//@param pulse - 需要写入的脉宽
/***/
void APP_Motor_SetPulse(uint8_t id,uint16_t pulse){
	//限定脉宽范围防止电机跑飞
	if(pulse > 2000) pulse = 2000;
	if(pulse < 1000) pulse = 1000;
	//向相应寄存器写入新脉宽
	switch(id){
		case 1:
			TIM2->CCR1 = pulse;
			break;
		case 2:
			TIM2->CCR2 = pulse;
			break;
		case 3:
			TIM2->CCR3 = pulse;
			break;
		case 4:
			TIM2->CCR4 = pulse;
			break;
		default:
			break;
	}
}

/***/
//@brief 向四个PWM通道都写入新的脉宽
//@param pulse1 - PWM1通道需要写入的脉宽
//@param pulse2 - PWM2通道需要写入的脉宽
//@param pulse3 - PWM3通道需要写入的脉宽
//@param pulse4 - PWM4通道需要写入的脉宽
/***/
void APP_Motor_SetAllPulse(uint16_t pulse1, uint16_t pulse2, uint16_t pulse3, uint16_t pulse4){
	APP_Motor_SetPulse(1, pulse1);
	APP_Motor_SetPulse(2, pulse2);
	APP_Motor_SetPulse(3, pulse3);
	APP_Motor_SetPulse(4, pulse4);
}