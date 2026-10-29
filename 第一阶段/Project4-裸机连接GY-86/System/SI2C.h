#ifndef SI2C_H
#define SI2C_H

#include "stm32f4xx.h"
#include <stdio.h>
/**
  * @brief  软件I2C结构体，保存SCL与SDA对应的端口和引脚
  * @note   每个使用中的软件I2C实例应定义一个本结构体变量
  */
typedef struct
{
    GPIO_TypeDef *SCL_GPIOx;   /* SCL所在GPIO端口 */
    uint16_t      SCL_GPIO_Pin; /* SCL所在引脚 */
    GPIO_TypeDef *SDA_GPIOx;   /* SDA所在GPIO端口 */
    uint16_t      SDA_GPIO_Pin; /* SDA所在引脚 */
} SI2C_TypeDef;

/**
  * @brief  软件I2C初始化，将SCL与SDA引脚配置为开漏输出模式
  * @param  hi2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_Init(SI2C_TypeDef *hi2c);

/**
  * @brief  向SCL写单比特数据
  * @param  hi2c 软件I2C结构体指针
  * @param  level 目标电平，0为拉低，1为释放为高电平
  * @return 无
  */
void SI2C_SCL_Write(SI2C_TypeDef *hi2c, uint8_t level);

/**
  * @brief  向SDA写单比特数据
  * @param  hi2c 软件I2C结构体指针
  * @param  level 目标电平，0为拉低，1为释放为高电平
  * @retval 无
  */
void SI2C_SDA_Write(SI2C_TypeDef *hi2c, uint8_t level);

/**
  * @brief  读取SDA的值
  * @param  hi2c 软件I2C结构体指针
  * @return 返回0表示SDA为低电平，返回1表示SDA为高电平
  */
uint8_t SI2C_SDA_Read(SI2C_TypeDef *hi2c);

/**
  * @brief  发送起始位
  * @param  hi2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_SendStart(SI2C_TypeDef *hi2c);

/**
  * @brief  发送停止位
  * @param  hi2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_SendStop(SI2C_TypeDef *hi2c);

/**
  * @brief  发送单个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  byte 待发送的8位数据
  * @return 返回从机应答：0表示收到ACK，1表示未收到ACK(NACK)
  */
uint8_t SI2C_SendByte(SI2C_TypeDef *hi2c, uint8_t byte);

/**
  * @brief  读取单个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  ACK 应答控制，1发送ACK，0发送NACK
  * @return 返回接收到的8位数据
  */
uint8_t SI2C_ReceiveByte(SI2C_TypeDef *hi2c, uint8_t ACK);

/**
  * @brief  向指定从机发送多个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  Addr 7位从机地址
  * @param  pData 待发送数据缓冲区指针
  * @param  Size 待发送字节数
  * @return 0表示正常，1表示寻址失败，-1表示参数错误，-2表示数据发送失败
  */
int8_t SI2C_SendBuffer(SI2C_TypeDef *hi2c, uint8_t Addr,const uint8_t *pData, uint16_t Size);

/**
  * @brief  从指定从机接收多个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  Addr 7位从机地址
  * @param  pBuffer 接收数据缓冲区指针
  * @param  Size 期望接收字节数
  * @return 0表示正常，1表示寻址失败，-1表示参数错误
  */
int8_t SI2C_ReceiveBuffer(SI2C_TypeDef *hi2c, uint8_t Addr, uint8_t *pBuffer, uint16_t Size);
/**
  * @brief  读取从机寄存器数据
  * @param  hi2c 软件I2C结构体指针
  * @param  SlaveAddr 从机地址
  * @param  RegAddr   寄存器地址
  * @param  pBuffer 接收数据缓冲区指针
  * @param  Size 期望接收字节数
  * @return 0表示正常，1表示寻址失败，2表示发送寄存器失败，3表示读取数据失败
  */
uint8_t SI2C_ReadReg(SI2C_TypeDef *hi2c,uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *pbuffer,uint16_t Size);
#endif
