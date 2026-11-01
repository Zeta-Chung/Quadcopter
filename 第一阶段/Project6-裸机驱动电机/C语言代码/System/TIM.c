#include "TIM.h"
/***/
//@brief 初始化TIM2_CH1的50Hz PWM输出（PA0，单电机测试）
/***/
void APP_TIM2_PWMOutput_Init(void){
	//初始化GPIOA和TIM2时钟
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//--配置TIM2_CH1-4的引脚(配置PA0-3)--//
	//设置为复用模式
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
	//使计数从0开始
	TIM2->CNT = 0;
	//设置PWM初始脉宽为1000μs
	TIM2->CCR1 = 1000;
	
	//--配置PWM1模式--//
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
	TIM2->CCMR1 |= (6U << 4) | TIM_CCMR1_OC1PE;
	//使能输出模式
	TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP |
	                TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);
	TIM2->EGR = TIM_EGR_UG; //立即装载预分频和预装载值
	TIM2->CCER |= TIM_CCER_CC1E;
	
	//--使能TIM2--//
	TIM2->CR1 |= TIM_CR1_CEN;
}

/***/
//@brief 初始化TIM1的PWM输入（用于接收遥控器信号）
/***/
void APP_TIM1_PWMInput_Init(void){
	//使能GPIOA和TIM1时钟
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	//---初始化GPIO引脚---//
	//设置PA8复用模式
	GPIOA->MODER &= ~(3U << (8*2));
	GPIOA->MODER |= (2U << (8*2));
	//设置推挽输出
	GPIOA->OTYPER &= ~(1U << 8);
	//设置速度为50MHz
	GPIOA->OSPEEDR &= ~(3U << (8*2));
	GPIOA->OSPEEDR |=  (2U << (8*2));
	//设置上拉输入
	GPIOA->PUPDR &= ~(3U << (8*2));
	GPIOA->PUPDR |= (1U << (8*2));
	//设置AF1复用映射
	GPIOA->AFR[1] &= ~(0xFU << (0*4));
	GPIOA->AFR[1] |= (1U << (0*4));
	
	//---初始化时基单元---//
	TIM1->PSC = 84-1;//设置分频系数为84
	TIM1->ARR = 0xFFFF;//ARR设为最大值，防止测量时间溢出归零
	TIM1->CNT = 0;
	
	//---配置通道1（捕获上升沿）---//
	//设置输入，映射到通道1
	TIM1->CCMR1 &= ~(TIM_CCMR1_CC1S);
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;
	//设置滤波
	TIM1->CCMR1 &= ~(TIM_CCMR1_IC1F);
	TIM1->CCMR1 |= (TIM_CCMR1_IC1F);
	//设置预分频系数为1
	TIM1->CCMR1 &= ~(TIM_CCMR1_IC1PSC);
	//设置上升沿触发
	TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
	//---配置通道2（捕获下降沿）---//
	//设置输入，映射到通道1
	TIM1->CCMR1 &= ~(TIM_CCMR1_CC2S);
	TIM1->CCMR1 |= TIM_CCMR1_CC2S_1;
	//设置滤波
	TIM1->CCMR1 &= ~(TIM_CCMR1_IC2F);
	TIM1->CCMR1 |= TIM_CCMR1_IC2F;
	//设置预分频系数为1
	TIM1->CCMR1 &= ~(TIM_CCMR1_IC2PSC);
	//设置下降沿触发
	TIM1->CCER &= ~(TIM_CCER_CC2NP);
	TIM1->CCER |= TIM_CCER_CC2P;
	
	//---使能捕获通道---//
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	
	//---配置从模式控制器（捕获信号）---//
	//设置从模式为复位模式
	TIM1->SMCR &= ~(TIM_SMCR_SMS);
	TIM1->SMCR |= TIM_SMCR_SMS_2;
	//设置TRGI为TI1FP1
	TIM1->SMCR &= ~(TIM_SMCR_TS);
	TIM1->SMCR |= (TIM_SMCR_TS_0 | TIM_SMCR_TS_2);
	
	//---配置定时器中断---//
	//只使能通道1捕获中断；通道2捕获值由中断读取CCR2
	TIM1->EGR = TIM_EGR_UG; //立即装载预分频值
	TIM1->SR = 0U;
	TIM1->DIER |= TIM_DIER_CC1IE;
	//NVIC配置
	NVIC_EnableIRQ(TIM1_CC_IRQn);
	
	//---启动定时器--//
	//使能定时器计数
	TIM1->CR1 |= TIM_CR1_CEN;
}

