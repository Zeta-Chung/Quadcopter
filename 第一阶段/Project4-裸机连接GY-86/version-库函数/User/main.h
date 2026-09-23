/**
  ******************************************************************************
  * @file    User/main.h
  * @brief   STM32F401 工程模板全局头文件（main.c 与 stm32f4xx_it.c 都包含本文件）
  * @note    库头文件统一在这里引入；自定义模块（System / Hardware）的头文件建议
  *          在使用它的那个 .c 里单独 include，不要全部堆到这里，否则改一处就要
  *          重编整个工程。跨文件调用的接口才写进 Exported functions。
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"      /* 内部已包含 core_cm4.h / system_stm32f4xx.h，
                               并且在定义了 USE_STDPERIPH_DRIVER 之后会自动
                               包含 User/stm32f4xx_conf.h */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void Delay(uint32_t nTime);           /* 阻塞式毫秒延时，时基来自 SysTick 中断 */
void TimingDelay_Decrement(void);     /* 供 SysTick_Handler() 调用，应用代码不要直接调 */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) STM32F401 project template - END OF FILE ************************/
