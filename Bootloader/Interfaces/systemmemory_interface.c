/**
  ******************************************************************************
  * @file    systemmemory_interface.c
  * @author  MCD Application Team
  * @brief   Contains System Memory access functions
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
#include "systemmemory_interface.h"
#include "openbootloader_conf.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef ICP1_Descriptor =
{
  ICP_START_ADDRESS,
  ICP_END_ADDRESS,
  ICP_SIZE,
  ICP_AREA,
  OPENBL_ICP_Read,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

OPENBL_MemoryTypeDef ICP2_Descriptor =
{
  ICP2_START_ADDRESS,
  ICP2_END_ADDRESS,
  ICP2_SIZE,
  ICP_AREA,
  OPENBL_ICP_Read,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

OPENBL_MemoryTypeDef ICP3_Descriptor =
{
  EB_START_ADDRESS,
  EB_END_ADDRESS,
  EB_SIZE,
  ICP_AREA,
  OPENBL_ICP_Read,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_ICP_Read(uint32_t Address)
{
  (void) Address;
  return 0;
}
