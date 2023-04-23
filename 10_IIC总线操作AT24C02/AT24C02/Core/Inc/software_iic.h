#ifndef __SOFTWARE_IIC_H__
#define __SOFTWARE_IIC_H__

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "stdio.h"

#define DLY_TIM_Handle (&htim1)


// SCL: PB6, SDA: PB7
#define IIC_SCL_PORT GPIOB
#define IIC_SCL_PIN  GPIO_PIN_6
#define IIC_SDA_PORT GPIOB
#define IIC_SDA_PIN  GPIO_PIN_7

#define IIC_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define IIC_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define IIC_SCL_WRITE_UP()    HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_SET)
#define IIC_SCL_WRITE_DOWN()  HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_RESET)
#define IIC_SDA_WRITE_UP()    HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_SET)
#define IIC_SDA_WRITE_DOWN()  HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_RESET)

#define IIC_SDA_READ()        HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN)

void delay_us(uint16_t nus);

void IIC_Init(void);
void IIC_SDA_OutputMode(void);
void IIC_SDA_InputMode(void);
void IIC_StartSignal(void);
void IIC_StopSignal(void);
void IIC_SendBytes(uint8_t data);
uint8_t IIC_ReadBytes(void);
uint8_t IIC_WaitACK(void);
void IIC_MasterACK(uint8_t ack);

void AT24C02_Init(void);
void AT24C02_PageWrite(uint32_t Address, uint8_t *buf, uint8_t DataLen);
void AT24C02_ByteWrite(uint8_t Address, uint8_t Data);
uint8_t AT24C02_CurrentAddressRead(void);
// uint8_t AT24C02_RandomRead(uint8_t Address);
void AT24C02_RandomRead(uint8_t Address, uint8_t *RecvBuf, uint8_t DataLen);
#endif
