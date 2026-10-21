#ifndef I2C_H
#define I2C_H
#include "stm32f4xx.h"
#include <stdio.h>

void APP_I2C_Init(I2C_TypeDef *I2Cx);
int8_t APP_I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pData, uint16_t Size);
int8_t APP_I2C_ReceiveData(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pBuffer, uint16_t Size);
int8_t APP_I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *pBuffer, uint16_t Size);
#endif

