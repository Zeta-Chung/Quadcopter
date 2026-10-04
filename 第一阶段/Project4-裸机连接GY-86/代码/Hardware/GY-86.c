#include "GY-86.h"
static void MPU6050_reg_write(uint8_t reg,uint8_t value);
static uint8_t MPU6050_reg_read(uint8_t reg);
static float ax,ay,az,gx,gy,gz,tamperature;

void APP_MPU6050_Init(void)
{
	APP_I2C_Init(I2C1);
	MPU6050_reg_write(0x6B,0x80);//复位MPU6050设备
	Delay(100);			         //等待复位
	MPU6050_reg_write(0x6B,0x00);//退出睡眠模式，设置为非循环，使能温度传感器
	MPU6050_reg_write(0x1B,(0x01<<3));//设置陀螺仪量程为+-500°/s
	MPU6050_reg_write(0x1C,(0x01<<3));//设置加速度计量程为+-4g
}

/***/
//@brief 
//@param reg - 需要写入的寄存器
//@param value - 需要写入的值
/***/
static void MPU6050_reg_write(uint8_t reg,uint8_t value)
{	
	uint8_t bytesToSend[] = {reg,value};
	APP_I2C_SendData(I2C1,0x68,bytesToSend,2);
}
/***/
//@brief 从MPU6050中读取单字节数据
//@param reg - 需要读取的寄存器
//@retval 读取到的值
static uint8_t MPU6050_reg_read(uint8_t reg)
{
	APP_I2C_SendData(I2C1,0x68,&reg,1);
	uint8_t RegValue;
	APP_I2C_ReceiveData(I2C1,0x68,&RegValue,1);
	return RegValue;
}
