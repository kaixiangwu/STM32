/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * 从机型号：W25Q128
 * 引脚定义：/CS：PB14，SCK：PB3，MISO：PB4，MOSI：PB5
 *
 */
void W25Q128_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  SPI_HandleTypeDef hspi1;

  // 1.打开SPI1和GPIOA外设时钟
  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // 2.配置SPI相关GPIO引脚PB3、PB4、PB5和/CS引脚PB14(/CS低电平有效)
  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;   // 引脚编号
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                       // 复用推挽输出
  GPIO_InitStruct.Pull = GPIO_NOPULL;                           // 无上下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;                    // 复用模式SPI1
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);          // 拉高CS(PB14)
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // 3.配置SPI外设参数
  hspi1.Instance = SPI1;                        // hspi1实例为SPI1
  hspi1.Init.Mode = SPI_MODE_MASTER;            // 主机模式                
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;  // 双线全双工
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;      // 数据位8bit
  hspi1.Init.CLKPolarity =  SPI_POLARITY_HIGH;  // 模式3时钟极性高
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;        // 模式3第二个边沿采集数据
  hspi1.Init.NSS = SPI_NSS_SOFT;                // 软件控制NSS
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;   // 2分频APB2
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;                   // W25Q128高位先出
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE; 
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
}

// 通时SPI外设发送一个字节数据同时会收到一个字节
uint8_t W25Q128_SendByte(uint8_t byte)
{
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE ) == RESET);
  WRITE_REG(hspi1.Instance->DR, byte);
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) == RESET);
  return READ_REG(hspi1.Instance->DR);
}

uint16_t W25Q128_ReadDeviceID(void)
{
  uint16_t ID = 0;

  // 1. 拉低CS片选引脚
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  // 2. 发送指令0x90
  W25Q128_SendByte(0x90);

  // 3. 发送地址 24bit
  W25Q128_SendByte(0x00);
  W25Q128_SendByte(0x00);
  W25Q128_SendByte(0x00);

  // 4. 接收厂商ID和设备ID, 主机发送任意数据即可
  ID = W25Q128_SendByte(0x00) << 8;   // 接收厂商ID后左移八位然后接收设备ID
  ID |= W25Q128_SendByte(0x00);       // 接收设备ID

  // 5. 拉高CS表示不再通信
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  return ID;
}

// 写使能
void W25Q128_WriteEnable(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  W25Q128_SendByte(0x60);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

// 写失能
void W25Q128_WriteDisable(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  W25Q128_SendByte(0x04);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

uint8_t W25Q128_ReadStatusRegister1(void)
{
  uint8_t status;

  // 1. 拉低CS片选引脚
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  // 2. 发送指令
  W25Q128_SendByte(0x05);

  // 3. 发送任意字节数据，目的是接收一个字节的返回值
  status = W25Q128_SendByte(0x00);

  // 4. 拉高CS表示不再通信
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  return status;
}

void W25Q128_SectorErase(uint32_t Address)
{
  // 1. 开启写使能
  W25Q128_WriteEnable();

  // 2. 拉低CS片选引脚
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  // 3. 发送擦除指令20h
  W25Q128_SendByte(0x20);

  // 4. 发送地址
  // 对于Flash的扇区而言，如果之前扇区已经被擦除过的话，扇区内部数据默认是0xFF
  W25Q128_SendByte((Address & 0xFF0000) >> 16);
  W25Q128_SendByte((Address & 0xFF000) >> 8);
  W25Q128_SendByte((Address) & 0xFF);

  // 4. 拉高CS表示不再通信
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
  
  // 6. 等待擦除完成
  while (W25Q128_ReadStatusRegister1() & 0x01);

  // 7. 关闭写使能
  W25Q128_WriteDisable();

}

void W25Q128_ReadData(uint32_t Address, uint8_t *RecvBuf, uint32_t ReadLen)
{
  // 1. 拉低CS片选引脚
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  // 2. 发送指令
  W25Q128_SendByte(0x03);

  // 3. 发送地址
  W25Q128_SendByte((Address & 0xFF0000) >> 16);
  W25Q128_SendByte((Address & 0xFF000) >> 8);
  W25Q128_SendByte((Address) & 0xFF);

  // 4. 接收数据
  while (ReadLen--)
  {
    *RecvBuf++ = W25Q128_SendByte(0x00);
  }

  // 5. 拉高CS表示不再通信
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

}

void W25Q128_PageProgram(uint32_t Address, uint8_t *RecvBuf, uint32_t ReadLen)
{
  // 1. 开启写使能
  W25Q128_WriteEnable();

  // 2. 拉低CS片选引脚
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  // 3. 发送擦除指令02h
  W25Q128_SendByte(0x02);

  // 4. 发送地址
  // 对于Flash的扇区而言，如果之前扇区已经被擦除过的话，扇区内部数据默认是0xFF
  W25Q128_SendByte((Address & 0xFF0000) >> 16);
  W25Q128_SendByte((Address & 0xFF000) >> 8);
  W25Q128_SendByte((Address) & 0xFF);

  // 5. 发送数据
  while (ReadLen--)
  {
    W25Q128_SendByte(*RecvBuf++);
  }

  // 6. 拉高CS表示不再通信
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  // 7. 等待写入完成
  while (W25Q128_ReadStatusRegister1() & 0x01);

  // 8. 关闭写使能
  W25Q128_WriteDisable();

}
/* USER CODE END 1 */
