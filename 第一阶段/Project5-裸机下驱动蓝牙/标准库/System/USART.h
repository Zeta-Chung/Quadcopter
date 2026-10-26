#ifndef USART_H
#define USART_H
#include "stm32f4xx.h"
#include <stdio.h>

#define USART_RX_RING_SIZE 64 //环形缓冲区的大小
#define USART_RX_LINE_MAX 10  //一条命令的存储空间

void APP_USART_Init(USART_TypeDef* USART);
void APP_USART_NVICInit(USART_TypeDef* USART);
void APP_USART_WriteByte(USART_TypeDef *USARTx, uint8_t data);
void APP_USART_WriteString(USART_TypeDef *USARTx,uint8_t *str);
uint8_t APP_USART_GetCommand(uint8_t *cmd, uint8_t size);
uint8_t APP_USART_ReadByte(USART_TypeDef *USARTx);
#endif
