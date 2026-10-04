#include "I2C.h"

/***/
//@brief 初始化I2C
//@param I2Cx - 需要初始化的I2C外设
/***/
void APP_I2C_Init(I2C_TypeDef *I2Cx){
	//使能时钟和配置GPIO
	if(I2Cx == I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		//设置GPIO复用功能
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                 //设置引脚复用
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;               //开漏输出
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                 //内部上拉
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
	I2C_StructInit(&I2C_InitStruct);
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
//@brief 使用I2C发送数据
//@param I2Cx - 发送数据的I2C外设
//@param Addr - 接收数据的从机地址
//@param pBuffer - 发送数据缓冲区指针
//@param Size - 需要发送的字节数
//@retval 0  - 发送正常
//@retval -1 - 参数非法
//@retval -2 - 总线忙超时
//@retval -3 - 发送起始位超时
//@retval -4 - 发送数据时TXE超时
//@retval -5 - 寻址失败
//@retval -6 - 寻址超时
//@retval -7 - 发送数据时从机NACK
//@retval -8 - 发送数据超时
/***/
int8_t APP_I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pData, uint16_t Size)
{
    uint16_t i;
    uint32_t timeout;

    if (pData == NULL || Size == 0) return -1;//参数错误

    timeout = 100000;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
        if (--timeout == 0) return -2;  // 总线忙超时
    }


    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 100000;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET) {  // 等待 SB 置位
        if (--timeout == 0) return -3;//发送起始位失败
    }

    // 清除可能残留的 AF 标志
    I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    I2C_Send7bitAddress(I2Cx, Addr << 1, I2C_Direction_Transmitter);  

    // 等待地址发送完成（ADDR 置位 或 AF 置位）
    timeout = 100000;
    while (1) {
        // 先检查 AF（寻址失败）
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -5; 
        }
        // 再检查 ADDR（寻址成功）
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
            I2C_ReadRegister(I2Cx, I2C_Register_SR1); 
            I2C_ReadRegister(I2Cx, I2C_Register_SR2); 
            break;
        }
        if (--timeout == 0) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -6;  // 地址发送超时
        }
    }
	//开始发送数据
    for (i = 0; i < Size; i++) {
        // 等待 TXE 置位（发送数据寄存器空）
        timeout = 100000;
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == RESET) {
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                return -4;  // TXE 超时
            }
        }
        // 写入数据
        I2C_SendData(I2Cx, pData[i]);

        // 等待字节发送完成（BTF = 1 或 AF 标志置位）
        timeout = 100000;
        while (1) {
            // 检查从机是否应答失败
            if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
                I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
                I2C_GenerateSTOP(I2Cx, ENABLE);
                return -7;  // 从机 NACK
            }
            // 检查字节传输完成（TXE=1 且 BTF=1）
            if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) {
                break;
            }
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                return -8;  // 发送超时
            }
        }
    } 

    I2C_GenerateSTOP(I2Cx, ENABLE);
    return 0;  // 成功
}



/***/
//@brief 使用I2C读取数据
//@param I2Cx - 读取数据的I2C外设
//@param Addr - 发送数据的从机地址
//@param pBuffer - 接收数据缓冲区指针
//@param Size - 期望接收的字节数
//@retval 0  - 接收正常
//@retval -1 - 输入参数非法
//@retval -2 - 总线忙超时
//@retval -3 - 起始位发送超时
//@retval -4 - 寻址失败
//@retval -5 - 寻址超时
//@retval -6 - 接收数据超时
//@retval -7 - 等待BTF超时
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
    I2C_Send7bitAddress(I2Cx, Addr << 1, I2C_Direction_Receiver);

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
            //接收一个字节时，先配置好NACK和STOP，再清除ADDR
            if(Size == 1){
                I2C_AcknowledgeConfig(I2Cx,DISABLE);
                I2C_GenerateSTOP(I2Cx,ENABLE);
                I2C_ReadRegister(I2Cx, I2C_Register_SR1);
                I2C_ReadRegister(I2Cx, I2C_Register_SR2);
            }
            //接收两个字节时，先配置好NACK再清除ADDR
            else if(Size == 2){
                I2C_AcknowledgeConfig(I2Cx,DISABLE);//关闭ACK
                I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Next);
                I2C_ReadRegister(I2Cx, I2C_Register_SR1);
                I2C_ReadRegister(I2Cx, I2C_Register_SR2);
                break;
            }
            //其余情况直接清除ADDR
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
    // 只接收1个字节
    if (Size == 1) {
        I2C_GenerateSTOP(I2Cx, ENABLE);//设置停止位
        timeout = 100000;
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
                return -6;  // 接收超时
            }
        }
        // 读取数据（硬件自动清除 RXNE）
        pBuffer[0] = I2C_ReceiveData(I2Cx);
    }

    //接收2个字节
    else if(Size == 2)
    {
        //等待BTF置位
        timeout = 100000;
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
                return -7;  // BTF置位超时
            }
        }
        //设置STOP
        I2C_GenerateSTOP(I2Cx,ENABLE);
        //接收数据
        for(i=0;i<2;i++)
        {
            timeout = 100000;
            while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
                if (--timeout == 0) {
                    I2C_GenerateSTOP(I2Cx, ENABLE);
                    I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
                    return -6;  // 接收超时
                }
            }
            // 读取数据（硬件自动清除 RXNE）
            pBuffer[i] = I2C_ReceiveData(I2Cx);
        }
        //恢复NACK的Current
        I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
    }

    //接收三个及以上字节
    else
    {
        for (i = 0; i < Size-1; i++) {
            // 倒数第二个字节前关闭ACK，连续读取DR和移位寄存器(转移到DR)中的数据
            if (i == Size - 2) {
                //等待BTF置位
                timeout = 100000;
                while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
                    if (--timeout == 0) {
                        I2C_GenerateSTOP(I2Cx, ENABLE);
                        I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
                        return -7;                            // BTF置位超时
                    }
                }
                //设置NACK
                I2C_AcknowledgeConfig(I2Cx,DISABLE);
                I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
                //接收数据
                for(uint8_t j=i;j<i+2;j++)
                {
                    if(j == i+1)
                        I2C_GenerateSTOP(I2Cx,ENABLE);
                    timeout = 100000;
                    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
                        if (--timeout == 0) {
                            I2C_GenerateSTOP(I2Cx, ENABLE);
                            I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
                            I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);//恢复POS
                            return -6;  // 接收超时
                        }
                    }
                    pBuffer[j] = I2C_ReceiveData(I2Cx);
                }
                break;
            }
            timeout = 100000;
            while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
                if (--timeout == 0) {
                    I2C_GenerateSTOP(I2Cx, ENABLE);
                    I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
                    return -6;  // 接收超时
                }
            }
            pBuffer[i] = I2C_ReceiveData(I2Cx);
        }
    }
    I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK 供下次使用
    return 0;  // 成功
}
/***/
//@brief 使用I2C读取从机寄存器数据
//@param I2Cx - 读取数据的I2C外设
//@param SlaveAddr - 接收数据的从机地址
//@param RegAddr - 接收数据的寄存器地址
//@param pBuffer - 接收数据缓冲区指针
//@param Size - 期望接收的字节数
//@retval 0  - 接收正常
//@retval -1 - 输入参数非法
//@retval -2 - 总线忙超时
//@retval -3 - 起始位发送超时
//@retval -4 - 寻址失败
//@retval -5 - 寻址超时
//@retval -6 - 接收数据超时
//@retval -7 - 等待BTF超时
//@retval -8  - 发送寄存器地址等待超时
//@retval -9  - 发送寄存器地址NACK
//@retval -10 - 发送寄存器地址超时
/***/
int8_t APP_I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *pBuffer, uint16_t Size)
{
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

    //4. 发送从机地址（写方向）
    I2C_Send7bitAddress(I2Cx, SlaveAddr << 1, I2C_Direction_Transmitter);

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
	
	//5. 发送具体寄存器地址
	timeout = 100000;
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == RESET) {
		if (--timeout == 0) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -8;  // TXE 超时
		}
	}
	I2C_SendData(I2Cx,RegAddr);
	timeout = 100000;
	while (1) 
	{
		// 检查从机是否应答失败
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -9;  // 从机 NACK
		}
		// 检查字节传输完成（TXE=1 且 BTF=1）
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) {
			break;
		}
		if (--timeout == 0) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -10;  // 发送超时
		}
	}
	//6.重新发送起始位
	I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 100000;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET) {
        if (--timeout == 0) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -3;  // 起始位超时
        }
    }
	//7.重新发送从机地址
	I2C_Send7bitAddress(I2Cx, SlaveAddr << 1, I2C_Direction_Receiver);

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
			//接收一个字节时，先配置好NACK和STOP，再清除ADDR
			if(Size == 1){
				I2C_AcknowledgeConfig(I2Cx,DISABLE);
				I2C_GenerateSTOP(I2Cx,ENABLE);
				I2C_ReadRegister(I2Cx, I2C_Register_SR1);
				I2C_ReadRegister(I2Cx, I2C_Register_SR2);
			}
			//接收两个字节时，先配置好NACK再清除ADDR
			else if(Size == 2){
				I2C_AcknowledgeConfig(I2Cx,DISABLE);//关闭ACK
				I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Next);
				I2C_ReadRegister(I2Cx, I2C_Register_SR1);
				I2C_ReadRegister(I2Cx, I2C_Register_SR2);
				break;
			}
			//其余情况直接清除ADDR
            I2C_ReadRegister(I2Cx, I2C_Register_SR1);
            I2C_ReadRegister(I2Cx, I2C_Register_SR2);
            break;
        }
        if (--timeout == 0) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -5;  // 寻址超时
        }
    }
    //8. 接收数据
    // 只接收1个字节
    if (Size == 1) {
		I2C_GenerateSTOP(I2Cx, ENABLE);//设置停止位
		timeout = 100000;
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
            if (--timeout == 0) {
                I2C_GenerateSTOP(I2Cx, ENABLE);
                I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
                return -6;  // 接收超时
            }
        }
		// 读取数据（硬件自动清除 RXNE）
        pBuffer[0] = I2C_ReceiveData(I2Cx);
		
    }
	//接收2个字节
	else if(Size == 2)
	{	
		//等待BTF置位
		timeout = 100000;
		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
			if (--timeout == 0) {
				I2C_GenerateSTOP(I2Cx, ENABLE);
				I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
				return -7;  // BTF置位超时
			}
		}
		//设置STOP
		I2C_GenerateSTOP(I2Cx,ENABLE);
		//接收数据
		for(uint8_t i=0;i<2;i++)
		{
			timeout = 100000;
			while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
				if (--timeout == 0) {
					I2C_GenerateSTOP(I2Cx, ENABLE);
					I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
					return -6;  // 接收超时
				}
			}
			// 读取数据（硬件自动清除 RXNE）
			pBuffer[i] = I2C_ReceiveData(I2Cx);
		}
		//恢复NACK的Current
		I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
	}
	//接收三个及以上字节
	else
	{	
		for (uint8_t i = 0; i < Size-1; i++) {
			// 倒数第二个字节前关闭ACK，连续读取DR和移位寄存器(转移到DR)中的数据
			if (i == Size - 2) {
				//等待BTF置位
				timeout = 100000;
				while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
					if (--timeout == 0) {
						I2C_GenerateSTOP(I2Cx, ENABLE);
						I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK
						return -7;  						  // BTF置位超时
					}
				}
				//设置NACK
				I2C_AcknowledgeConfig(I2Cx,DISABLE);
				I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
				//接收数据
				for(uint8_t j=i;j<i+2;j++)
				{	
					//在接收最后一个字节前配置STOP
					if(j == i+1)
						I2C_GenerateSTOP(I2Cx,ENABLE);
					//等待RxNE
					timeout = 100000;
					while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
						if (--timeout == 0) {
							I2C_GenerateSTOP(I2Cx, ENABLE);
							I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
							I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);//恢复POS
							return -6;  // 接收超时
						}
					}
					pBuffer[j] = I2C_ReceiveData(I2Cx);
				}
				break;
			}
			timeout = 100000;
			while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
				if (--timeout == 0) {
					I2C_GenerateSTOP(I2Cx, ENABLE);
					I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复 ACK
					return -6;  // 接收超时
				}
			}
			pBuffer[i] = I2C_ReceiveData(I2Cx);
		}
	}
    I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复 ACK 供下次使用
    return 0;  // 成功
}
