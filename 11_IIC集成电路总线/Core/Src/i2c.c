#include "i2c.h"
#include "delay_us.h"

#define IIC_SCL_WRITE_UP        HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_SET)  
#define IIC_SCL_WRITE_DOWN      HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_RESET)  
#define IIC_SDA_WRITE_UP        HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_SET)  
#define IIC_SDA_WRITE_DOWN      HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_RESET) 
#define IIC_SDA_READ            HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN)

void IIC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = IIC_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);

  IIC_SCL_WRITE_UP;
  IIC_SDA_WRITE_UP;

}
void IIC_SDA_OutputMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

void IIC_SDA_InputMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

void IIC_StartSignal(void)
{
  IIC_SDA_OutputMode();
  IIC_SCL_WRITE_UP;
  IIC_SDA_WRITE_UP;
  IIC_SDA_WRITE_DOWN;
  delay_us(5);
  IIC_SCL_WRITE_DOWN;
}

void IIC_SendBytes(uint8_t Data)
{
  uint8_t i = 0;

  IIC_SDA_OutputMode();
  for (i = 0; i < 8; i++)
  {
    IIC_SDA_WRITE_DOWN;
    IIC_SCL_WRITE_DOWN;
    if (Data & (1<<(7 - i)))
    {
      IIC_SDA_WRITE_UP;
    }
    else
    {
      IIC_SDA_WRITE_DOWN;
    }
    delay_us(5);

    IIC_SCL_WRITE_UP;
    delay_us(5);
    IIC_SCL_WRITE_DOWN;
    delay_us(5);

  }
}

uint8_t IIC_ReadBytes(void)
{
  uint8_t i = 0;
  uint8_t Data;

  IIC_SDA_InputMode();
  IIC_SCL_WRITE_DOWN;
  delay_us(5);
  for (i = 0; i < 8; i++)
  {
    IIC_SCL_WRITE_UP;
    delay_us(5);

    if (IIC_SDA_READ == 1)
    {
      Data |= 1<<(7-i);
    }
    IIC_SCL_WRITE_DOWN;
    delay_us(5);
  }
  return Data;
}

uint8_t IIC_WaitACK(void)
{
  uint8_t ACK;

  IIC_SDA_InputMode();
  IIC_SCL_WRITE_DOWN;
  delay_us(5);
  IIC_SCL_WRITE_UP;
  delay_us(5);
  if (IIC_SDA_READ == 1)
    ACK = 1;
  else
    ACK = 0;
  IIC_SCL_WRITE_DOWN;
  delay_us(5);

  return ACK;
}

void IIC_MasterACK(uint8_t ACK)
{
  IIC_SDA_OutputMode();
  IIC_SDA_WRITE_DOWN;
  IIC_SCL_WRITE_DOWN;
  if (ACK == 1)
  {
    IIC_SDA_WRITE_UP;
  }
  else
  {
    IIC_SDA_WRITE_DOWN;
  }
  delay_us(5);

  IIC_SCL_WRITE_DOWN;
  delay_us(5);
  
}

void IIC_StopSignal(void)
{
  IIC_SDA_OutputMode();
  IIC_SCL_WRITE_DOWN;
  IIC_SDA_WRITE_DOWN;
  IIC_SCL_WRITE_UP;
  delay_us(5);
  IIC_SDA_WRITE_UP;
  delay_us(5);
}


