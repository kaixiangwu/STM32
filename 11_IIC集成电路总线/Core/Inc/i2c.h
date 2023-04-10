#ifndef __I2C_H__
#define __I2C_H__

#include "main.h"

#define IIC_SCL_PORT    GPIOB
#define IIC_SCL_PIN     GPIO_PIN_6
#define IIC_SDA_PORT    GPIOB
#define IIC_SDA_PIN     GPIO_PIN_7

void IIC_Init(void);
void IIC_SDA_OutputMode(void);
void IIC_SDA_InputMode(void);
void IIC_StartSignal(void);
void IIC_SendBytes(uint8_t Data);
uint8_t IIC_ReadBytes(void);
uint8_t IIC_WaitACK(void);
void IIC_MasterACK(uint8_t ACK);
void IIC_StopSignal(void);

#endif
