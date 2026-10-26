#include "SI2C.h"

/**
  * @brief  软件I2C初始化，将SCL与SDA引脚配置为开漏输出模式
  * @param  hi2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_Init(SI2C_TypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint32_t SCL_RCC, SDA_RCC;

    if (hi2c == NULL)
    {
        return;
    }

    // STM32F401的GPIOA~GPIOE和GPIOH时钟均挂在AHB1总线上
    if      (hi2c->SCL_GPIOx == GPIOA) SCL_RCC = RCC_AHB1Periph_GPIOA;
    else if (hi2c->SCL_GPIOx == GPIOB) SCL_RCC = RCC_AHB1Periph_GPIOB;
    else if (hi2c->SCL_GPIOx == GPIOC) SCL_RCC = RCC_AHB1Periph_GPIOC;
    else if (hi2c->SCL_GPIOx == GPIOD) SCL_RCC = RCC_AHB1Periph_GPIOD;
    else if (hi2c->SCL_GPIOx == GPIOE) SCL_RCC = RCC_AHB1Periph_GPIOE;
    else if (hi2c->SCL_GPIOx == GPIOH) SCL_RCC = RCC_AHB1Periph_GPIOH;
    else                               return;

    if      (hi2c->SDA_GPIOx == GPIOA) SDA_RCC = RCC_AHB1Periph_GPIOA;
    else if (hi2c->SDA_GPIOx == GPIOB) SDA_RCC = RCC_AHB1Periph_GPIOB;
    else if (hi2c->SDA_GPIOx == GPIOC) SDA_RCC = RCC_AHB1Periph_GPIOC;
    else if (hi2c->SDA_GPIOx == GPIOD) SDA_RCC = RCC_AHB1Periph_GPIOD;
    else if (hi2c->SDA_GPIOx == GPIOE) SDA_RCC = RCC_AHB1Periph_GPIOE;
    else if (hi2c->SDA_GPIOx == GPIOH) SDA_RCC = RCC_AHB1Periph_GPIOH;
    else                               return;

    RCC_AHB1PeriphClockCmd(SCL_RCC | SDA_RCC, ENABLE);
	//初始化引脚
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//设置引脚上拉
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//设置输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//设置开漏输出
    GPIO_InitStructure.GPIO_Pin   = hi2c->SCL_GPIO_Pin;
    GPIO_Init(hi2c->SCL_GPIOx, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = hi2c->SDA_GPIO_Pin;
    GPIO_Init(hi2c->SDA_GPIOx, &GPIO_InitStructure);

    SI2C_SCL_Write(hi2c, 1);   // 总线空闲时保持高电平
    SI2C_SDA_Write(hi2c, 1);
}

/**
  * @brief  向SCL写单比特数据
  * @param  hi2c 软件I2C结构体指针
  * @param  level 0为拉低，1为释放为高电平
  * @return 无
  */
void SI2C_SCL_Write(SI2C_TypeDef *hi2c, uint8_t level)
{
    if (hi2c == NULL)
    {
        return;
    }

    if (level == 1)
    {
        GPIO_SetBits(hi2c->SCL_GPIOx, hi2c->SCL_GPIO_Pin);   // 释放为高电平
    }
    else
    {
        GPIO_ResetBits(hi2c->SCL_GPIOx, hi2c->SCL_GPIO_Pin); // 拉低
    }
}

/**
  * @brief  向SDA写单比特数据
  * @param  hi2c 软件I2C结构体指针
  * @param  level 0为拉低，1为释放为高电平
  * @return 无
  */
void SI2C_SDA_Write(SI2C_TypeDef *hi2c, uint8_t level)
{
    if (hi2c == NULL)
    {
        return;
    }

    if (level == 1)
    {
        GPIO_SetBits(hi2c->SDA_GPIOx, hi2c->SDA_GPIO_Pin);   // 释放为高电平
    }
    else
    {
        GPIO_ResetBits(hi2c->SDA_GPIOx, hi2c->SDA_GPIO_Pin); // 拉低
    }
}

/**
  * @brief  读取SDA的值
  * @param  hi2c 软件I2C结构体指针
* @return 返回0表示SDA为低电平，返回1表示SDA为高电平，返回2则表示设置的hi2c有误
  */
uint8_t SI2C_SDA_Read(SI2C_TypeDef *hi2c)
{
    if (hi2c == NULL)
    {
        return 2;
    }

    return GPIO_ReadInputDataBit(hi2c->SDA_GPIOx, hi2c->SDA_GPIO_Pin);
}

/**
  * @brief  发送起始位
  * @param  hi 2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_SendStart(SI2C_TypeDef *hi2c)
{
    SI2C_SDA_Write(hi2c, 1);   // 确保SDA先为高电平
    for(int i=0;i<84*2;i++);
    SI2C_SCL_Write(hi2c, 1);   // 确保SCL先为高电平
    for(int i=0;i<84*2;i++);
    SI2C_SDA_Write(hi2c, 0);   // SDA由高电平拉低，产生起始条件
    for(int i=0;i<84*2;i++);
    SI2C_SCL_Write(hi2c, 0);   // 时钟线拉低，准备发送数据
}

/**
  * @brief  发送停止位
  * @param  hi2c 软件I2C结构体指针
  * @return 无
  */
void SI2C_SendStop(SI2C_TypeDef *hi2c)
{
    SI2C_SCL_Write(hi2c, 0);   // 先拉低SCL，避免SDA模式切换产生额外起始/停止条件
    SI2C_SDA_Write(hi2c, 0);   // 确保SDA先为低电平
    for(int i=0;i<84*2;i++);
    SI2C_SCL_Write(hi2c, 1);   // SCL拉高
    for(int i=0;i<84*2;i++);
    SI2C_SDA_Write(hi2c, 1);   // SDA由低电平拉高，产生停止条件
    for(int i=0;i<84*5;i++);               // 总线空闲时间
}

/**
  * @brief  发送单个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  byte 待发送的8位数据
  * @return 返回从机应答：0表示收到ACK，1表示未收到ACK
  */
uint8_t SI2C_SendByte(SI2C_TypeDef *hi2c, uint8_t byte)
{
    uint8_t i;
    uint8_t ack;

    for (i = 0; i < 8; i++)
    {
        SI2C_SCL_Write(hi2c, 0);               // 拉低时钟，准备发送数据位
        for(int i=0;i<84;i++);
        if (byte & 0x80)                       // 发送当前最高位
        {
            SI2C_SDA_Write(hi2c, 1);
        }
        else
        {
            SI2C_SDA_Write(hi2c, 0);
        }
        for(int i=0;i<84;i++);                          // 给出短暂的建立时间
        SI2C_SCL_Write(hi2c, 1);               // 拉高时钟，数据有效
        for(int i=0;i<84;i++);
        byte <<= 1;                            // 继续发送下一位
    }

    SI2C_SCL_Write(hi2c, 0);                   // 准备修改SDA
    SI2C_SDA_Write(hi2c, 1);                   // 释放数据线
    for(int i=0;i<84;i++);
    SI2C_SCL_Write(hi2c, 1);
    for(int i=0;i<84;i++);
    ack = SI2C_SDA_Read(hi2c);                 // 在第9个时钟的高电平期间采样ACK
    SI2C_SCL_Write(hi2c, 0);                   // 结束ACK时钟，让从机准备下一字节的首位
    return ack;
}

/**
  * @brief  读取单个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  ACK 应答控制，1发送ACK，0发送NACK
  * @return 返回接收到的8位数据
  */
uint8_t SI2C_ReceiveByte(SI2C_TypeDef *hi2c, uint8_t ACK)
{
    uint8_t i;
    uint8_t receive = 0;

    SI2C_SDA_Write(hi2c, 1);                   // 释放SDA，让从机控制
    for (i = 0; i < 8; i++)
    {
        receive <<= 1;                         // 左移，空出最低位
        SI2C_SCL_Write(hi2c, 1);               // 拉高时钟，数据有效
        for(int i=0;i<84;i++);
        if (SI2C_SDA_Read(hi2c))               // 读取SDA电平
        {
            receive |= 0x01;                   // 若为高电平，当前位为1
        }
        SI2C_SCL_Write(hi2c, 0);               // 拉低时钟
        for(int i=0;i<84;i++);
    }

    // 发送ACK或NACK 
    SI2C_SDA_Write(hi2c, ACK ? 0 : 1);         // ACK=1拉低SDA，ACK=0释放SDA
    for(int i=0;i<84;i++);
    SI2C_SCL_Write(hi2c, 1);                   // 拉高时钟，从机读取ACK/NACK
    for(int i=0;i<84;i++);
    SI2C_SCL_Write(hi2c, 0);                   // 拉低时钟完成应答位
    SI2C_SDA_Write(hi2c, 1);                   // 释放SDA
    return receive;
}

/**
  * @brief  向指定从机发送多个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  Addr 7位从机地址
  * @param  pData 待发送数据缓冲区指针
  * @param  Size 待发送字节数
  * @return 0表示正常，1表示寻址失败，-1表示参数错误，-2表示数据发送失败
  */
int8_t SI2C_SendBuffer(SI2C_TypeDef *hi2c, uint8_t Addr,const uint8_t *pData, uint16_t Size)
{
    uint16_t i;

    if (pData == NULL || Size == 0)
    {
        return -1;                            // 参数错误
    }

    SI2C_SendStart(hi2c);                     // 发送起始位

    if (SI2C_SendByte(hi2c, (Addr << 1) | 0) == 1)  // 发送从机地址+写方向位
    {
        SI2C_SendStop(hi2c);                  // 从机无应答，释放总线
        return 1;                             // 寻址失败
    }

    for (i = 0; i < Size; i++)
    {
        if (SI2C_SendByte(hi2c, pData[i]) == 1)  // 发送数据，检查从机ACK
        {
            SI2C_SendStop(hi2c);
            return -2;                        // 数据发送失败
        }
    }

    SI2C_SendStop(hi2c);                      // 发送停止位
    return 0;                                 // 正常完成
}

/**
  * @brief  从指定从机接收多个字节
  * @param  hi2c 软件I2C结构体指针
  * @param  Addr 7位从机地址
  * @param  pBuffer 接收数据缓冲区指针
  * @param  Size 期望接收字节数
  * @return 0表示正常，1表示寻址失败，-1表示参数错误
  */
int8_t SI2C_ReceiveBuffer(SI2C_TypeDef *hi2c, uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
    uint16_t i;

    if (pBuffer == NULL || Size == 0)
    {
        return -1;                            // 参数错误
    }

    SI2C_SendStart(hi2c);                     // 发送起始位

    if (SI2C_SendByte(hi2c, (Addr << 1) | 1) == 1)  // 发送从机地址+读方向位
    {
        SI2C_SendStop(hi2c);                  // 从机无应答，释放总线
        return 1;                             // 寻址失败
    }

    for (i = 0; i < Size; i++)
    {
        if (i == Size - 1)
        {
            pBuffer[i] = SI2C_ReceiveByte(hi2c, 0); // 最后一个字节发送NACK
        }
        else
        {
            pBuffer[i] = SI2C_ReceiveByte(hi2c, 1); // 其余字节发送ACK
        }
    }

    SI2C_SendStop(hi2c);                      // 发送停止位
    return 0;                                 // 正常完成
}
/**
  * @brief  读取从机寄存器数据
  * @param  hi2c 软件I2C结构体指针
  * @param  SlaveAddr 从机地址
  * @param  RegAddr   寄存器地址
  * @param  pBuffer 接收数据缓冲区指针
  * @param  Size 期望接收字节数
  * @return 0表示正常，1表示寻址失败，2表示发送寄存器失败，3表示读取数据失败
  */
uint8_t SI2C_ReadReg(SI2C_TypeDef *hi2c,uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *pbuffer,uint16_t Size){
	//发送起始位
	SI2C_SendStart(hi2c);
	//发送从机地址
	if(SI2C_SendByte(hi2c,(SlaveAddr<<1)) != 0){
		SI2C_SendStop(hi2c);
		return 1;
	}
	//发送需要读取的寄存器地址
	if(SI2C_SendByte(hi2c,RegAddr) != 0){
		SI2C_SendStop(hi2c);
		return 2;
	}
	//拉低SCL以发送REPEATED START
	SI2C_SCL_Write(hi2c,0);
	for(uint8_t i=0;i<200;i++);
	//REPEATED START，读取数据
	if(SI2C_ReceiveBuffer(hi2c,SlaveAddr,pbuffer,Size) != 0){
		SI2C_SendStop(hi2c);
		return 3;
	}
	return 0;
}
