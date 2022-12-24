# 定时器PWM输出功能

# PWM工作原理

脉冲宽度调制（PWM：Pulse Width Modulation）是一种对模拟信号电平进行数字编码的方法。广泛应用于电机控制、灯光的亮度调节、功率控制等领域。

## **PWM信号的两个基本参数**

- **周期（Period）：**一个完整PWM波形所持续的时间
- **占空比（Duty）：**高电平持续时间（Ton）与周期时间（Period）的比值

## **PWM信号的电压调节原理**

![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled.png)

- 不同占空比的PWM信号等效于不同的平均电压
- 平均电压 = 峰值 x 占空比

## 定时器通道结构

![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled%201.png)

- 时基单元工作于定时模式，预分频时钟CK_PSC等于定时器时钟TIMx_CLK。
- 每个定时器具备1~4个独立的通道，各个通道具有独立的**输入捕获单元**、**捕获/比较寄存器**和**输出比较单元**，但共享同一个**时基单元。**
- x表示定时器编号，每个通道有对应的GPIO引脚作为通道的输入/输出引脚，每个通道可以选择作为输入捕获或者输出比较功能，但是只能二选一。

## 功能单元的作用

- **输入捕获单元：**用于捕获外部触发信号，捕获方式为上升沿/下降沿/双边沿捕获。发生捕获事件时，将此刻计数器的值锁存到捕获比较寄存器中，供用户读取，同时可以产生捕获中断。
- **捕获/比较寄存器：**TIMx_CCR寄存器在输入捕获模式下用于存放发生捕获事件时的当前计数值；在输出比较模式下用于存放预设的比较值。该寄存器具备预装载功能。
- **输出比较单元：**用于信号输出。定时器通过将预设的比较值与计数器的值做匹配比较，以实现各类输出，如PWM输出、单脉冲输出等。预设的比较值存放在捕获/比较寄存器中。

## **PWM输出的工作原理**

![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled%202.png)

- 实例：假设预分频时钟CK_PSC为100MHz，产生周期为1ms，占空比为47.5%的PWM信号。Period = 1ms，可以设置 PSC = 99，ARR = 999，Duty = 47.5%，则 CRR = 475。

定时器的每个通道都可以输出PWM信号，对于同一个定时器而言，它的多个通道共享同一个自动重载寄存器，因此可以输出占空比不同，但周期相同的PWM信号。

![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled%203.png)

# **PWM功能的数据类型和接口函数**

## 输出比较单元

![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled%204.png)

**输出比较模式：成员变量OCMode的取值范围**

| 宏常量定义 | 含义 |
| --- | --- |
| IM_OCMODE_TIMING | 输出比较冻结模式，匹配时无通道输出 |
| TIM_OCMODE_ACTIVE | 输出比较冻结模式，匹配时无通道输出 |
| TIM_OCMODE_INACTIVE | 匹配时设置通道输出为无效电平 |
| TIM_OCMODE_TOGGLE | 匹配时设置通道输出电平翻转 |
| TIM_OCMODE_PWM1 | PWM输出模式1 |
| TIM_OCMODE_PWM2 | PWM输出模式2 |
| TIM_OCMODE_FORCED_ACTIVE | 不进行匹配，强制通道输出为有效电平 |
| TIM_OCMODE_FORCED_INACTIVE | 不进行匹配，强制通道输出为无效电平 |
- PWM模式1：递增计数时，当TIMx_CNT (当前计数值) <TIMx_CCR (捕获/比较值) 时，通道输出为有效电平，否则为无效电平。递减计数模式则刚好相反。
- PWM模式2：递增计数时，当TIMx_CNT (当前计数值) <TIMx_CCR (捕获/比较值) 时，通道输出为无效电平，否则为有效电平。递减计数模式则刚好相反。

**有效电平极性：成员变量OCPolarity的取值范围**

| 宏常量定义 | 含义 |
| --- | --- |
| TIM_OCPOLARITY_HIGH | 输出有效电平为高电平 |
| TIM_OCPOLARITY_LOW | 输出有效电平为低电平 |

**快速输出使能：成员变量OCFastMode的取值范围**

| 宏常量定义 | 含义 |
| --- | --- |
| TIM_OCFAST_DISABLE | 不使能快速输出模式 |
| TIM_OCPOLARITY_LOWTIM_OCFAST_ENABLE | 使能快速输出模式 |
- 可以加快触发输入事件对通道输出的影响，默认配置为不使能
1. **定时器PWM输出启动函数：`HAL_TIM_PWM_Start`**
   
   
    | 函数原型 | `HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel) `|
    | --- | --- |
    | 功能描述 | 在轮询方式下启动PWM信号输出 |
    | 入口参数 | `htim`：定时器句柄的地址 |
    | 返回值 | HAL状态值 |
    | 注意事项 | 1. 该函数在定时器初始化完成之后调用<br />2. 函数需要由用户调用，用于启动定时器的指定通道输出PWM信号行 |
    
2. **定时器比较/捕获寄存器设置函数： `__HAL_TIM_SET_COMPARE`**
   
   
    | 函数原型 |` __HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __COMPARE__) `|
    | --- | --- |
    | 功能描述 | 设置捕获/比较寄存器TIMx_CCR的值。在PWM输出时， 用于改变PWM 信号的占空比 |
    | 入口参数 | `__HANDLE__`： 定时器句柄的地址<br />`__CHANNEL__`： 定时器通道号， 取值范围是TIM_CHANNEL_1 ~TIM_CHANNEL_4<br />`__COMPARE__` ：写入捕获/比较寄存器TIMx_CCR的值 |
    | 返回值 | 无 |
    | 注意事项 | 1. 该函数是宏函数，进行宏替换，不发生函数调用<br />2. 函数需要由用户调用，用于PWM输出时，改变PWM信号的占空比 |

# 任务实践1

用PWM信号控制发板上的指示灯LED1。设置PWM周期为 20 ms，占空比从0%开始，步进为20%。递增到100%后，又从0%开始，并重复整个过程。占空比修改的时间间隔为200ms。

经过计算可得，可以取值：PSC=799，ARR=199，CRR从0到200，步进值为40。

1. 配置定时器2时钟源为内部时钟 (8MHz)，通道1配置为PWM输出模式，设置PSC=799，ARR199，Pulse设置为0。
   
    ![Untitled](%E5%AE%9A%E6%97%B6%E5%99%A8PWM%E8%BE%93%E5%87%BA%E5%8A%9F%E8%83%BD/Untitled%205.png)
    
2. 编写代码
   
    在`main.c`中
    
    ```c
    /* USER CODE BEGIN PV */
    uint16_t CCR = 0;
    uint16_t Step = 40;
    /* USER CODE END PV */
    
    /* USER CODE BEGIN 2 */
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
     /* USER CODE END 2 */
    
    /* USER CODE BEGIN 3 */
        for (CCR = 0; CCR <= 200; CCR = CCR + Step)
        {
          __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, CCR);
          HAL_Delay(200);
        }
      }
    /* USER CODE END 3 */
    ```