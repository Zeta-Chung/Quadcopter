#include "GY-86.h"

#define MS5611_ADDR 0x77  //当前模块的 MS5611的七位地址是0x77

static void MPU6050_Reg_Write(uint8_t reg,uint8_t value);
static uint8_t MPU6050_Reg_Read(uint8_t reg);
static void HMC5883L_Reg_Write(uint8_t reg,uint8_t value);
static uint8_t HMC5883L_Reg_Read(uint8_t reg);

static float ax,ay,az,gx,gy,gz,temperature;
static float Bx,By,Bz;
static MSdata msdata;

SI2C_TypeDef hi2c_gy86 = {GPIOB,GPIO_Pin_8,GPIOB,GPIO_Pin_9};

/***/
//@brief 初始化GY-86的三个设备
/***/
void APP_GY86_Init(void)
{	
	//初始化I2C1
	SI2C_Init(&hi2c_gy86);
	//初始化MPU6050
	MPU6050_Reg_Write(0x6B,0x80);//复位MPU6050设备
	Delay(100);			         //等待复位
	MPU6050_Reg_Write(0x6B,0x00);//退出睡眠模式，设置为非循环，使能温度传感器
	MPU6050_Reg_Write(0x1B,0x18);//设置陀螺仪量程为+-2000°/s
	MPU6050_Reg_Write(0x1C,0x00);//设置加速度计量程为+-2g
	MPU6050_Reg_Write(0x6A,0x00);//关闭MPU6050的主机模式，让旁路I2C接入主I2C总线
	MPU6050_Reg_Write(0x37,0x02);//开启MPU6050的I2C旁路模式，使芯片能够读取磁力计
	//初始化磁力计
	HMC5883L_Reg_Write(0x00,0x70);//8次采样平均，数据输出速度15MHz
	HMC5883L_Reg_Write(0x01,0x20);//设置增益值为1090Ga
	HMC5883L_Reg_Write(0x02,0x00);//设置测量模式为连续测量
	//初始化气压计
	const uint8_t MS5611_reset = 0x1E;//复位值
	uint8_t readbuffer[4];            //读取校验值的缓冲区
	const uint8_t C1READ = 0xA2;
	const uint8_t C2READ = C1READ + 2;
	const uint8_t C3READ = C2READ + 2;
	const uint8_t C4READ = C3READ + 2;
	const uint8_t C5READ = C4READ + 2;
	const uint8_t C6READ = C5READ + 2;
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&MS5611_reset,1);
	Delay(5);//等待复位完成
	//读取C1
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C1READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c1 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
	//读取C2
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C2READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c2 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
	//读取C3
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C3READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c3 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
	//读取C4
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C4READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c4 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
	//读取C5
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C5READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c5 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
	//读取C6
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&C6READ,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,2);
	msdata.c6 = (uint16_t)((readbuffer[0]<<8)|readbuffer[1]);
}

//
//@brief 更新mpu6050的值
//@retval 0 - 读取正常
//@retval 1 - 读取出错
//
uint8_t APP_MPU6050_Update(void)
{
	uint8_t data_raw[14];
	if(SI2C_ReadReg(&hi2c_gy86,0x68,0x3b,data_raw,14) != 0)
		return 1;
	int16_t ax_raw = (int16_t)((data_raw[0]<<8) + data_raw[1]);//ax的原始数据
	int16_t ay_raw = (int16_t)((data_raw[2]<<8) + data_raw[3]);//ay的原始数据
	int16_t az_raw = (int16_t)((data_raw[4]<<8) + data_raw[5]);//az的原始数据
	//换算成实际数据（16位无符号到+-2g）
	ax = ax_raw * 6.1035e-5f;
	ay = ay_raw * 6.1035e-5f;
	az = az_raw * 6.1035e-5f;
	
	int16_t temperature_raw = (int16_t)((data_raw[6]<<8) + data_raw[7]);//温度的原始数据
	temperature = temperature_raw/340.0f + 36.53f;//换算后的温度数据

	int16_t gx_raw = (int16_t)((data_raw[8]<<8) + data_raw[9]);//gx的原始数据
	int16_t gy_raw = (int16_t)((data_raw[10]<<8) + data_raw[11]);//gy的原始数据
	int16_t gz_raw = (int16_t)((data_raw[12]<<8) + data_raw[13]);//gz的原始数据
	//换算成实际数据（16位无符号到+-2000°/s)
	gx = gx_raw * 6.1035e-2f;
	gy = gy_raw * 6.1035e-2f;
	gz = gz_raw * 6.1035e-2f;
	
	return 0;
}

/***/
//@brief 更新HMC5883L的值 
/***/
uint8_t APP_HMC5883L_Update(void)
{
	uint8_t data[6];
	//读取原始数据
	SI2C_ReadReg(&hi2c_gy86,0x1E,0x03,data,6);
	int16_t Bx_raw = (int16_t)((data[0]<<8)+data[1]);
	int16_t By_raw = (int16_t)((data[2]<<8)+data[2]);
	int16_t Bz_raw = (int16_t)((data[4]<<8)+data[5]);
	if(Bx_raw == -4096 || By_raw == -4096 || Bz_raw == -4096)
		return 1;
	//换算成标准数据值
	Bx = Bx_raw * 6.35e-5f; 
	By = By_raw * 6.35e-5f;
	Bz = Bz_raw * 6.35e-5f;
	
	return 0;
}

/***/
//@brief 更新MS5611的数据
/***/
void APP_MS5611_Update(void){
	uint8_t readbuffer[4];
	const uint8_t pREAD = 0x48;//更新气压命令
	const uint8_t tREAD = 0x58;//更新温度命令
	const uint8_t ADREAD = 0x00;//ADC读取命令
	//读取原始气压
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&pREAD,1);
	Delay(11);
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&ADREAD,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,3);
	msdata.d1 = (uint32_t)((readbuffer[0]<<16)|(readbuffer[1]<<8)|readbuffer[2]);
	//读取原始温度
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&tREAD,1);
	Delay(11);
	SI2C_SendBuffer(&hi2c_gy86,MS5611_ADDR,&ADREAD,1);
	SI2C_ReceiveBuffer(&hi2c_gy86,MS5611_ADDR,readbuffer,3);
	msdata.d2 = (uint32_t)((readbuffer[0]<<16)|(readbuffer[1]<<8)|readbuffer[2]);
	
	//一阶补偿公式
	int32_t dT = (int32_t)(msdata.d2 - (int64_t)msdata.c5*256);//温度差
	int32_t TEMP = (int32_t)(2000 + dT * (int64_t)msdata.c6 / 8388608);//温度,单位0.01°C
	int64_t OFF = (int64_t)msdata.c2 * 65536 + (int64_t)msdata.c4*dT/128; //偏移量
	int64_t SENS = (int64_t)msdata.c1 * 32768 + (int64_t)msdata.c3 * dT / 256;//灵敏度
	//温度小于20时的二阶补偿
	if(TEMP < 2000){
		int32_t T2 = (int32_t)((int64_t)dT*(int64_t)dT / 2147483648);
		int64_t OFF2 = 5*((int64_t)TEMP - 2000)*((int64_t)TEMP - 2000)/2;
		int64_t SENS2 = 5*((int64_t)TEMP - 2000)*((int64_t)TEMP - 2000)/4;
		if(TEMP < -1500){
			OFF2 = OFF2 + 7*((int64_t)TEMP+1500)*((int64_t)TEMP+1500);
			SENS2 = SENS2 + 11*((int64_t)TEMP+1500)*((int64_t)TEMP+1500)/2;
		}
		TEMP = TEMP - T2;
		OFF = OFF - OFF2;
		SENS = SENS - SENS2;
	}
	int32_t P = (int32_t)(((int64_t)msdata.d1 * SENS / 2097152 - OFF) / 32768); //气压,单位0.01mbar
	msdata.t = TEMP;
	msdata.p = P;
}

/***/
//@brief 输出MPU6050读取到的值
//@retval 含3轴加速度和角速度，以及温度的结构体
/***/
MPUdata APP_MPU6050_Print(void)
{	
	MPUdata data;
	APP_MPU6050_Update();
	data.ax_out = ax;
	data.ay_out = ay;
	data.az_out = az;
	data.gx_out = gx;
	data.gy_out = gy;
	data.gz_out = gz;
	data.tem_out = temperature;
	return data;
}

/***/
//@brief 打印HMC5883L的测量值
//@retval 含3轴磁感应强度的结构体，单位为高斯
/***/
HMCdata APP_HMC5883L_Print(void)
{
	HMCdata data;
	APP_HMC5883L_Update();
	data.Bx_out = Bx;
	data.By_out = By;
	data.Bz_out = Bz;
	return data;
}
/***/
//@brief 更新并获取MS5611的气压测量值
//@retval 气压值，单位为mbar
/***/
float APP_MS5611_Print(void)
{
	APP_MS5611_Update();
	return (float)msdata.p / 100.0f;
}


/***/
//@brief 向MPU6050中写入单字节数据
//@param reg - 需要写入的寄存器
//@param value - 需要写入的值
/***/
static void MPU6050_Reg_Write(uint8_t reg,uint8_t value)
{	
	uint8_t bytesToSend[] = {reg,value};
	SI2C_SendBuffer(&hi2c_gy86,0x68,bytesToSend,2);
}
static void HMC5883L_Reg_Write(uint8_t reg,uint8_t value)
{	
	uint8_t bytesToSend[] = {reg,value};
	SI2C_SendBuffer(&hi2c_gy86,0x1E,bytesToSend,2);
}
/***/
//@brief 从磁力计中读取单字节数据
//@param reg - 需要读取的寄存器
//@retval 读取到的值
static uint8_t HMC5883L_Reg_Read(uint8_t reg)
{
	uint8_t regvalue;
	if(SI2C_ReadReg(&hi2c_gy86,0x1E,reg,&regvalue,1)!=0)
		regvalue = 0;
	return regvalue;
}
