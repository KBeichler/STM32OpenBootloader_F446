/**
  ******************************************************************************
  * @file    usart_interface.c
  * @author  MCD Application Team
  * @brief   Contains USART HW configuration
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "interfaces_conf.h"
#include "openbl_core.h"
#include "openbl_usart_cmd.h"
#include "usart_interface.h"
#include "iwdg_interface.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t UsartDetected = 0U;
static UART_HandleTypeDef huart2;
/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OPENBL_USART_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  This function is used to initialize the used USART instance.
 * @retval None.
 */
static void OPENBL_USART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  This function is used to configure USART pins and then initialize the used USART instance.
 * @retval None.
 */
void OPENBL_USART_Configuration(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*
  GPIO_InitStruct.Pin = USARTx_TX_PIN;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct); 
*/
  OPENBL_USART_Init();
}

/**
 * @brief  This function is used to De-initialize the USART pins and instance.
 * @retval None.
 */
void OPENBL_USART_DeInit(void)
{
}

/**
 * @brief  This function is used to detect if there is any activity on USART protocol.
 * @retval Returns 1 if interface is detected else 0.
 */
uint8_t OPENBL_USART_ProtocolDetection(void)
{
  if (LL_USART_IsActiveFlag_RXNE(USARTx))
  {
    OPENBL_USART_ReadByte();   
    
    /* Aknowledge the host */
    OPENBL_USART_SendByte(ACK_BYTE);

    UsartDetected = 1;
  }
  else
  {
    UsartDetected = 0;
  }
  return UsartDetected;
}

/**
 * @brief  This function is used to get the command opcode from the host.
 * @retval Returns the command.
 */
uint8_t OPENBL_USART_GetCommandOpcode(void)
{
  uint8_t command_opc = 0x0;
  /* Get the command opcode */
  command_opc = OPENBL_USART_ReadByte();

  /* Check the data integrity */
  if ((command_opc ^ OPENBL_USART_ReadByte()) != 0xFFU)
  {
    command_opc = ERROR_COMMAND;
  }

  return command_opc;

  return command_opc;
}

/**
  * @brief  This function is used to read one byte from USART pipe.
  * @retval Returns the read byte.
  */
uint8_t OPENBL_USART_ReadByte(void)
{
  while (!LL_USART_IsActiveFlag_RXNE(USARTx))
  {
    OPENBL_IWDG_Refresh();
  }

  return LL_USART_ReceiveData8(USARTx);
}

/**
  * @brief  This function is used to send one byte through USART pipe.
  * @param  Byte The byte to be sent.
  * @retval None.
  */
void OPENBL_USART_SendByte(uint8_t Byte)
{
  LL_USART_TransmitData8(USARTx, (Byte & 0xFFU));

  while (!LL_USART_IsActiveFlag_TC(USARTx))
  {
  }
}

/**
 * @brief  This function is used to process and execute the special commands.
 *         The user must define the special commands routine here.
 * @param  SpecialCmd Pointer to the OPENBL_SpecialCmdTypeDef structure.
 * @retval Returns NACK status in case of error else returns ACK status.
 */
void OPENBL_USART_SpecialCommandProcess(OPENBL_SpecialCmdTypeDef *SpecialCmd)
{
}
