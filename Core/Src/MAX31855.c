/*************************************************************************************
 Title	 :  MAXIM Integrated MAX31855 Library for STM32 Using HAL Libraries
 Author  :  Bardia Alikhan Afshar <bardia.a.afshar@gmail.com>
 Software:  STM32CubeIDE
 Hardware:  Any STM32 device
*************************************************************************************/
#include"MAX31855.h"

#define MAX31855_SPI_TIMEOUT 100

// ------------------- Variables ----------------

//uint8_t TC_Error=0;                                      // Thermocouple Connection acknowledge Flag
//uint32_t sign=0;									  // Sign bit
//uint8_t data_rec[4];                                    // Raw Data from MAX6675
//// ------------------- Functions ----------------
//float Max31855_Read_Temp(void){
//	int Temp=0;                                           // Temperature Variable
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);       // Low State for SPI Communication
//	HAL_SPI_Receive(TC_HSPI_INSTANCE,data_rec,4,100);                // DATA Transfer
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);         // High State for SPI Communication
//
//	TC_Error=data_rec[3]&0x07;								  // Error Detection
//	sign=(data_rec[0]&(0x80))>>7;							  // Sign Bit calculation
//
//	if(TC_Error) {								  // Returns Error Number
//		return(-1*(TC_Error));
//	} else if(sign==1) {  // Negative Temperature
//		Temp = (data_rec[0] << 6) | (data_rec[1] >> 2);
//		Temp&=0b01111111111111;
//		Temp^=0b01111111111111;
//		return((double)-Temp/4);
//	} else {  // Positive Temperature
//		Temp = (data_rec[0] << 6) | (data_rec[1] >> 2);
//		return((double)Temp / 4);
//	}
//}
//
//float Max31855_Read_Internal_Temp(void){
//	int Temp=0;                                           // Temperature Variable
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);       // Low State for SPI Communication
//	HAL_SPI_Receive(TC_HSPI_INSTANCE,data_rec,4,100);                // DATA Transfer
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);         // High State for SPI Communication
//
//	TC_Error=data_rec[3]&0x07;								  // Error Detection
//	sign=(data_rec[2]&(0x80))>>7;							  // Sign Bit calculation
//
//	if(TC_Error) {								  // Returns Error Number
//		return(-1*(TC_Error));
//	} else if(sign==1) {  // Negative Temperature
//		Temp = (data_rec[2] << 4) | (data_rec[3] >> 4);
//		Temp&=0b01111111111111;
//		Temp^=0b01111111111111;
//		return((double)-Temp/4);
//	} else {  // Positive Temperature
//		Temp = (data_rec[2] << 4) | (data_rec[3] >> 4);
//		return((double)Temp / 16);
//	}
//}

MAX31855_Data MAX31855_ReadData(void) {
    MAX31855_Data data;
    uint8_t buffer[4] = {0};

    HAL_GPIO_WritePin(MAX31855_CS_PORT, MAX31855_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Receive(&hspi2, buffer, 4, MAX31855_SPI_TIMEOUT) != HAL_OK) {
        // Handle SPI error
        data.temperature = 0;
        data.internalTemperature = 0;
        data.fault = 0xFF;
        HAL_GPIO_WritePin(MAX31855_CS_PORT, MAX31855_CS_PIN, GPIO_PIN_SET);
        return data;
    }

    HAL_GPIO_WritePin(MAX31855_CS_PORT, MAX31855_CS_PIN, GPIO_PIN_SET);

    int32_t rawTemperature = ((buffer[0] << 24) | (buffer[1] << 16)) >> 18;
    int16_t rawInternalTemperature = ((buffer[2] << 8) | buffer[3]) >> 4;

    data.temperature = rawTemperature * 0.25;
    data.internalTemperature = rawInternalTemperature * 0.0625;
    data.fault = buffer[3] & 0x07;

    return data;
}
