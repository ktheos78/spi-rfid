/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"

void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t spi_transceive(uint8_t);
/* USER CODE END Prototypes */

#endif /* __SPI_H__ */

