#include "progs.h"
#include "main.h"
#include "user_syscalls.h"
#include "process.h"
#include "usart.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"
#include "Adafruit_ST7735.h"
#include "gfxfont.h"
#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
uint8_t fg = 0;
int process1(void) 
{
while (1)
{
	HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
//	check for lock 8
	if (HAL_HSEM_Take(8, current->pid) == HAL_OK)//get the lock
	{
		printf("Current: %d | The current value of i is: %u\r\n",current->pid,fg++);

	} else {

		yield();

	}
	HAL_HSEM_Release(8, current->pid); //release the lock
	microsleep(1000);//sleep for 1 ms

}
	return 1;
}
int process2(void)
{
	while (1) 
	{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	HAL_Delay(1000); //1Hz is 1 sec or 1000ms
	microsleep(1000000);//one million microseconds = 1sec
	}
	return 1;
}



