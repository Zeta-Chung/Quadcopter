#include "USART.h"


char usart_cmd_line[64];           //存储读出的一条含转义字符的命令
uint8_t usart_cmd_len = 0;          //一条命令的长度
extern volatile uint8_t usart_rx_ring[64];//存储输入数据的环形缓冲区
extern volatile uint8_t usart_rx_write;//写数据的索引
extern volatile uint8_t usart_rx_read;//读数据的索引

/////
//@brief 初始化USART外设
//@param USARTx - 需要初始化的USART外设
/////
void APP_USART_Init(USART_TypeDef* USARTx){
	if(USARTx == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		//设置GPIO复用功能
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
		//初始化GPIO口
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	}
	else if(USARTx == USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		//设置GPIO复用功能
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
		//初始化GPIO口
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	}
	else if(USARTx == USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		//设置GPIO复用功能
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
		//初始化GPIO口
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
	}
	
	USART_InitTypeDef US;
	US.USART_BaudRate = 115200;
	US.USART_StopBits = USART_StopBits_1;
	US.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	US.USART_Parity = USART_Parity_No;
	US.USART_WordLength = USART_WordLength_8b;
	US.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx,&US);
	    
	USART_Cmd(USARTx,ENABLE);
}

void APP_USART_NVICInit(USART_TypeDef *USARTx){
	NVIC_InitTypeDef NVIC_InitStruct;
	if(USARTx == USART1){
		NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;

	}
	else if(USARTx == USART2){
		NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	}
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

/////
//@brief USART写单字节
//@param USARTx - 执行的USART外设
//@param data - 需要写入的内容
/////
void USART_WriteByte(USART_TypeDef *USARTx, uint8_t data)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, data);
	while (USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}
/////
//@brief USART写字符串
//@param USARTx - 执行的USART外设
//@param str - 需要写入的字符串
/////
void USART_WriteString(USART_TypeDef *USARTx,uint8_t *str)
{
	while(*str != '\0'){
		USART_WriteByte(USARTx,*str++);
	}
}
/////
//@brief USART读单字节
//@param USARTx - 执行的USART外设
/////
uint8_t USART_ReadByte(USART_TypeDef *USARTx){
	while(USART_GetFlagStatus(USARTx,USART_FLAG_RXNE)==RESET);
	return (uint8_t)USART_ReceiveData(USARTx);
}
/////
//@brief USART处理读到的指令信息
//@param cmd - 命令字符串
//@param size - 命令长度
//@retval 0 - 读取到有效信息
//@retval 1 - 读取信息为空 
/////
uint8_t USART_GetCommand(uint8_t *cmd, uint8_t size)
{
	while(usart_rx_write != usart_rx_read)
	{
		uint8_t c = usart_rx_ring[usart_rx_read];
		usart_rx_read = (uint16_t)((usart_rx_read + 1) % 64);
		//到命令末尾时
		if(c == '\r' || c == '\n')
		{
			if(usart_cmd_len > 0)
			{
				uint8_t i;
				for(i = 0; i < usart_cmd_len && i < size - 1; i++)
					cmd[i] = usart_cmd_line[i];
				cmd[i] = '\0';
				usart_cmd_len = 0;
				return 0;
			}
		}
		//未到末尾且未超出最大长度时
		else if(usart_cmd_len <USART_RX_LINE_MAX - 1)
		{
			usart_cmd_line[usart_cmd_len++] = c;
		}
	}
	return 1;
}

