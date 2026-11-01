/**
  ******************************************************************************
  * @file    User/main.c
  * @brief   STM32F401 工程模板主程序
  * @note    复制本模板做新项目时的注意事项：
  *          1. 时钟不需要在 main() 里配置：启动文件 startup_stm32f401xx.s 复位后
  *             先调用 SystemInit()，本工程使用 ST-LINK 8MHz MCO 作为 HSE 输入并经 PLL 倍频，
  *             系统主频 SystemCoreClock = 84MHz。
  *          2. 本文件只保留一个可直接使用的毫秒延时 Delay()；外设驱动请放到
  *             System / Hardware 分组中，不要写在 main.c 里，方便整包复用。
  *          3. 当前按 NUCLEO-F401RE MB1136 C-04 的原厂时钟接法配置：8MHz HSE 旁路。
  *             更换板卡或时钟源时，需同步检查 HSE_VALUE、PLL_M 与 HSEBYP，
  *             否则 Delay() 和定时器 PWM 脉宽都会失准。
  *          4. 需要检查库函数参数时，在 User/stm32f4xx_conf.h 里打开
  *             USE_FULL_ASSERT，文件末尾的 assert_failed() 会自动参与编译。
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "motor.h"
#include "receiver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 毫秒倒计时：由 SysTick 中断每 1ms 减 1，主循环轮询其值。
   必须 volatile（防止编译优化掉轮询）；上电初值由启动代码清零，不必显式赋值 */
static volatile uint32_t uwTimingDelay;

/* Private function prototypes -----------------------------------------------*/
/* 对外接口 Delay() / TimingDelay_Decrement() 已声明在 main.h 中 */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  程序入口
  * @param  None
  * @retval None
  */
int main(void)
{
    uint16_t throttle_us = 1000U;
    uint8_t throttle_valid;

    /* 配置 SysTick 每 1ms 产生一次中断（返回 0 表示成功），为 Delay() 提供时基 */
    SysTick_Config(SystemCoreClock / 1000U);
    APP_Motor_Init(); /* 先在 PA0 输出最低油门，再启动接收机输入捕获 */
    APP_Receiver_Init();

    while (1)
    {

    }
}

/**
  * @brief  阻塞式毫秒延时（依赖 SysTick 中断，调用前必须已配置 SysTick）
  * @param  nTime: 延时时间，单位为 ms
  * @retval None
  */
void Delay(uint32_t nTime)
{
    uwTimingDelay = nTime;

    while (uwTimingDelay != 0U)
    {
    }
}

/**
  * @brief  延时倒计时，由 stm32f4xx_it.c 中的 SysTick_Handler() 调用
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    if (uwTimingDelay != 0U)
    {
        uwTimingDelay--;
    }
}


#ifdef USE_FULL_ASSERT
/**
  * @brief  断言失败时进入此函数，报告出错的源文件和行号
  * @param  file: 源文件名指针
  * @param  line: assert_param 出错的行号
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* 可在此打印信息，例如：printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    while (1)
    {
    }
}
#endif

/************************ (C) STM32F401 project template - END OF FILE ************************/
