/**
  ******************************************************************************
  * @file    interfaces_conf.h
  * @author  MCD Application Team
  * @brief   Contains Interfaces configuration
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
#ifndef INTERFACES_CONF_H
#define INTERFACES_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_usart.h"

#define MEMORIES_SUPPORTED                7U

/* ------------------------- Definitions for USART -------------------------- */
#define USARTx                            USART2
#define USARTx_CLK_ENABLE()               __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_CLK_DISABLE()              __HAL_RCC_USART2_CLK_DISABLE()
#define USARTx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_DeInit()                   LL_USART_DeInit(USARTx)

#define USARTx_TX_PIN                     GPIO_PIN_2
#define USARTx_TX_GPIO_PORT               GPIOA
#define USARTx_RX_PIN                     GPIO_PIN_3



/* ------------------------- Definitions for FDCAN -------------------------- */
/*
#define FDCANx                            FDCAN1
#define FDCANx_CLK_ENABLE()               __HAL_RCC_FDCAN1_CLK_ENABLE()
#define FDCANx_CLK_DISABLE()              __HAL_RCC_FDCAN1_CLK_DISABLE()
#define FDCANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()

#define FDCANx_TX_PIN                     GPIO_PIN_8
#define FDCANx_TX_GPIO_PORT               GPIOB
#define FDCANx_TX_AF                      GPIO_AF9_FDCAN1
#define FDCANx_RX_PIN                     GPIO_PIN_9
#define FDCANx_RX_GPIO_PORT               GPIOB
#define FDCANx_RX_AF                      GPIO_AF9_FDCAN1

#define FDCANx_IT0_IRQn                   TIM16_FDCAN_IT0_IRQn
#define FDCANx_IT1_IRQn                   TIM17_FDCAN_IT1_IRQn
#define FDCANx_IRQHandler                 TIM16_FDCAN_IT0_IRQHandler

#define FDCANx_FORCE_RESET()              __HAL_RCC_FDCAN1_FORCE_RESET()
#define FDCANx_RELEASE_RESET()            __HAL_RCC_FDCAN1_RELEASE_RESET()
*/


#ifdef __cplusplus
}
#endif

#endif /* INTERFACES_CONF_H */
