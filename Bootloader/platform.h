/**
  ******************************************************************************
  * @file    platform.h
  * @author  MCD Application Team
  * @brief   Header for patterns
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_gcc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_iwdg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define USERPROG_START_ADDRESS  0x8004000 
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H */
