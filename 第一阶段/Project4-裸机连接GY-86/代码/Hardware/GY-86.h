#ifndef GY_86_H
#define GY_86_H

#include "stm32f4xx.h"
#include "SI2C.h"
#include "main.h"

//定义输出读取值的格式
typedef struct{
	float ax_out;
	float ay_out;
	float az_out;
	float gx_out;
	float gy_out;
	float gz_out;
	float tem_out;
}MPUdata;

typedef struct{
	float Bx_out;
	float By_out;
	float Bz_out;
}HMCdata;

typedef struct{
	//校准系数
	uint16_t c1,c2,c3,c4,c5,c6;
	//原始数据
	uint32_t d1;//原始读取气压
	uint32_t d2;//原始读取温度
	//校验后数据
	int32_t t;//温度，单位0.01°C
	int32_t p;//气压，单位0.01mbar
}MSdata;

void APP_GY86_Init(void);
uint8_t APP_MPU6050_Update(void);
MPUdata APP_MPU6050_Print(void);
#endif
