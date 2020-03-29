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
#include "ff.h"
#include "string.h"
#include "./sdram/bsp_sdram.h"
#include "./malloc/malloc.h"


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

/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_flash;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024] = {0};      /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
    "欢迎使用野火STM32 F429开发板 今天是个好日子，新建文件系统测试文件\r\n";
char fpath[100];                  /* 保存当前扫描路径 */
char readbuffer[512];             /*  */


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
void FATFS_test25(void);	 // FATFS的代码
void FATFS_test25_2(void);
void SDRAM_test26(void);    // SDRAM1
uint8_t I2C_Test(void);
uint8_t Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength);
TestStatus Buffercmp1(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static FRESULT miscellaneous(void);
static FRESULT file_check(void);
static FRESULT scan_files (char *path);

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
    //SPI_FLASH_test24_1();
    //FATFS_test25();
    //FATFS_test25_2();
    SDRAM_test26();
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
void FATFS_test25(void)	 // FATFS的代码
{
    /* 初始化LED */
    LED_GPIO_Config();
    LED_BLUE;

    /* 初始化调试串口，一般为串口1 */
    Debug_USART_Config();
    printf("****** 这是一个SPI FLASH 文件系统实验 ******\r\n");

    //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_flash = f_mount(&fs, "1:", 1);

    /*----------------------- 格式化测试 ---------------------------*/
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if(res_flash == FR_NO_FILESYSTEM)
    {
        printf("》FLASH还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_flash = f_mkfs("1:", 0, 0);

        if(res_flash == FR_OK)
        {
            printf("》FLASH已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL, "1:", 1);
            /* 重新挂载	*/
            res_flash = f_mount(&fs, "1:", 1);
        }
        else
        {
            LED_RED;
            printf("《《格式化失败。》》\r\n");
            while(1);
        }
    }
    else if(res_flash != FR_OK)
    {
        printf("！！外部Flash挂载文件系统失败。(%d)\r\n", res_flash);
        printf("！！可能原因：SPI Flash初始化不成功。\r\n");
        while(1);
    }
    else
    {
        printf("》文件系统挂载成功，可以进行读写测试\r\n");
    }

    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("\r\n****** 即将进行文件写入测试... ******\r\n");
    res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE );
    if ( res_flash == FR_OK )
    {
        printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if(res_flash == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnum);
            printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", res_flash);
        }
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {
        LED_RED;
        printf("！！打开/创建文件失败。\r\n");
    }

    /*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("****** 即将进行文件读取测试... ******\r\n");
    res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);
    if(res_flash == FR_OK)
    {
        LED_GREEN;
        printf("》打开文件成功。\r\n");
        res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if(res_flash == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", res_flash);
        }
    }
    else
    {
        LED_RED;
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);

    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "1:", 1);

    /* 操作完成，停机 */
    while(1)
    {
    }
}

void FATFS_test25_2(void)
{
    /* 初始化调试串口，一般为串口1 */
    Debug_USART_Config();
    printf("******** 这是一个SPI FLASH 文件系统实验 *******\r\n");

    //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_flash = f_mount(&fs, "1:", 1);
    if(res_flash != FR_OK)
    {
        printf("！！外部Flash挂载文件系统失败。(%d)\r\n", res_flash);
        printf("！！可能原因：SPI Flash初始化不成功。\r\n");
        while(1);
    }
    else
    {
        printf("》文件系统挂载成功，可以进行测试\r\n");
    }

    /* FatFs多项功能测试 */
    res_flash = miscellaneous();


    printf("\n*************** 文件信息获取测试 **************\r\n");
    res_flash = file_check();


    printf("***************** 文件扫描测试 ****************\r\n");
    strcpy(fpath, "1:");
    scan_files(fpath);


    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "1:", 1);

    /* 操作完成，停机 */
    while(1)
    {
    }
}

void SDRAM_test26(void)	 // SDRAM1
{
    u8 *p = 0;
    u8 sramx = 1;				//默认为内部sram

    /* LED 端口初始化 */
    LED_GPIO_Config();

    /* 初始化串口 */
    Debug_USART_Config();

    printf("\r\n 野火STM32F429 SDRAM 读写测试例程\r\n");

    /*初始化SDRAM模块*/
    SDRAM_Init();

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMEX);		//初始化外部内存池


    printf ( "\r\n野火内存管理\r\n" );


    printf ( "\r\n分配内存\r\n" );

    p = mymalloc ( sramx, 1024 * 2 ); //申请2K字节

    sprintf((char *)p, "Memory Malloc"); //使用申请空间存储内容
    sprintf((char *)p, Tx_Buffer);
    printf ( "p_addr:%x\r\n", p );

    printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//显示内部内存使用率
    printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//显示外部内存使用率

    myfree(sramx, p); //释放内存
    p = 0;		      	//指向空地址

    printf ( "\r\n释放内存\r\n" );
    printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//显示内部内存使用率
    printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//显示外部内存使用率


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
TestStatus Buffercmp1(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
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
/**
  ******************************************************************************
  *                                任务函数
  ******************************************************************************
  */
/* FatFs多项功能测试 */
static FRESULT miscellaneous(void)
{
    DIR dir;
    FATFS *pfs;
    DWORD fre_clust, fre_sect, tot_sect;

    printf("\n*************** 设备信息获取 ***************\r\n");
    /* 获取设备信息和空簇大小 */
    res_flash = f_getfree("1:", &fre_clust, &pfs);

    /* 计算得到总的扇区个数和空扇区个数 */
    tot_sect = (pfs->n_fatent - 2) * pfs->csize;
    fre_sect = fre_clust * pfs->csize;

    /* 打印信息(4096 字节/扇区) */
    printf("》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n", tot_sect * 4, fre_sect * 4);

    printf("\n******** 文件定位和格式化写入功能测试 ********\r\n");
    res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",
                       FA_OPEN_ALWAYS | FA_WRITE | FA_READ );
    if ( res_flash == FR_OK )
    {
        /*  文件定位 */
        res_flash = f_lseek(&fnew, f_size(&fnew));
        if (res_flash == FR_OK)
        {
            /* 格式化写入，参数格式类似printf函数 */
            f_printf(&fnew, "\n在原来文件新添加一行内容\n");
            f_printf(&fnew, "》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n", tot_sect * 4, fre_sect * 4);
            /*  文件定位到文件起始位置 */
            res_flash = f_lseek(&fnew, 0);
            /* 读取文件所有内容到缓存区 */
            res_flash = f_read(&fnew, readbuffer, f_size(&fnew), &fnum);
            if(res_flash == FR_OK)
            {
                printf("》文件内容：\n%s\n", readbuffer);
            }
        }
        f_close(&fnew);

        printf("\n********** 目录创建和重命名功能测试 **********\r\n");
        /* 尝试打开目录 */
        res_flash = f_opendir(&dir, "1:TestDir");
        if(res_flash != FR_OK)
        {
            /* 打开目录失败，就创建目录 */
            res_flash = f_mkdir("1:TestDir");
        }
        else
        {
            /* 如果目录已经存在，关闭它 */
            res_flash = f_closedir(&dir);
            /* 删除文件 */
            f_unlink("1:TestDir/testdir.txt");
        }
        if(res_flash == FR_OK)
        {
            /* 重命名并移动文件 */
            res_flash = f_rename("1:FatFs读写测试文件.txt", "1:TestDir/testdir.txt");
        }
    }
    else
    {
        printf("!! 打开文件失败：%d\n", res_flash);
        printf("!! 或许需要再次运行“FatFs移植与读写测试”工程\n");
    }
    return res_flash;
}

FILINFO fno;
/**
  * 文件信息获取
  */
static FRESULT file_check(void)
{


    /* 获取文件信息 */
    res_flash = f_stat("1:TestDir/testdir.txt", &fno);
    if(res_flash == FR_OK)
    {
        printf("“testdir.txt”文件信息：\n");
        printf("》文件大小: %ld(字节)\n", fno.fsize);
        printf("》时间戳: %u/%02u/%02u, %02u:%02u\n",
               (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31, fno.ftime >> 11, fno.ftime >> 5 & 63);
        printf("》属性: %c%c%c%c%c\n\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',      // 是一个目录
               (fno.fattrib & AM_RDO) ? 'R' : '-',      // 只读文件
               (fno.fattrib & AM_HID) ? 'H' : '-',      // 隐藏文件
               (fno.fattrib & AM_SYS) ? 'S' : '-',      // 系统文件
               (fno.fattrib & AM_ARC) ? 'A' : '-');     // 档案文件
    }
    return res_flash;
}

/**
  * @brief  scan_files 递归扫描FatFs内的文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char *path)
{
    FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // 文件名

#if _USE_LFN
    /* 长文件名支持 */
    /* 简体中文需要2个字节保存一个“字”*/
    static char lfn[_MAX_LFN * 2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    //打开目录
    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        i = strlen(path);
        for (;;)
        {
            //读取目录下的内容，再读会自动读下一个文件
            res = f_readdir(&dir, &fno);
            //为空时表示所有项目读取完毕，跳出
            if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            //点表示当前目录，跳过
            if (*fn == '.') continue;
            //目录，递归读取
            if (fno.fattrib & AM_DIR)
            {
                //合成完整目录名
                sprintf(&path[i], "/%s", fn);
                //递归遍历
                res = scan_files(path);
                path[i] = 0;
                //打开失败，跳出循环
                if (res != FR_OK)
                    break;
            }
            else
            {
                printf("%s/%s\r\n", path, fn);								//输出文件名
                /* 可以在这里提取特定格式的文件路径 */
            }//else
        } //for
    }
    return res;
}
