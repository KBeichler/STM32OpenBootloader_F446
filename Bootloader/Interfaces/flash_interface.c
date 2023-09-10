/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "flash_interface.h"
//#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
FLASH_TypeDef * flash = (FLASH_TypeDef *)FLASH_R_BASE ;
/* Private variables ---------------------------------------------------------*/
uint32_t Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED, \
                                     .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                     .ProcedureOnGoing = 0U, \
                                     .Address = 0U, \
                                     .Bank = FLASH_BANK_1, \
                                     .Sector = 0U, \
                                     .NbSectorsToErase = 0U
                                    };

/* Private function prototypes -----------------------------------------------*/
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *ListOfPages, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);
static void writeOB(FLASH_OBProgramInitTypeDef *flash_ob);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  FLASH_BL_SIZE,
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  OPENBL_FLASH_SetReadOutProtectionLevel,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  Address The address where that data will be written.
  * @param  Data The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  uint32_t index  = 0U;
  uint32_t length = DataLength;
  uint64_t CurrentWord = 0;

  if (length & 7U)
  {
    length = (length & 0xFFFFFFF8U) + 4U;
  }

  /* Unlock the flash memory for write operation */
  OPENBL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  for (index = 0U; index < length; (index += 8U))
  { 
    CurrentWord = (uint64_t)(*((uint64_t *)((uint32_t)pData + index)));
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (Address + index), CurrentWord & 0xFFFFFFFF);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (Address + index + 4), CurrentWord >> 32);
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  Address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{
  Function_Pointer appStart;

  /* Deinitialize all HW resources used by the Bootloader to their reset values */
  OpenBootloader_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  uint32_t *userProgStart = (uint32_t*)Address;  // pointer to the start of the application at 0x08004000

  appStart = (Function_Pointer) userProgStart[1];   // get the address of the application's reset handler by loading the 2nd entry in the table
  SCB->VTOR = (uint32_t)userProgStart;   // point VTOR to the start of the application's vector table
  Common_SetMsp(userProgStart[0]);   // setup the initial stack pointer using the RAM address contained at the start of the vector table
  appStart();   // call the application's reset handler

}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval The return value can be one of the following values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;
  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);
  return flash_ob.RDPLevel;
}

/**
  * @brief  Set  the FLASH Read Protection level.
  * @param  Level Can be one of these values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  * @retval None.
  */
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level)
{
  FLASH_OBProgramInitTypeDef flash_ob;
  flash_ob.OptionType = OPTIONBYTE_RDP;
  flash_ob.RDPLevel = Level;
  /* set the Option bytes configuration */
  writeOB(&flash_ob);
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *pListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    OPENBL_FLASH_EnableWriteProtection(pListOfPages, Length);
  }
  else if (State == DISABLE)
  {
    OPENBL_FLASH_DisableWriteProtection();
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function is used to start FLASH mass erase operation.
  * @param  *p_Data Pointer to the buffer that contains mass erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done
  *          - ERROR:   Mass erase operation failed or the value of one parameter is not OK
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *p_Data, uint32_t DataLength)
{
  OPENBL_FLASH_Unlock();
  ErrorStatus status = SUCCESS;
  uint32_t error = 0;
  FLASH_EraseInitTypeDef flash_er;
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_er.TypeErase = FLASH_TYPEERASE_MASSERASE;
  flash_er.Banks = FLASH_BANK_1;
  flash_er.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  status = HAL_FLASHEx_Erase(&flash_er, &error); 
  OPENBL_FLASH_Lock();
  return status;

}

/**
  * @brief  This function is used to erase the specified FLASH pages.
  * @param  *p_Data Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not OK
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_Data, uint32_t DataLength)
{
  OPENBL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
  ErrorStatus status = SUCCESS;
  uint32_t error = 0;
  FLASH_EraseInitTypeDef flash_er;
  uint8_t nr_of_sectors = *(p_Data);
  uint8_t start_sector = *(p_Data);


  flash_er.TypeErase = FLASH_TYPEERASE_SECTORS;
  flash_er.Banks = FLASH_BANK_1;
  flash_er.NbSectors = nr_of_sectors;
  flash_er.Sector = start_sector;
  flash_er.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  status = HAL_FLASHEx_Erase(&flash_er, &error); 
  OPENBL_FLASH_Lock();
  return status;
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfPages, uint32_t Length)
{
  //uint8_t wrp_start_offset = 0x7FU;
  //uint8_t wrp_end_offset   = 0x00U;
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState = OB_WRPSTATE_ENABLE;
  flash_ob.Banks    = FLASH_BANK_1;

  /* Write protection of bank 1 area WRPA 1 area */
  //if (Length >= 2U)
  //{
  //wrp_start_offset = *(pListOfPages);
  //wrp_end_offset   = *(pListOfPages + 1U);
  // TODO check if this works
  //#define OB_WRP_SECTOR_0       0x00000001U /*!< Write protection of Sector0     */
  //#define OB_WRP_SECTOR_1       0x00000002U /*!< Write protection of Sector1     */
  //#define OB_WRP_SECTOR_2       0x00000004U /*!< Write protection of Sector2     */
  //#define OB_WRP_SECTOR_3       0x00000008U /*!< Write protection of Sector3     */
  //#define OB_WRP_SECTOR_4       0x00000010U /*!< Write protection of Sector4     */
  //#define OB_WRP_SECTOR_5       0x00000020U /*!< Write protection of Sector5     */
  //#define OB_WRP_SECTOR_6       0x00000040U /*!< Write protection of Sector6     */
  //#define OB_WRP_SECTOR_7       0x00000080U /*!< Write protection of Sector7     */
  //#define OB_WRP_SECTOR_All     0x00000FFFU /*!< Write protection of all Sectors */

  for (uint8_t ii = 0; ii < Length; ii++)
  {
    flash_ob.WRPSector |= *(pListOfPages);
    pListOfPages++;
  }

  status = HAL_FLASHEx_OBProgram(&flash_ob);
  //}

  /* Write protection of bank 1 area WRPA 2 area 
  if (Length >= 4U)
  {
    wrp_start_offset = *(pListOfPages + 2U);
    wrp_end_offset   = *(pListOfPages + 3U);

    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
    flash_ob.WRPStartOffset = wrp_start_offset;
    flash_ob.WRPEndOffset   = wrp_end_offset;

    HAL_FLASHEx_OBProgram(&flash_ob);
  }
  */
  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState = OB_WRPSTATE_ENABLE;
  flash_ob.Banks    = FLASH_BANK_1;
  flash_ob.WRPSector = OB_WRP_SECTOR_All;
  writeOB(&flash_ob);
  return status;
}


static void writeOB(FLASH_OBProgramInitTypeDef *flash_ob)
{
  OPENBL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();
  /* set the Option bytes configuration */
  HAL_FLASHEx_OBProgram(flash_ob);
  OPENBL_FLASH_Lock();
  HAL_FLASH_OB_Lock(); 
}
