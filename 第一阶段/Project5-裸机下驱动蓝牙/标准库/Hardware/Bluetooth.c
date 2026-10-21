#include "Bluetooth.h"

void APP_BlueTooth_Init(void){
	APP_USART_Init(USART1);
	APP_USART_NVICInit(USART1);
}
	