#include "TIM.h"

void APP_TIM2_PWM_Init(void){
	//初始化GPIOA和TIM2时钟
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//--配置TIM2_CH1-4的引脚(配置PA0-3)--//
	//设置为复用输出
	for(int i=0;i<4;i++){
		GPIOA->MODER &= ~(3U << (i*2));
		GPIOA->MODER |= (2U << (i*2));
	}
	//设置为推挽输出
	for(int i=0;i<4;i++){
		GPIOA->OTYPER &= ~(1U << i);
	}
	//设置速度为50MHz
	for(int i=0;i<4;i++){
		GPIOA->OSPEEDR &= ~(3U << (i * 2));
		GPIOA->OSPEEDR |=  (2U << (i * 2));
	}
	//设置端口上拉
	for(int i=0;i<4;i++){
		GPIOA->PUPDR &= ~(3U << (i * 2));
		GPIOA->PUPDR |= (1U << (i * 2));
	}
	
	//设置AF1复用映射
	for(int i=0;i<4;i++){
		GPIOA->AFR[0] &= ~(0xFU << (i*4));
		GPIOA->AFR[0] |= (1U << (i*4));
	}
	//--配置TIM2时基--//
	//设置频率为50Hz
	TIM2->PSC = 84-1;
	TIM2->ARR = 20000-1;
	//设置PWM初始脉宽为1000μs
	TIM2->CCR1 = 1000;
	TIM2->CCR2 = 1000;
	TIM2->CCR3 = 1000;
	TIM2->CCR4 = 1000;
	
	//--配置PWM1模式--//
	//配置通道1和通道2
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);//配置通道输出
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC2M);//清零模式位
	TIM2->CCMR1 |= (6U << 4 | 6U << 12);//配置模式位
	TIM2->CCMR1 |= (TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE);//配置预加载
	//配置通道3和通道4
	TIM2->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_CC4S);
	TIM2->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC4M);
	TIM2->CCMR2 |= (6U << 4 | 6U << 12);
	TIM2->CCMR2 |= (TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE);
	//使能输出模式
	TIM2->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC2NP | TIM_CCER_CC3NP | TIM_CCER_CC4NP);//清零CCxNP位
	TIM2->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);//使能四通道输出
	
	//--使能TIM2--//
	TIM2->CR1 |= TIM_CR1_CEN;
}

