#include "software_iic.h"

void delay_us(uint16_t nus)
{ 
	__HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0);  // set the counter value a 0
	__HAL_TIM_ENABLE(DLY_TIM_Handle);          // start the counter
  // wait for the counter to reach the us input in the parameter
	while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < nus);
	__HAL_TIM_DISABLE(DLY_TIM_Handle);         // stop the counter
}

void IIC_Init(void)
{
  // 初始化SCL和SDA为开漏输出
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  IIC_SDA_GPIO_CLK_ENABLE();
  IIC_SCL_GPIO_CLK_ENABLE(); 

  GPIO_InitStruct.Pin = IIC_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);

  // 初始化SCL和SDA为高电平
  IIC_SCL_WRITE_UP();
  IIC_SDA_WRITE_UP();
}

// SDA输出模式
void IIC_SDA_OutputMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  IIC_SDA_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

// SDA输入模式
void IIC_SDA_InputMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  IIC_SDA_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

// IIC开始信号
void IIC_StartSignal(void)
{
  IIC_SDA_OutputMode();   // 设置SDA为输出模式

  // 确保SCL和SDA都是高电平 
  IIC_SCL_WRITE_UP();
  IIC_SDA_WRITE_UP();
  delay_us(5);            // 提高程序可靠性

  // 拉低SDA，产生一个下降沿
  // 一般常用的IIC总线标准速率为100kHz，即每个时钟周期为10us，故SDA低电平应持续5us
  IIC_SDA_WRITE_DOWN();   // SDA拉低
  delay_us(5);

 
  IIC_SCL_WRITE_DOWN();   // 拉低SCL，表示准备通信
}

// IIC停止信号
void IIC_StopSignal(void)
{
  IIC_SDA_OutputMode();  // 设置SDA为输出模式

  // 确保SCL和SDA都是低电平
  IIC_SCL_WRITE_DOWN();
  IIC_SDA_WRITE_DOWN();

  // 拉高SCL，产生一个上升沿
  // 一般常用的IIC总线标准速率为100kHz，即每个时钟周期为10us，故SCL高电平应持续5us
  IIC_SCL_WRITE_UP();
  delay_us(5);
  
  IIC_SDA_WRITE_UP(); // 拉高SDA，表示通信结束
  delay_us(5);        // 确保SDA的电平可以被其他器件检测到
}

// 主机发送数据
void IIC_SendBytes(uint8_t Data)
{
  uint8_t i = 0;

  IIC_SDA_OutputMode();  // 设置SDA为输出模式
      
  // 确保SCL和SDA都是低电平
  IIC_SCL_WRITE_DOWN();
  IIC_SDA_WRITE_DOWN();

  // 开始发送8位数据
  for (i = 0; i < 8; i++)
  {
    // SCL低电平期间主机准备数据
    if (Data & (1 << (7 - i))) // 判断数据的第7-i位是否为1
    {
      IIC_SDA_WRITE_UP();  // 如果为1，SDA拉高
    }
    else
    {
      IIC_SDA_WRITE_DOWN();// 如果为0，SDA拉低
    }
    delay_us(5); // 至此，数据准备完毕

    // 拉高SCL，主机发送数据
    IIC_SCL_WRITE_UP();    
    delay_us(5); // 至此，数据发送完毕

    // 拉低SCL，准备发送下一个数据
    IIC_SCL_WRITE_DOWN();
    delay_us(5);
  }
}

// 主机接收数据
uint8_t IIC_ReadBytes(void)
{
  uint8_t i = 0;
  uint8_t Data = 0; // 用于存储接收到的数据

  IIC_SDA_InputMode();  // 设置SDA为输入模式

  IIC_SCL_WRITE_DOWN(); // 确保SCL为低电平

  // 开始接收8位数据
  for (i = 0; i < 8; i++)
  {
    // 拉高SCL，主机准备接收数据
    IIC_SCL_WRITE_UP();
    delay_us(5);  // 至此，从机数据准备完毕，主机开始接收

    if (IIC_SDA_READ() == 1)   // 主机收到1
    {
      Data |= (1 << (7 - i));  // 将收到的1存储到Data的第7-i位
    }

    /* 由于Data初始化为0000 0000，所以不需要else语句
    else // 收到0
    {
      Data &= ~(1 << (7 - i)); // 将收到的0存储到Data的第7-i位
    }
    */
    
    // 拉低SCL，主机准备接收下一个数据
    IIC_SCL_WRITE_DOWN();
    delay_us(5);
  }
  return Data;  // 返回接收到的数据
}

// 主机发送数据，从机进行应答
uint8_t IIC_WaitACK(void)
{
  uint8_t ack;
  IIC_SDA_InputMode();  // 设置SDA为输入模式

  IIC_SCL_WRITE_DOWN(); // 确保SCL是低电平
  delay_us(5); 

  IIC_SCL_WRITE_UP();  // 拉高SCL，主机准备接收从机的应答信号
  delay_us(5);         // 至此，从机应答信号准备完毕，主机开始接收

  // 如果从机应答信号为0，表示从机接收到数据
  if (IIC_SDA_READ() == 0)
  {
    ack = 0;
  }
  else // 如果从机应答信号为1，表示从机没有接收到数据
  {
    ack = 1;
  }
  
  IIC_SCL_WRITE_DOWN(); // 拉低SCL，主机忽略数据
  delay_us(5);

  return ack;  // 返回从机的应答信号
}

// 从机发送数据，主机进行应答，0表示应答，1表示不应答
void IIC_MasterACK(uint8_t ack)
{
  IIC_SDA_OutputMode();  // 设置SDA为输出模式

  // 确保SCL和SDA都是低电平
  IIC_SCL_WRITE_DOWN();
  IIC_SDA_WRITE_DOWN();

  if (ack == 0) // 如果ack为0，表示主机应答
  {
    IIC_SDA_WRITE_DOWN(); // SDA拉低
  }
  else // 如果ack为1，表示主机不应答
  {
    IIC_SDA_WRITE_UP();   // SDA拉高
  }
  delay_us(5);  //  至此，应答信号准备完毕

  // 拉高SCL，主机发出应答信号
  IIC_SCL_WRITE_UP();
  delay_us(5);  // 至此，应答信号发送完毕

  // 拉低SCL，从机忽略数据
  IIC_SCL_WRITE_DOWN();
  delay_us(5);
}

/**
 * @brief AT24C02初始化
 * 
 */
void AT24C02_Init(void)
{
  IIC_Init();  // 初始化IIC总线
}

/**
 * @brief AT24C02字节写入
 * 
 * @param Address: 字节地址
 * @param Data: 待写入的数据
 */
void AT24C02_ByteWrite(uint8_t Address, uint8_t Data)
{
  IIC_StartSignal();        // 发送开始信号

  // 发送设备地址，写操作
  IIC_SendBytes(0xA0);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }

  // 发送字地址
  IIC_SendBytes(Address);   
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the word address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the word address: OK\n");
  }

  // 发送待写入的数据
  IIC_SendBytes(Data);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the data: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the data: OK\n");
  }

  IIC_StopSignal();         // 发送停止信号
}


/**
 * @brief AT24C02页编程（8字节/页）
 * 
 * @param Address: 页地址
 * @param buf: 待写入的数据
 * @param DataLen: 待写入的数据长度 
 */
void AT24C02_PageWrite(uint32_t Address, uint8_t *buf, uint8_t DataLen)
{
  IIC_StartSignal();        // 发送开始信号

  // 发送设备地址，写操作
  IIC_SendBytes(0xA0);
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }

  // 发送页地址
  IIC_SendBytes(Address);
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the page address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the page address: OK\n");
  }

  // 循环发送数据
  while (DataLen--)
  {
    IIC_SendBytes(*buf++);  // 发送数据
    if (IIC_WaitACK() == 1) // AT24C02没应答
    {
      printf("[AT24C02] Answered the data: Error\n");
      IIC_StopSignal();     // 发送停止信号
    }
    else
    {
      printf("[AT24C02] Answered the data: OK\n");
    }
  }

  IIC_StopSignal();         // 发送停止信号
}

/**
 * @brief AT24C02当前地址读
 * 
 * @return uint8_t 当前地址的数据
 */
uint8_t AT24C02_CurrentAddressRead(void)
{
  uint8_t Data;

  IIC_StartSignal(); // 发送开始信号

  // 发送设备地址，读操作
  IIC_SendBytes(0xA1);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }
  
  // 读取1字节数据
  Data = IIC_ReadBytes();

  // 发送应答信号
  IIC_MasterACK(1); // 不应答

  IIC_StopSignal(); // 发送停止信号
  return Data;      // 返回接收到的数据
}

#if 0
/**
 * @brief AT24C02随机读
 * 
 * @param Address: 字地址 
 * @return uint8_t 读取到的数据
 */
uint8_t AT24C02_RandomRead(uint8_t Address)
{
  uint8_t Data;

  IIC_StartSignal(); // 发送开始信号

  // 发送设备地址，写操作
  IIC_SendBytes(0xA0);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }

  // 发送字数据的地址
  IIC_SendBytes(Address);   
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the word address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the word address: OK\n");
  }

  IIC_StartSignal();        // 发送开始信号

  // 发送设备地址，读操作
  IIC_SendBytes(0xA1);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }
  
  // 读取1字节数据
  Data = IIC_ReadBytes();

  // 发送应答信号
  IIC_MasterACK(1); // 不应答

  IIC_StopSignal(); // 发送停止信号

  return Data;      // 返回接收到的数据
}
#endif

/**
 * @brief AT24C02随机读n个字节
 * 
 * @param Address: 字地址 
 * @param RecvBuf: 接收缓冲区 
 * @param DataLen: 接收数据长度 
 */
void AT24C02_RandomRead(uint8_t Address, uint8_t *RecvBuf, uint8_t DataLen)
{
  IIC_StartSignal(); // 发送开始信号

  // 发送设备地址，写操作
  IIC_SendBytes(0xA0);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }

  // 发送字数据的地址
  IIC_SendBytes(Address);   
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the word address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the word address: OK\n");
  }

  IIC_StartSignal();        // 发送开始信号

  // 发送设备地址，读操作
  IIC_SendBytes(0xA1);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }
  
  // 读取n字节数据
  DataLen -= 1;
  while (DataLen--) // 读取n-1字节数据,最后1字节数据单独读取
  {
    *RecvBuf++ = IIC_ReadBytes();

    // 发送应答信号
    IIC_MasterACK(0); // 应答
  }
  *RecvBuf++ = IIC_ReadBytes(); // 读取最后1字节数据
  
  // 发送应答信号
  IIC_MasterACK(1); // 不应答

  IIC_StopSignal(); // 发送停止信号
}

/**
 * @brief AT24C02顺序读n个字节
 * 
 * @param Address: 字地址 
 * @param RecvBuf: 接收缓冲区 
 * @param DataLen: 接收数据长度 
 */
void AT24C02_SequentialRead(uint8_t Address, uint8_t *RecvBuf, uint8_t DataLen)
{
  IIC_StartSignal(); // 发送开始信号

  // 发送设备地址，写操作
  IIC_SendBytes(0xA0);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }

  // 发送字数据的地址
  IIC_SendBytes(Address);   
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the word address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the word address: OK\n");
  }

  IIC_StartSignal();        // 发送开始信号

  // 发送设备地址，读操作
  IIC_SendBytes(0xA1);     
  if (IIC_WaitACK() == 1)   // AT24C02没应答
  {
    printf("[AT24C02] Answered the device address: Error\n");
    IIC_StopSignal();       // 发送停止信号
  }
  else
  {
    printf("[AT24C02] Answered the device address: OK\n");
  }
  
  // 读取n字节数据
  while (DataLen--) // 读取n字节数据
  {
    *RecvBuf++ = IIC_ReadBytes();

    // 发送应答信号
    if (DataLen > 0) {
      IIC_MasterACK(0); // 应答
    } else {
      IIC_MasterACK(1); // 不应答
    }
  }

  IIC_StopSignal(); // 发送停止信号
}
