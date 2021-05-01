/*
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * LPS node firmware.
 *
 * Copyright 2016, Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef STM32F0
  #include <stm32f0xx_hal.h>
#else
  #include <stm32f1xx_hal.h>
  uint8_t dummyMemory[100];
  #define FLASH_PAGE_START_ADDRESS 0x0800FC00UL // FC00
#endif

#include <stdio.h>

#include <stdbool.h>
#include <string.h>

#include "eeprom.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef STM32F0
static I2C_HandleTypeDef * hi2c;
static int devAddr = 0xA0;

void eepromInit(I2C_HandleTypeDef * i2c)
{
    hi2c = i2c;
}
#else
void eepromInit()
{

}
#endif
bool eepromTest()
{
  uint8_t test;

  return eepromRead(0, &test, 1);
}

bool eepromRead(int address, void* data, size_t length)
{
  #ifdef STM32F0
    int status;
  
    status = HAL_I2C_Mem_Read(hi2c, devAddr, address, I2C_MEMADD_SIZE_16BIT, data, length, 100);

    if (status == HAL_OK)
      return true;
    
    return false;
  #else
    for (int i=0; i<length; i++) {
      *(&data+i) = dummyMemory[address+i];
    }

    return true;  
  #endif
}

bool eepromFlashRead(int address, void *data, size_t length)
{
    uint16_t *start_address;
    start_address =  (uint16_t *)(FLASH_PAGE_START_ADDRESS + address);
    for (int i=0; i<length; i++) {
      *( (uint8_t *)data + i ) = (uint8_t)*(__IO uint16_t*)start_address;
      start_address++;
    }
    return true;  
}

bool eepromCommit(int address, void* data, size_t length)
{
  HAL_FLASH_Unlock();

  int status = HAL_OK;
  uint32_t start_address = FLASH_PAGE_START_ADDRESS + address;
  size_t start_i = 0;
  
  // for(int i=0;i<length;i++){
  //   uint16_t x = *(uint8_t *)(data + i);
  //   printf("%x ",x);
  // }
    
  // printf("\r\n");
  for (int i=0; i<length; i++) {
    uint16_t x = *(uint8_t *)(data + i);
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, start_address, x);
    start_address = start_address + 2;
    start_i = i + 1;
    vTaskDelay(10);
  }

  if (start_i != length) {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, start_address, ((uint8_t*)data)+start_i);
    vTaskDelay(10);
  }

  HAL_FLASH_Lock();

  if (status == HAL_OK)
    return true;

  return false;
}

bool eepromWrite(int address, void* data, size_t length)
{
  #ifdef STM32F0
    int status = HAL_OK;
    size_t start_address = address;
    size_t start_i = 0;

    for (int i=0; i<length; i++) {
      if ((address+i+1)%32 == 0) {
        status = HAL_I2C_Mem_Write(hi2c, devAddr, start_address, I2C_MEMADD_SIZE_16BIT, ((uint8_t*)data)+start_i, (i+1) - start_i, 100);
        start_address = address + i + 1;
        start_i = i+1;
        vTaskDelay(10);
      }
    }
    if (start_i != length) {
      status = HAL_I2C_Mem_Write(hi2c, devAddr, start_address, I2C_MEMADD_SIZE_16BIT, ((uint8_t*)data)+start_i, length - start_i, 100);
      vTaskDelay(10);
    }

    if (status == HAL_OK)
      return true;

    return false;
  #else

    if(address + length > 99){
      printf("EEPROM Write beyond scope\n");
    }
    int status = HAL_OK;
    size_t start_address = address;
    size_t start_i = 0;

    for (int i=0; i<length; i++) {
      dummyMemory[address+i] =  ((uint8_t*)data)+i;
    }
    
    return true;
  #endif
}

bool eepromErase()
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if (HAL_FLASH_Erase(FLASH_PAGE_START_ADDRESS) != HAL_OK)
  {
    /*Error occurred while page erase.*/
    // return HAL_FLASH_GetError();
    return false;
  }
  HAL_FLASH_Lock();
  return true;
}