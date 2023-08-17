/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32h7xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
#include "process.h"
#include <stdio.h>
#include "user_syscalls.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1)
	{
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); //turn on the red led for hardfault
	//	HAL_Delay(1000); //delay for 1 sec	
		microsleep(1000000); //sleep for 1 sec	
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
__attribute__((naked))void PendSV_Handler(void)
{
	/* USER CODE BEGIN PendSV_IRQn 0 */
	//push context registers R4-R11 from the current process

	Context_Register_Save(); 
	register struct task_struct *ret_val_sch;
	ret_val_sch = schedule_r();

	//second varibale tied to the sp register
	register uint32_t *sp __asm("sp"); //or R13

	//save R4 through R11 to the current process struct
	current->r_struct.R4 = (*(sp));
	current->r_struct.R5 = (*(sp+1));
	current->r_struct.R6 =  (*(sp+2));
	current->r_struct.R7 =  (*(sp+3));
	current->r_struct.R8 =  (*(sp+4));
	current->r_struct.R9 =  (*(sp+5));
	current->r_struct.R10 =  (*(sp+6));
	current->r_struct.R11 = (*(sp+7));

	if (ret_val_sch == &task_idle)
	{
		current->r_struct.R13 = __get_PSP(); //if in idle save it tostackpointer R13 in that case
	}


	//determine next process to run
	else if (current == &task_idle)
	{
		__set_PSP(ret_val_sch->r_struct.R13);
	}
	else
	{
		current->r_struct.R13 = __get_PSP(); //saving PSP to current
		__set_PSP(ret_val_sch->r_struct.R13);	
	}


	//save the current process state to its struct and update current pointer
	//current->state = STOP;//shouldn't this be just stop

	current = ret_val_sch;
	sp +=8;

	//call function that restores registers
	Context_Restore_Registers(ret_val_sch); //idk if you do that before or after memory pop
	//return from the volatile exception an load PC with exc_return
	Context_Switch_Return(ret_val_sch);


	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */

}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	// HAL_IncTick();
	//increment the value uwtick
	uwTick += 1;
	//if 32ms has elapsed and kready = 1 toggle an LED
	//HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin); //the other LED

	if (((uwTick % 32) == 0) &&( kready == 1))
	{
		//toggle LED outside
		//	HAL_Delay(100);		
		HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin); //the other LED
		yield();


	}
	//call yield function after toggling LED
	//  yield();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 stream0 global interrupt.
 */
void DMA1_Stream0_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

	/* USER CODE END DMA1_Stream0_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_spi1_tx);
	/* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

	/* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
 * @brief This function handles SPI1 global interrupt.
 */
void SPI1_IRQHandler(void)
{
	/* USER CODE BEGIN SPI1_IRQn 0 */

	/* USER CODE END SPI1_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi1);
	/* USER CODE BEGIN SPI1_IRQn 1 */

	/* USER CODE END SPI1_IRQn 1 */
}

/**
 * @brief This function handles USB On The Go HS global interrupt.
 */
void USART3_IRQHandler(void)
{
	 HAL_UART_IRQHandler(&huart3);
}
void OTG_HS_IRQHandler(void)
{
	/* USER CODE BEGIN OTG_HS_IRQn 0 */

	/* USER CODE END OTG_HS_IRQn 0 */
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
	/* USER CODE BEGIN OTG_HS_IRQn 1 */

	/* USER CODE END OTG_HS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
