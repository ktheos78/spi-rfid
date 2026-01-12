/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MFRC522.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int validate (uint8_t *uid)
{
	uint8_t tag_uid[] = {0xC3, 0xB1, 0xE6, 0x1B, 0x8F};

	for (uint8_t i = 0; i < 5; ++i)
	{
		if (uid[i] != tag_uid[i])
			return 0;
	}

	return 1;
}

/* stupid way to get card UID if a serial debugger or LCD isn't available */
void get_uid_helper(uint8_t *card_uid)
{
	uint8_t temp1, temp2;

	// card uid [0:5] = C3 B1 E6 1B 8F
	for (uint8_t i = 0; i < 5; ++i)
	{
		temp1 = card_uid[i];
		for (uint8_t j = 0; j < 8; ++j)
		{
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
			temp2 = (temp1 & 0x80);
			if (temp2)
				LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
			LL_mDelay(1000);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);
			LL_mDelay(1000);
			temp1 <<= 1;
		}
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */
	uint8_t buf_atqa[2], uid[5];
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */
	MFRC522_init();
	/* USER CODE END 2 */

	// init PORTA as off
	LL_GPIO_ResetOutputPin(GPIOA, 0xFF);

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// check if card is in the field
		if (MFRC522_request(buf_atqa) == STATUS_OK)
		{
			// if present, read UID
			if (MFRC522_anticoll(uid) == STATUS_OK)
			{

				// check if scanned UID matches
				if (validate(uid))
				{
					LL_GPIO_SetOutputPin(GPIOA, 0xFF);
					LL_mDelay(5000);
					LL_GPIO_ResetOutputPin(GPIOA, 0xFF);
				}

				else
				{
					for (uint8_t i = 0; i < 5; ++i)
					{
						LL_GPIO_SetOutputPin(GPIOA, 0xFF);
						LL_mDelay(500);
						LL_GPIO_ResetOutputPin(GPIOA, 0xFF);
						LL_mDelay(500);
					}
				}
			}
		}
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
	while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
	{
	}
	LL_PWR_EnableRange1BoostMode();
	LL_RCC_HSI_Enable();
	/* Wait till HSI is ready */
	while(LL_RCC_HSI_IsReady() != 1)
	{
	}

	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 85, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();
	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1)
	{
	}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
	}

	/* Insure 1us transition state at intermediate medium speed clock*/
	for (__IO uint32_t i = (170 >> 1); i !=0; i--);

	/* Set AHB prescaler*/
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_SetSystemCoreClock(170000000);

	/* Update the time base */
	if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	// do nothing
}
