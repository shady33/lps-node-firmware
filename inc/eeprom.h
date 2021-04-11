#ifndef __EEPROM_H__
#define __EEPROM_H__


#include <stdbool.h>

#ifdef STM32F0
    #include <stm32f0xx_hal.h>
#else
    #include <stm32f1xx_hal.h>
#endif


void eepromInit(I2C_HandleTypeDef * i2c);

bool eepromRead(int address, void* data, size_t length);

bool eepromWrite(int address, void* data, size_t length);

bool eepromTest();

#endif
