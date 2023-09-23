/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
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
#include "openbl_mem.h"
#include "Bootloader.h"
#include "common_interface.h"
#include "optionbytes_interface.h"
#include "flash_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB_Descriptor =
{
  OB_START_ADDRESS,
  OB_END_ADDRESS,
  OB_SIZE,
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_OB_Launch(void)
{
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OB_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  Write Flash OB keys to unlock the option bytes settings
  * @param  None
  * @retval None
  */
void BL_FLASH_WriteOptKeys(void)
{
}
/**
  * @brief  This function is used to write data in Option bytes.
  * @param  Address The address where that data will be written.
  * @param  Data The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_OB_Write(uint32_t Address, uint8_t *pData, uint32_t length)
{
  /* The actual data send by the CubeProgrammer is somethink like this
    0x ef aa 10 55 ef aa 10 55 7f 3f 00 c0 ff 3f 00 c0
    Thats new values  byte 0 and byte 1:  ef  aa
    Then the XOR of these values:         10  55 
    Then the old values for byte 0 and 1: ef  aa
    And the XOr again                     10  55
    Then new values for byte 3 and 4:     7f  3f
    Their XOR values (here not :/ )       00  c0
    Then the old values of these bytes:   ff  3f
    And their XOR vales                   00  c0               

  */
  __IO uint8_t *pOBBase;
  uint16_t dataLength = length / 8; // length is in bytes, STM cube programmer always sends 6t4 bit doublewords
  /* Unlock the FLASH & Option Bytes Registers access */
  HAL_FLASH_OB_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

#ifdef __STM32F446xx_H
    pOBBase = ( __IO uint8_t *) OPTCR_BYTE0_ADDRESS;
    /* STM CubeProgrammer sets OPSTRT and OPTLOCK Flag, and thus we cannot change any other values afterwords.
    *  This flags will be set by HAL_FLASH_OB_Launch later, thats why we ignore them here */
    pData[0] &= ~(FLASH_OPTCR_OPTSTRT | FLASH_OPTCR_OPTLOCK);   

   for (uint16_t i = 0; i < dataLength; i++ )
   {
      pOBBase[(i * 2)]      = pData[(i * 8)];
      pOBBase[(i * 2) + 1]  = pData[(i * 8) + 1];
   }
#endif

  HAL_FLASH_OB_Launch();
  HAL_FLASH_OB_Lock();
}
