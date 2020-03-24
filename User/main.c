/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhang Hongda, a64091937
  * @version V1.0
  * @date    2019-9-8
  * @brief   A Standard STM32 Project
  ******************************************************************************
  * @attention
  * 程序来到main函数之前，启动文件：statup_stm32f429xx.s已经调用
  * SystemInit()函数把系统时钟初始化成180MHZ
  *  SystemInit()在system_stm32f4xx.c中定义
  * 如果用户想修改系统时钟，可自行编写程序修改
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./rcc/bsp_clkconfig.h"
#include "./exti/bsp_exti.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsP_i2c_ee.h"
#include "./flash/bsp_spi_flash.h" 

/* 相关宏定义，使用存储器到存储器传输必须使用DMA2 */
#define DMA_STREAM               DMA2_Stream0
#define DMA_CHANNEL              DMA_Channel_0
#define DMA_STREAM_CLOCK         RCC_AHB1Periph_DMA2
#define DMA_FLAG_TCIF            DMA_FLAG_TCIF0

#define BUFFER_SIZE              32
#define TIMEOUT_MAX              10 /* Maximum timeout value */
#define  EEP_Firstpage      0x00
uint8_t I2c_Buf_Write[256];
uint8_t I2c_Buf_Read[256];
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

   
/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "感谢您选用野火stm32开发板\r\nhttps://fire-stm32.taobao.com";
uint8_t Rx_Buffer[BufferSize];

//读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
  const关键字将aSRC_Const_Buffer数组变量定义为常量类型 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] =
{
    0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
    0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
    0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
    0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
    0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
    0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
    0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
    0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};
/* 定义DMA传输目标存储器 */
uint32_t aDST_Buffer[BUFFER_SIZE];


static void Delay(__IO uint32_t nCount);
static void DMA_Config(void);
uint8_t Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength);

void Delay(__IO u32 nCount);
void LED_test11(void);
void KEY_test12(void);
void RCC_test15(void);
void EXTI_test17(void);
void UART_test18_1(void);
void UART_test18_2(void);
void DMA_test19_1(void);	 // DMA1
static void Show_Message(void);
void Flash_test1_1(void);    // Flash1
void I2C_test23_1(void);	 // I2C1
void SPI_FLASH_test24_1(void);
uint8_t I2C_Test(void);
uint8_t Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength);
TestStatus Buffercmp1(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

    // LED_test11(); // 第11章的代码
    // KEY_test12(); // 第12章的代码
    // RCC_test15(); // 第15章的代码
    //EXTI_test17(); // 第17章的代码
    // UART_test18_1(); // 第18_1章的代码
    //UART_test18_2(); // 第18_2章的代码
    //DMA_test19_1();
    //Flash_test1_1();
    //I2C_test23_1();
    SPI_FLASH_test24_1();
    return 0;

}

void LED_test11(void)	 // LED闪烁的代码
{
    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    for (;;)
    {
        LED1( ON ); 		 // 亮
        Delay(0xFFFFFF);
        LED1( OFF );		  // 灭

        LED2( ON ); 		// 亮
        Delay(0xFFFFFF);
        LED2( OFF );		  // 灭

        LED3( ON ); 		 // 亮
        Delay(0xFFFFFF);
        LED3( OFF );		  // 灭

        /*轮流显示 红绿蓝黄紫青白 颜色*/
        LED_RED;
        Delay(0xFFFFFF);

        LED_GREEN;
        Delay(0xFFFFFF);

        LED_BLUE;
        Delay(0xFFFFFF);

        LED_YELLOW;
        Delay(0xFFFFFF);

        LED_PURPLE;
        Delay(0xFFFFFF);

        LED_CYAN;
        Delay(0xFFFFFF);

        LED_WHITE;
        Delay(0xFFFFFF);

        LED_RGBOFF;
        Delay(0xFFFFFF);

    }
}
void KEY_test12(void)	 // KEY反转LED的代码
{
    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    /* 轮询按键状态，若按键按下则反转LED */
    for (;;)
    {
        if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON  )
        {
            /*LED1反转*/
            LED1_TOGGLE;
        }

        if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON  )
        {
            /*LED2反转*/
            LED2_TOGGLE;
        }
    }
}

void RCC_test15(void)	 // KEY反转LED的代码
{
    // 程序来到main函数之前，启动文件：statup_stm32f429_439xx.s已经调用
    // SystemInit()函数把系统时钟初始化成72MHZ
    // SystemInit()在system_stm32f4xx.c中定义
    // 如果用户想修改系统时钟，可自行编写程序修改
    // 重新设置系统时钟，这时候可以选择使用HSE还是HSI

    // 使用HSE，配置系统时钟为180M
    HSE_SetSysClock(25, 360, 2, 7);

    //系统时钟超频到216M爽一下，最高是216M，别往死里整
    //HSE_SetSysClock(25, 432, 2, 9);

    // 使用HSI，配置系统时钟为180M
    //HSI_SetSysClock(16, 360, 2, 7);

    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();

    // MCO GPIO  初始化
    MCO1_GPIO_Config();
    MCO2_GPIO_Config();

    // MCO1 输出PLLCLK
    RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);

    // MCO2 输出SYSCLK
    RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO1Div_1);

    while (1)
    {
        LED1( ON );			 // 亮
        Delay(0xFFFFFF);
        LED1( OFF );		  // 灭
        Delay(0xFFFFFF);
    }
}

void EXTI_test17(void)	 // KEY反转LED的代码
{
    /* 初始化EXTI中断，按下按键会触发中断，
    *  触发中断会进入stm32f4xx_it.c文件中的函数
    *  KEY1_IRQHandler和KEY2_IRQHandler，处理中断，反转LED灯。
    */
    EXTI_Key_Config();
    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    /* 等待中断，由于使用中断方式，CPU不用轮询按键 */
    while(1)
    {
    }
}
void UART_test18_1(void)	 // USART_USART1接发
{
    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    Debug_USART_Config();
    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    /* 发送一个字符串 */
    Usart_SendString( DEBUG_USART, "这是一个串口中断接收回显实验\n");
    // printf("这是一个串口中断接收回显实验\n");

    while(1)
    {

    }
}
void UART_test18_2(void)	 // USART_USART1彩灯
{

    char ch;

    /* 初始化RGB彩灯 */
    LED_GPIO_Config();

    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    LED_USART_Config();


    /* 打印指令输入提示信息 */
    Show_Message();
    while(1)
    {
        /* 获取字符指令 */
        ch = getchar();
        printf("接收到字符：%c\n", ch);

        /* 根据字符指令控制RGB彩灯颜色 */
        switch(ch)
        {
        case '1':
            LED_RED;
            break;
        case '2':
            LED_GREEN;
            break;
        case '3':
            LED_BLUE;
            break;
        case '4':
            LED_YELLOW;
            break;
        case '5':
            LED_PURPLE;
            break;
        case '6':
            LED_CYAN;
            break;
        case '7':
            LED_WHITE;
            break;
        case '8':
            LED_RGBOFF;
            break;
        default:
            /* 如果不是指定指令字符，打印提示信息 */
            Show_Message();
            break;
        }
    }
}

void DMA_test19_1(void)	 // DMA1
{
    /* 定义存放比较结果变量 */
    uint8_t TransferStatus;
    //uint8_t *flash_addr = 0x0;

    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    LED_USART_Config();
    /* 设置RGB彩色灯为紫色 */
    LED_PURPLE;

    /* 简单延时函数 */
    Delay(0xFFFFFF);

    /* DMA传输配置 */
    DMA_Config();

    /* 等待DMA传输完成 */
    while(DMA_GetFlagStatus(DMA_STREAM, DMA_FLAG_TCIF) == DISABLE)
    {

    }

    /* 比较源数据与传输后数据 */
    TransferStatus = Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);

    /* 判断源数据与传输后数据比较结果*/
    if(TransferStatus == 0)
    {
        /* 源数据与传输后数据不相等时RGB彩色灯显示红色 */
        LED_RED;
    }
    else
    {
        /* 源数据与传输后数据相等时RGB彩色灯显示蓝色 */
        LED_BLUE;
    }

    while (1)
    {
        printf("src_addr %x,out_addr %x,tmp_addr %x\n", aSRC_Const_Buffer, aDST_Buffer, &TransferStatus);
        printf("src0 %x,out0 %x\n", aSRC_Const_Buffer[0], aDST_Buffer[0]);
        //printf("Flash_addr %x,Flash_num %x\n",&flash_addr[0],flash_addr[0]);
        Delay(0xFFFFFF);
    }
}

void I2C_test23_1(void)	 // I2C1
{
    LED_GPIO_Config();

    LED_BLUE;
    /*初始化USART1*/
    Debug_USART_Config();

    printf("\r\n 欢迎使用野火  STM32 F429 开发板。\r\n");

    printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");

    /* I2C 外设初(AT24C02)始化 */
    I2C_EE_Init();

    if(I2C_Test() == 1)
    {
        LED_GREEN;
    }
    else
    {
        LED_RED;
    }

    while (1)
    {
    }
}
void SPI_FLASH_test24_1(void)	 // SPI1
{
	LED_GPIO_Config();
	LED_BLUE;
	
	/* 配置串口1为：115200 8-N-1 */
	Debug_USART_Config();
  
	printf("\r\n这是一个16M串行flash(W25Q128)实验 \r\n");
	
	/* 16M串行flash W25Q128初始化 */
	SPI_FLASH_Init();
	
	/* 获取 Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	
	Delay( 200 );
	
	/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	
	printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	
	/* 检验 SPI Flash ID */
	if (FlashID == sFLASH_ID) 
	{	
		printf("\r\n检测到SPI FLASH W25Q128 !\r\n");
		
		/* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* 将发送缓冲区的数据写到flash中 */
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\n写入的数据为：\r\n%s", Tx_Buffer);
		
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n读出的数据为：\r\n%s", Rx_Buffer);
		
		/* 检查写入的数据与读出的数据是否相等 */
		TransferStatus1 = Buffercmp1(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{    
			LED_GREEN;
			printf("\r\n16M串行flash(W25Q128)测试成功!\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n16M串行flash(W25Q128)测试失败!\n\r");
		}
	}// if (FlashID == sFLASH_ID)
	else
	{    
		LED_RED;
		printf("\r\n获取不到 W25Q128 ID!\n\r");
	}
	
	SPI_Flash_PowerDown();  
	while(1);  
}
//////////////////////////////以下是支持函数////////////////////////////
void Flash_test1_1(void)	 // Flash1
{
    /* 定义存放比较结果变量 */
    uint32_t num = 0;
    uint32_t *flash_addr = (uint32_t *)0x08000000;

    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    LED_USART_Config();
    /* 设置RGB彩色灯为紫色 */
    LED_PURPLE;



    while (num <= 0x1FFFFF)
    {
        //printf("src_addr %x,out_addr %x,tmp_addr %x\n",aSRC_Const_Buffer,aDST_Buffer,&TransferStatus);
        //printf("src0 %x,out0 %x\n",aSRC_Const_Buffer[0],aDST_Buffer[0]);
        printf("Flash_addr %x,Flash_num %x\n", &flash_addr[num], flash_addr[num]);
        num++;
        //Delay(0xFFFFFF);
    }
}


static void Show_Message(void)
{
    printf("\r\n   这是一个通过串口通信指令控制RGB彩灯实验 \n");
    printf("使用  USART1  参数为：%d 8-N-1 \n", DEBUG_USART_BAUDRATE);
    printf("开发板接到指令后控制RGB彩灯颜色，指令对应如下：\n");
    printf("   指令   ------ 彩灯颜色 \n");
    printf("     1    ------    红 \n");
    printf("     2    ------    绿 \n");
    printf("     3    ------    蓝 \n");
    printf("     4    ------    黄 \n");
    printf("     5    ------    紫 \n");
    printf("     6    ------    青 \n");
    printf("     7    ------    白 \n");
    printf("     8    ------    灭 \n");
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
    for(; nCount != 0; nCount--);
}

/**
  * DMA传输配置
  */
static void DMA_Config(void)
{
    DMA_InitTypeDef  DMA_InitStructure;
    __IO uint32_t    Timeout = TIMEOUT_MAX;

    /* 使能DMA时钟 */
    RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);

    /* 复位初始化DMA数据流 */
    DMA_DeInit(DMA_STREAM);

    /* 确保DMA数据流复位完成 */
    while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
    {
    }

    /* DMA数据流通道选择 */
    DMA_InitStructure.DMA_Channel = DMA_CHANNEL;
    /* 源数据地址 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer;
    /* 目标地址 */
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aDST_Buffer;
    /* 存储器到存储器模式 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
    /* 数据数目 */
    DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
    /* 使能自动递增功能 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    /* 使能自动递增功能 */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /* 源数据是字大小(32位) */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    /* 目标数据也是字大小(32位) */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    /* 一次传输模式，存储器到存储器模式不能使用循环传输 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /* DMA数据流优先级为高 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    /* 禁用FIFO模式 */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    /* 单次模式 */
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    /* 单次模式 */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    /* 完成DMA数据流参数配置 */
    DMA_Init(DMA_STREAM, &DMA_InitStructure);

    /* 清除DMA数据流传输完成标志位 */
    DMA_ClearFlag(DMA_STREAM, DMA_FLAG_TCIF);

    /* 使能DMA数据流，开始DMA数据传输 */
    DMA_Cmd(DMA_STREAM, ENABLE);

    /* 检测DMA数据流是否有效并带有超时检测功能 */
    Timeout = TIMEOUT_MAX;
    while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (Timeout-- > 0))
    {
        printf("enable %x,Timeout %x\n", DMA_GetCmdStatus(DMA_STREAM), Timeout);
    }
    printf("timeout = %x\n", Timeout);
    printf("timeout = %d\n", Timeout);
    /* 判断是否超时 */
    if ((int32_t)Timeout <= 0)
    {
        printf("get in timeout\n");
        /* 超时就让程序运行下面循环：RGB彩色灯闪烁 */
        while (1)
        {
            LED_RED;
            Delay(0xFFFFFF);
            LED_RGBOFF;
            Delay(0xFFFFFF);
        }
    }
    printf("timeout over\n");
}

/**
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t Buffercmp(const uint32_t *pBuffer,
                  uint32_t *pBuffer1, uint16_t BufferLength)
{
    /* 数据长度递减 */
    while(BufferLength--)
    {
        /* 判断两个数据源是否对应相等 */
        if(*pBuffer != *pBuffer1)
        {
            /* 对应数据源不相等马上退出函数，并返回0 */
            return 0;
        }
        /* 递增两个数据源的地址指针 */
        pBuffer++;
        pBuffer1++;
    }
    /* 完成判断并且对应数据相对 */
    return 1;
}
/*
 * 函数名：Buffercmp1
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
 */
TestStatus Buffercmp1(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}
/**
  * @brief  I2C(AT24C02)读写测试
  * @param  无
  * @retval 正常返回1 ，不正常返回0
  */
uint8_t I2C_Test(void)
{
    u16 i;

    EEPROM_INFO("写入的数据");

    for ( i = 0; i <= 255; i++ ) //填充缓冲
    {
        I2c_Buf_Write[i] = i;

        printf("0x%02X ", I2c_Buf_Write[i]);
        if(i % 16 == 15)
            printf("\n\r");
    }

    //将I2c_Buf_Write中顺序递增的数据写入EERPOM中
    I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);

    EEPROM_INFO("写成功");

    EEPROM_INFO("读出的数据");
    //将EEPROM读出数据顺序保持到I2c_Buf_Read中
    I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);

    //将I2c_Buf_Read中的数据通过串口打印
    for (i = 0; i < 256; i++)
    {
        if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
        {
            printf("0x%02X ", I2c_Buf_Read[i]);
            EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致");
            return 0;
        }
        printf("0x%02X ", I2c_Buf_Read[i]);
        if(i % 16 == 15)
            printf("\n\r");

    }
    EEPROM_INFO("I2C(AT24C02)读写测试成功");
    return 1;
}
