/**
  ******************************************************************************
  * @file    app_openbootloader.c
  * @author  MCD Application Team
  * @brief   OpenBootloader application entry point
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
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
#include "Bootloader.h"
#include "openbl_core.h"
#include "usart_interface.h"
#include "openbl_mem.h"
#include "openbl_usart_cmd.h"
#include "iwdg_interface.h"
#include "ram_interface.h"
#include "flash_interface.h"
#include "systemmemory_interface.h"
#include "optionbytes_interface.h"
/*
#include "i2c_interface.h"
#include "otp_interface.h"
#include "openbl_i2c_cmd.h"
*/



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OPENBL_HandleTypeDef USART_Handle;
static OPENBL_HandleTypeDef IWDG_Handle;



static OPENBL_OpsTypeDef USART_Ops =
{
  OPENBL_USART_Configuration,
  NULL,
  OPENBL_USART_ProtocolDetection,
  OPENBL_USART_GetCommandOpcode,
  OPENBL_USART_SendByte
};

static OPENBL_OpsTypeDef IWDG_Ops =
{
  OPENBL_IWDG_Configuration,
  NULL,
  NULL,
  NULL,
  NULL
};

/* External variables --------------------------------------------------------*/
extern OPENBL_MemoryTypeDef FLASH_Descriptor;
extern OPENBL_MemoryTypeDef RAM_Descriptor;
extern OPENBL_MemoryTypeDef OB_Descriptor;
extern OPENBL_MemoryTypeDef OTP_Descriptor;
extern OPENBL_MemoryTypeDef ICP1_Descriptor;
extern OPENBL_MemoryTypeDef ICP2_Descriptor;
extern OPENBL_MemoryTypeDef ICP3_Descriptor;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize open Bootloader.
  * @param  None.
  * @retval None.
  */
void OpenBootloader_Init(void)
{
  /* Register USART interfaces */
  USART_Handle.p_Ops = &USART_Ops;
  USART_Handle.p_Cmd = OPENBL_USART_GetCommandsList();

  OPENBL_RegisterInterface(&USART_Handle);

  /* Register IWDG interfaces */
  IWDG_Handle.p_Ops = &IWDG_Ops;
  IWDG_Handle.p_Cmd = NULL;

  OPENBL_RegisterInterface(&IWDG_Handle);

  /* Initialise interfaces */
  OPENBL_Init();

  /* Initialise memories */
  OPENBL_MEM_RegisterMemory(&FLASH_Descriptor);
  OPENBL_MEM_RegisterMemory(&RAM_Descriptor);
  OPENBL_MEM_RegisterMemory(&OB_Descriptor);
  OPENBL_MEM_RegisterMemory(&OTP_Descriptor);
  OPENBL_MEM_RegisterMemory(&ICP1_Descriptor);
  OPENBL_MEM_RegisterMemory(&ICP2_Descriptor);
  OPENBL_MEM_RegisterMemory(&ICP3_Descriptor);

}

/**
  * @brief  DeInitialize open Bootloader.
  * @param  None.
  * @retval None.
  */
void OpenBootloader_DeInit(void)
{
  __HAL_RCC_APB1_FORCE_RESET();
  __HAL_RCC_APB1_RELEASE_RESET();

  __HAL_RCC_APB2_FORCE_RESET();
  __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_AHB1_FORCE_RESET();
  __HAL_RCC_AHB1_RELEASE_RESET();

  __HAL_RCC_AHB2_FORCE_RESET();
  __HAL_RCC_AHB2_RELEASE_RESET();

  __HAL_RCC_AHB3_FORCE_RESET();
  __HAL_RCC_AHB3_RELEASE_RESET();

  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  OPENBL_DeInit();
}

/**
  * @brief  This function is used to select which protocol will be used when communicating with the host.
  * @param  None.
  * @retval None.
  */
void OpenBootloader_ProtocolDetection(void)
{
  static uint32_t interface_detected = 0U;

  if (interface_detected == 0U)
  {
    interface_detected = OPENBL_InterfaceDetection();
  }

  if (interface_detected == 1U)
  {
    OPENBL_CommandProcess();
  }
}

void OpenBootloader_CheckforUserProgram(void)
{
  Function_Pointer appStart;
  uint32_t *userProgStart = (uint32_t*)USERPROG_START_ADDRESS;  // point _vectable to the start of the application at 0x08004000

  if (userProgStart[0] != 0)  // if there is data in sector 1 we assume a program is present
  {
    appStart = (Function_Pointer) userProgStart[1];   // get the address of the application's reset handler by loading the 2nd entry in the table
    SCB->VTOR = (uint32_t)userProgStart;   // point VTOR to the start of the application's vector table
    Common_SetMsp(userProgStart[0]);   // setup the initial stack pointer using the RAM address contained at the start of the vector table
    appStart();   // call the application's reset handler
  }

}


