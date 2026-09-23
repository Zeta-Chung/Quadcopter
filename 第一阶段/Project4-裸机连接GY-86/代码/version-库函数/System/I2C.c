#include "I2C.h"

/***/
//@brief 初始化I2C
//@param I2Cx - 需要初始化的I2C外设
/***/
void App_I2C_Init(I2C_TypeDef *I2Cx){
	//使能时钟和配置GPIO
	if(I2Cx == I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		//设置GPIO复用功能
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                            //设置引脚复用
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;                          //开漏输出
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                            //内部上拉
		GPIO_Init(GPIOB,&GPIO_InitStruct);
	}
	else if(I2Cx == I2C2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_I2C2);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
	}
	else if(I2Cx == I2C3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_I2C3);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_I2C3);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
	}
	else return;
	//初始化I2C
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed = 100000;                               //设置时钟速度为100kHz
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;                              //使能Ack应答
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;                       //占空比为2
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//设置地址位数为7位
	I2C_Init(I2Cx,&I2C_InitStruct);
	
	//使能I2C
	I2C_Cmd(I2Cx,ENABLE);
}
/***/
//@brief 使用I2C读取数据
//@param I2Cx - 读取数据的I2C外设
/***/
int8_t APP_I2C_ReceiveData(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
    uint16_t i;
    uint32_t timeout;

    // 1. 参数检查
    if (pBuffer == NULL || Size == 0) {
        return -1;
    }

    //2. 等待总线空闲
    timeout = 100000;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
        if (--timeout == 0) {
            return -2;  // 总线忙超时
        }
    }

    //3. 发送起始位
    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 100000;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET) {
        if (--timeout == 0) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -3;  // 起始位超时
        }
    }

    //4. 发送从机地址（读方向）
    I2C_Send7bitAddress(I2Cx, Addr, I2C_Direction_Receiver);

    // 等待地址发送完成
    timeout = 100000;
    while (1) {
        // 先检查 AF（寻址失败）
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -4;  // 寻址失败
        }
        // 检查 ADDR（寻址成功）
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
            I2C_ReadRegister(I2Cx, I2C_Register_SR1);
            I2C_ReadRegister(I2Cx, I2C_Register_SR2);
            break;
        }
        if (--timeout == 0) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -5;  // 寻址超时
        }
    }

    //5. 接收数据
    // 如果只接收 1 个字节，提前关闭 ACK
    if (Size == 1) {
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
    }

    for (i = 0; i < Size; i++) {
        // 如果是最后一个字节，在接收前确保 ACK 已关闭
        if (i == Size - 1) {
            I2C_AcknowledgeConfig(I2Cx, DISABLE);
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }

        // 等待 RXNE 标志（数据已收到）
        timeout = 100000;
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
                return -6;  // 接收超时
            }
        }
        // 读取数据（硬件自动清除 RXNE）
        pBuffer[i] = I2C_ReceiveData(I2Cx);
    }
    I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK 供下次使用
    return 0;  // 成功
}

