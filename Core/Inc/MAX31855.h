
/*************************************************************************************
 Title	 :  MAXIM Integrated MAX31855 Library for STM32 Using HAL Libraries
 Author  :  Bardia Alikhan Afshar <bardia.a.afshar@gmail.com>
 Software:  STM32CubeIDE
 Hardware:  Any STM32 device
*************************************************************************************/
#ifndef MAX31855_H_
#define MAX31855_H_

#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi2;

typedef struct {
    float temperature;
    float internalTemperature;
    uint8_t fault;
} MAX31855_Data;


// ------------------------- Defines -------------------------
extern uint8_t TC_Error;	   // Error Detection - 1-> No Connection / 2-> Short to GND / 4-> Short to VCC
#define SSPORT GPIOA       // GPIO Port of Chip Select(Slave Select)
#define SSPIN  GPIO_PIN_8  // GPIO PIN of Chip Select(Slave Select)
#define TC_HSPI_INSTANCE			&hspi2

#define MAX31855_CS_PIN GPIO_PIN_8   // Adjust to your CS pin
#define MAX31855_CS_PORT GPIOA       // Adjust to your CS port


#define MAX31855_NO_FAULT            0x00
#define MAX31855_FAULT_OPEN_CIRCUIT  0x01
#define MAX31855_FAULT_SHORT_GND     0x02
#define MAX31855_FAULT_SHORT_VCC     0x04
// ------------------------- Functions  ----------------------
float Max31855_Read_Temp(void);
float Max31855_Read_Internal_Temp(void);
MAX31855_Data MAX31855_ReadData(void);
#endif
