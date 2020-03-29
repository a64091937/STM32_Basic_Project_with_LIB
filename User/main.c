/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhang Hongda, a64091937
  * @version V1.0
  * @date    2019-9-8
  * @brief   A Standard STM32 Project
  ******************************************************************************
  * @attention
  * ��������main����֮ǰ�������ļ���statup_stm32f429xx.s�Ѿ�����
  * SystemInit()������ϵͳʱ�ӳ�ʼ����180MHZ
  *  SystemInit()��system_stm32f4xx.c�ж���
  * ����û����޸�ϵͳʱ�ӣ������б�д�����޸�
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


/* ��غ궨�壬ʹ�ô洢�����洢���������ʹ��DMA2 */
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

/* ��ȡ�������ĳ��� */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress


/* ���ͻ�������ʼ�� */
uint8_t Tx_Buffer[] = "��л��ѡ��Ұ��stm32������\r\nhttps://fire-stm32.taobao.com";
uint8_t Rx_Buffer[BufferSize];

//��ȡ��ID�洢λ��
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

/* ����aSRC_Const_Buffer������ΪDMA��������Դ
  const�ؼ��ֽ�aSRC_Const_Buffer�����������Ϊ�������� */
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
/* ����DMA����Ŀ��洢�� */
uint32_t aDST_Buffer[BUFFER_SIZE];

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_flash;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024] = {0};      /* �������� */
BYTE WriteBuffer[] =              /* д������*/
    "��ӭʹ��Ұ��STM32 F429������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";
char fpath[100];                  /* ���浱ǰɨ��·�� */
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
void FATFS_test25(void);	 // FATFS�Ĵ���
void FATFS_test25_2(void);
void SDRAM_test26(void);    // SDRAM1
uint8_t I2C_Test(void);
uint8_t Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength);
TestStatus Buffercmp1(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static FRESULT miscellaneous(void);
static FRESULT file_check(void);
static FRESULT scan_files (char *path);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{

    // LED_test11(); // ��11�µĴ���
    // KEY_test12(); // ��12�µĴ���
    // RCC_test15(); // ��15�µĴ���
    //EXTI_test17(); // ��17�µĴ���
    // UART_test18_1(); // ��18_1�µĴ���
    //UART_test18_2(); // ��18_2�µĴ���
    //DMA_test19_1();
    //Flash_test1_1();
    //I2C_test23_1();
    //SPI_FLASH_test24_1();
    //FATFS_test25();
    //FATFS_test25_2();
    SDRAM_test26();
    return 0;

}

void LED_test11(void)	 // LED��˸�Ĵ���
{
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    for (;;)
    {
        LED1( ON ); 		 // ��
        Delay(0xFFFFFF);
        LED1( OFF );		  // ��

        LED2( ON ); 		// ��
        Delay(0xFFFFFF);
        LED2( OFF );		  // ��

        LED3( ON ); 		 // ��
        Delay(0xFFFFFF);
        LED3( OFF );		  // ��

        /*������ʾ ������������� ��ɫ*/
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
void KEY_test12(void)	 // KEY��תLED�Ĵ���
{
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    /* ��ѯ����״̬��������������תLED */
    for (;;)
    {
        if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON  )
        {
            /*LED1��ת*/
            LED1_TOGGLE;
        }

        if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON  )
        {
            /*LED2��ת*/
            LED2_TOGGLE;
        }
    }
}

void RCC_test15(void)	 // KEY��תLED�Ĵ���
{
    // ��������main����֮ǰ�������ļ���statup_stm32f429_439xx.s�Ѿ�����
    // SystemInit()������ϵͳʱ�ӳ�ʼ����72MHZ
    // SystemInit()��system_stm32f4xx.c�ж���
    // ����û����޸�ϵͳʱ�ӣ������б�д�����޸�
    // ��������ϵͳʱ�ӣ���ʱ�����ѡ��ʹ��HSE����HSI

    // ʹ��HSE������ϵͳʱ��Ϊ180M
    HSE_SetSysClock(25, 360, 2, 7);

    //ϵͳʱ�ӳ�Ƶ��216Mˬһ�£������216M������������
    //HSE_SetSysClock(25, 432, 2, 9);

    // ʹ��HSI������ϵͳʱ��Ϊ180M
    //HSI_SetSysClock(16, 360, 2, 7);

    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();

    // MCO GPIO  ��ʼ��
    MCO1_GPIO_Config();
    MCO2_GPIO_Config();

    // MCO1 ���PLLCLK
    RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);

    // MCO2 ���SYSCLK
    RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO1Div_1);

    while (1)
    {
        LED1( ON );			 // ��
        Delay(0xFFFFFF);
        LED1( OFF );		  // ��
        Delay(0xFFFFFF);
    }
}

void EXTI_test17(void)	 // KEY��תLED�Ĵ���
{
    /* ��ʼ��EXTI�жϣ����°����ᴥ���жϣ�
    *  �����жϻ����stm32f4xx_it.c�ļ��еĺ���
    *  KEY1_IRQHandler��KEY2_IRQHandler�������жϣ���תLED�ơ�
    */
    EXTI_Key_Config();
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    /* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
    while(1)
    {
    }
}
void UART_test18_1(void)	 // USART_USART1�ӷ�
{
    /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
    Debug_USART_Config();
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    /* ����һ���ַ��� */
    Usart_SendString( DEBUG_USART, "����һ�������жϽ��ջ���ʵ��\n");
    // printf("����һ�������жϽ��ջ���ʵ��\n");

    while(1)
    {

    }
}
void UART_test18_2(void)	 // USART_USART1�ʵ�
{

    char ch;

    /* ��ʼ��RGB�ʵ� */
    LED_GPIO_Config();

    /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
    LED_USART_Config();


    /* ��ӡָ��������ʾ��Ϣ */
    Show_Message();
    while(1)
    {
        /* ��ȡ�ַ�ָ�� */
        ch = getchar();
        printf("���յ��ַ���%c\n", ch);

        /* �����ַ�ָ�����RGB�ʵ���ɫ */
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
            /* �������ָ��ָ���ַ�����ӡ��ʾ��Ϣ */
            Show_Message();
            break;
        }
    }
}

void DMA_test19_1(void)	 // DMA1
{
    /* �����űȽϽ������ */
    uint8_t TransferStatus;
    //uint8_t *flash_addr = 0x0;

    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
    LED_USART_Config();
    /* ����RGB��ɫ��Ϊ��ɫ */
    LED_PURPLE;

    /* ����ʱ���� */
    Delay(0xFFFFFF);

    /* DMA�������� */
    DMA_Config();

    /* �ȴ�DMA������� */
    while(DMA_GetFlagStatus(DMA_STREAM, DMA_FLAG_TCIF) == DISABLE)
    {

    }

    /* �Ƚ�Դ�����봫������� */
    TransferStatus = Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);

    /* �ж�Դ�����봫������ݱȽϽ��*/
    if(TransferStatus == 0)
    {
        /* Դ�����봫������ݲ����ʱRGB��ɫ����ʾ��ɫ */
        LED_RED;
    }
    else
    {
        /* Դ�����봫����������ʱRGB��ɫ����ʾ��ɫ */
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
    /*��ʼ��USART1*/
    Debug_USART_Config();

    printf("\r\n ��ӭʹ��Ұ��  STM32 F429 �����塣\r\n");

    printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");

    /* I2C �����(AT24C02)ʼ�� */
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

    /* ���ô���1Ϊ��115200 8-N-1 */
    Debug_USART_Config();

    printf("\r\n����һ��16M����flash(W25Q128)ʵ�� \r\n");

    /* 16M����flash W25Q128��ʼ�� */
    SPI_FLASH_Init();

    /* ��ȡ Flash Device ID */
    DeviceID = SPI_FLASH_ReadDeviceID();

    Delay( 200 );

    /* ��ȡ SPI Flash ID */
    FlashID = SPI_FLASH_ReadID();

    printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

    /* ���� SPI Flash ID */
    if (FlashID == sFLASH_ID)
    {
        printf("\r\n��⵽SPI FLASH W25Q128 !\r\n");

        /* ������Ҫд��� SPI FLASH ������FLASHд��ǰҪ�Ȳ��� */
        SPI_FLASH_SectorErase(FLASH_SectorToErase);

        /* �����ͻ�����������д��flash�� */
        SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
        printf("\r\nд�������Ϊ��\r\n%s", Tx_Buffer);

        /* ���ո�д������ݶ������ŵ����ջ������� */
        SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
        printf("\r\n����������Ϊ��\r\n%s", Rx_Buffer);

        /* ���д�������������������Ƿ���� */
        TransferStatus1 = Buffercmp1(Tx_Buffer, Rx_Buffer, BufferSize);

        if( PASSED == TransferStatus1 )
        {
            LED_GREEN;
            printf("\r\n16M����flash(W25Q128)���Գɹ�!\n\r");
        }
        else
        {
            LED_RED;
            printf("\r\n16M����flash(W25Q128)����ʧ��!\n\r");
        }
    }// if (FlashID == sFLASH_ID)
    else
    {
        LED_RED;
        printf("\r\n��ȡ���� W25Q128 ID!\n\r");
    }

    SPI_Flash_PowerDown();
    while(1);
}
void FATFS_test25(void)	 // FATFS�Ĵ���
{
    /* ��ʼ��LED */
    LED_GPIO_Config();
    LED_BLUE;

    /* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
    Debug_USART_Config();
    printf("****** ����һ��SPI FLASH �ļ�ϵͳʵ�� ******\r\n");

    //���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
    res_flash = f_mount(&fs, "1:", 1);

    /*----------------------- ��ʽ������ ---------------------------*/
    /* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
    if(res_flash == FR_NO_FILESYSTEM)
    {
        printf("��FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
        /* ��ʽ�� */
        res_flash = f_mkfs("1:", 0, 0);

        if(res_flash == FR_OK)
        {
            printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
            /* ��ʽ������ȡ������ */
            res_flash = f_mount(NULL, "1:", 1);
            /* ���¹���	*/
            res_flash = f_mount(&fs, "1:", 1);
        }
        else
        {
            LED_RED;
            printf("������ʽ��ʧ�ܡ�����\r\n");
            while(1);
        }
    }
    else if(res_flash != FR_OK)
    {
        printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n", res_flash);
        printf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
        while(1);
    }
    else
    {
        printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
    }

    /*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
    /* ���ļ�������ļ��������򴴽��� */
    printf("\r\n****** ���������ļ�д�����... ******\r\n");
    res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt", FA_CREATE_ALWAYS | FA_WRITE );
    if ( res_flash == FR_OK )
    {
        printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
        /* ��ָ���洢������д�뵽�ļ��� */
        res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if(res_flash == FR_OK)
        {
            printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n", fnum);
            printf("�����ļ�д�������Ϊ��\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("�����ļ�д��ʧ�ܣ�(%d)\n", res_flash);
        }
        /* ���ٶ�д���ر��ļ� */
        f_close(&fnew);
    }
    else
    {
        LED_RED;
        printf("������/�����ļ�ʧ�ܡ�\r\n");
    }

    /*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
    printf("****** ���������ļ���ȡ����... ******\r\n");
    res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ);
    if(res_flash == FR_OK)
    {
        LED_GREEN;
        printf("�����ļ��ɹ���\r\n");
        res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if(res_flash == FR_OK)
        {
            printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n", fnum);
            printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("�����ļ���ȡʧ�ܣ�(%d)\n", res_flash);
        }
    }
    else
    {
        LED_RED;
        printf("�������ļ�ʧ�ܡ�\r\n");
    }
    /* ���ٶ�д���ر��ļ� */
    f_close(&fnew);

    /* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
    f_mount(NULL, "1:", 1);

    /* ������ɣ�ͣ�� */
    while(1)
    {
    }
}

void FATFS_test25_2(void)
{
    /* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
    Debug_USART_Config();
    printf("******** ����һ��SPI FLASH �ļ�ϵͳʵ�� *******\r\n");

    //���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
    res_flash = f_mount(&fs, "1:", 1);
    if(res_flash != FR_OK)
    {
        printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n", res_flash);
        printf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
        while(1);
    }
    else
    {
        printf("���ļ�ϵͳ���سɹ������Խ��в���\r\n");
    }

    /* FatFs����ܲ��� */
    res_flash = miscellaneous();


    printf("\n*************** �ļ���Ϣ��ȡ���� **************\r\n");
    res_flash = file_check();


    printf("***************** �ļ�ɨ����� ****************\r\n");
    strcpy(fpath, "1:");
    scan_files(fpath);


    /* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
    f_mount(NULL, "1:", 1);

    /* ������ɣ�ͣ�� */
    while(1)
    {
    }
}

void SDRAM_test26(void)	 // SDRAM1
{
    u8 *p = 0;
    u8 sramx = 1;				//Ĭ��Ϊ�ڲ�sram

    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();

    /* ��ʼ������ */
    Debug_USART_Config();

    printf("\r\n Ұ��STM32F429 SDRAM ��д��������\r\n");

    /*��ʼ��SDRAMģ��*/
    SDRAM_Init();

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��


    printf ( "\r\nҰ���ڴ����\r\n" );


    printf ( "\r\n�����ڴ�\r\n" );

    p = mymalloc ( sramx, 1024 * 2 ); //����2K�ֽ�

    sprintf((char *)p, "Memory Malloc"); //ʹ������ռ�洢����
    sprintf((char *)p, Tx_Buffer);
    printf ( "p_addr:%x\r\n", p );

    printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//��ʾ�ڲ��ڴ�ʹ����
    printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//��ʾ�ⲿ�ڴ�ʹ����

    myfree(sramx, p); //�ͷ��ڴ�
    p = 0;		      	//ָ��յ�ַ

    printf ( "\r\n�ͷ��ڴ�\r\n" );
    printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//��ʾ�ڲ��ڴ�ʹ����
    printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//��ʾ�ⲿ�ڴ�ʹ����


    while(1);
}


//////////////////////////////������֧�ֺ���////////////////////////////
void Flash_test1_1(void)	 // Flash1
{
    /* �����űȽϽ������ */
    uint32_t num = 0;
    uint32_t *flash_addr = (uint32_t *)0x08000000;

    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
    LED_USART_Config();
    /* ����RGB��ɫ��Ϊ��ɫ */
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
    printf("\r\n   ����һ��ͨ������ͨ��ָ�����RGB�ʵ�ʵ�� \n");
    printf("ʹ��  USART1  ����Ϊ��%d 8-N-1 \n", DEBUG_USART_BAUDRATE);
    printf("������ӵ�ָ������RGB�ʵ���ɫ��ָ���Ӧ���£�\n");
    printf("   ָ��   ------ �ʵ���ɫ \n");
    printf("     1    ------    �� \n");
    printf("     2    ------    �� \n");
    printf("     3    ------    �� \n");
    printf("     4    ------    �� \n");
    printf("     5    ------    �� \n");
    printf("     6    ------    �� \n");
    printf("     7    ------    �� \n");
    printf("     8    ------    �� \n");
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
    for(; nCount != 0; nCount--);
}

/**
  * DMA��������
  */
static void DMA_Config(void)
{
    DMA_InitTypeDef  DMA_InitStructure;
    __IO uint32_t    Timeout = TIMEOUT_MAX;

    /* ʹ��DMAʱ�� */
    RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);

    /* ��λ��ʼ��DMA������ */
    DMA_DeInit(DMA_STREAM);

    /* ȷ��DMA��������λ��� */
    while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
    {
    }

    /* DMA������ͨ��ѡ�� */
    DMA_InitStructure.DMA_Channel = DMA_CHANNEL;
    /* Դ���ݵ�ַ */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer;
    /* Ŀ���ַ */
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aDST_Buffer;
    /* �洢�����洢��ģʽ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
    /* ������Ŀ */
    DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
    /* ʹ���Զ��������� */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    /* ʹ���Զ��������� */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /* Դ�������ִ�С(32λ) */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    /* Ŀ������Ҳ���ִ�С(32λ) */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    /* һ�δ���ģʽ���洢�����洢��ģʽ����ʹ��ѭ������ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /* DMA���������ȼ�Ϊ�� */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    /* ����FIFOģʽ */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    /* ����ģʽ */
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    /* ����ģʽ */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    /* ���DMA�������������� */
    DMA_Init(DMA_STREAM, &DMA_InitStructure);

    /* ���DMA������������ɱ�־λ */
    DMA_ClearFlag(DMA_STREAM, DMA_FLAG_TCIF);

    /* ʹ��DMA����������ʼDMA���ݴ��� */
    DMA_Cmd(DMA_STREAM, ENABLE);

    /* ���DMA�������Ƿ���Ч�����г�ʱ��⹦�� */
    Timeout = TIMEOUT_MAX;
    while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (Timeout-- > 0))
    {
        printf("enable %x,Timeout %x\n", DMA_GetCmdStatus(DMA_STREAM), Timeout);
    }
    printf("timeout = %x\n", Timeout);
    printf("timeout = %d\n", Timeout);
    /* �ж��Ƿ�ʱ */
    if ((int32_t)Timeout <= 0)
    {
        printf("get in timeout\n");
        /* ��ʱ���ó�����������ѭ����RGB��ɫ����˸ */
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
  * �ж�ָ�����ȵ���������Դ�Ƿ���ȫ��ȣ�
  * �����ȫ��ȷ���1��ֻҪ����һ�����ݲ���ȷ���0
  */
uint8_t Buffercmp(const uint32_t *pBuffer,
                  uint32_t *pBuffer1, uint16_t BufferLength)
{
    /* ���ݳ��ȵݼ� */
    while(BufferLength--)
    {
        /* �ж���������Դ�Ƿ��Ӧ��� */
        if(*pBuffer != *pBuffer1)
        {
            /* ��Ӧ����Դ����������˳�������������0 */
            return 0;
        }
        /* ������������Դ�ĵ�ַָ�� */
        pBuffer++;
        pBuffer1++;
    }
    /* ����жϲ��Ҷ�Ӧ������� */
    return 1;
}
/*
 * ��������Buffercmp1
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-PASSED pBuffer1 ����   pBuffer2
 *         -FAILED pBuffer1 ��ͬ�� pBuffer2
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
  * @brief  I2C(AT24C02)��д����
  * @param  ��
  * @retval ��������1 ������������0
  */
uint8_t I2C_Test(void)
{
    u16 i;

    EEPROM_INFO("д�������");

    for ( i = 0; i <= 255; i++ ) //��仺��
    {
        I2c_Buf_Write[i] = i;

        printf("0x%02X ", I2c_Buf_Write[i]);
        if(i % 16 == 15)
            printf("\n\r");
    }

    //��I2c_Buf_Write��˳�����������д��EERPOM��
    I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);

    EEPROM_INFO("д�ɹ�");

    EEPROM_INFO("����������");
    //��EEPROM��������˳�򱣳ֵ�I2c_Buf_Read��
    I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);

    //��I2c_Buf_Read�е�����ͨ�����ڴ�ӡ
    for (i = 0; i < 256; i++)
    {
        if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
        {
            printf("0x%02X ", I2c_Buf_Read[i]);
            EEPROM_ERROR("����:I2C EEPROMд������������ݲ�һ��");
            return 0;
        }
        printf("0x%02X ", I2c_Buf_Read[i]);
        if(i % 16 == 15)
            printf("\n\r");

    }
    EEPROM_INFO("I2C(AT24C02)��д���Գɹ�");
    return 1;
}
/**
  ******************************************************************************
  *                                ������
  ******************************************************************************
  */
/* FatFs����ܲ��� */
static FRESULT miscellaneous(void)
{
    DIR dir;
    FATFS *pfs;
    DWORD fre_clust, fre_sect, tot_sect;

    printf("\n*************** �豸��Ϣ��ȡ ***************\r\n");
    /* ��ȡ�豸��Ϣ�Ϳմش�С */
    res_flash = f_getfree("1:", &fre_clust, &pfs);

    /* ����õ��ܵ����������Ϳ��������� */
    tot_sect = (pfs->n_fatent - 2) * pfs->csize;
    fre_sect = fre_clust * pfs->csize;

    /* ��ӡ��Ϣ(4096 �ֽ�/����) */
    printf("���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺  %10lu KB��\n", tot_sect * 4, fre_sect * 4);

    printf("\n******** �ļ���λ�͸�ʽ��д�빦�ܲ��� ********\r\n");
    res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",
                       FA_OPEN_ALWAYS | FA_WRITE | FA_READ );
    if ( res_flash == FR_OK )
    {
        /*  �ļ���λ */
        res_flash = f_lseek(&fnew, f_size(&fnew));
        if (res_flash == FR_OK)
        {
            /* ��ʽ��д�룬������ʽ����printf���� */
            f_printf(&fnew, "\n��ԭ���ļ������һ������\n");
            f_printf(&fnew, "���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺  %10lu KB��\n", tot_sect * 4, fre_sect * 4);
            /*  �ļ���λ���ļ���ʼλ�� */
            res_flash = f_lseek(&fnew, 0);
            /* ��ȡ�ļ��������ݵ������� */
            res_flash = f_read(&fnew, readbuffer, f_size(&fnew), &fnum);
            if(res_flash == FR_OK)
            {
                printf("���ļ����ݣ�\n%s\n", readbuffer);
            }
        }
        f_close(&fnew);

        printf("\n********** Ŀ¼���������������ܲ��� **********\r\n");
        /* ���Դ�Ŀ¼ */
        res_flash = f_opendir(&dir, "1:TestDir");
        if(res_flash != FR_OK)
        {
            /* ��Ŀ¼ʧ�ܣ��ʹ���Ŀ¼ */
            res_flash = f_mkdir("1:TestDir");
        }
        else
        {
            /* ���Ŀ¼�Ѿ����ڣ��ر��� */
            res_flash = f_closedir(&dir);
            /* ɾ���ļ� */
            f_unlink("1:TestDir/testdir.txt");
        }
        if(res_flash == FR_OK)
        {
            /* ���������ƶ��ļ� */
            res_flash = f_rename("1:FatFs��д�����ļ�.txt", "1:TestDir/testdir.txt");
        }
    }
    else
    {
        printf("!! ���ļ�ʧ�ܣ�%d\n", res_flash);
        printf("!! ������Ҫ�ٴ����С�FatFs��ֲ���д���ԡ�����\n");
    }
    return res_flash;
}

FILINFO fno;
/**
  * �ļ���Ϣ��ȡ
  */
static FRESULT file_check(void)
{


    /* ��ȡ�ļ���Ϣ */
    res_flash = f_stat("1:TestDir/testdir.txt", &fno);
    if(res_flash == FR_OK)
    {
        printf("��testdir.txt���ļ���Ϣ��\n");
        printf("���ļ���С: %ld(�ֽ�)\n", fno.fsize);
        printf("��ʱ���: %u/%02u/%02u, %02u:%02u\n",
               (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31, fno.ftime >> 11, fno.ftime >> 5 & 63);
        printf("������: %c%c%c%c%c\n\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',      // ��һ��Ŀ¼
               (fno.fattrib & AM_RDO) ? 'R' : '-',      // ֻ���ļ�
               (fno.fattrib & AM_HID) ? 'H' : '-',      // �����ļ�
               (fno.fattrib & AM_SYS) ? 'S' : '-',      // ϵͳ�ļ�
               (fno.fattrib & AM_ARC) ? 'A' : '-');     // �����ļ�
    }
    return res_flash;
}

/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char *path)
{
    FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // �ļ���

#if _USE_LFN
    /* ���ļ���֧�� */
    /* ����������Ҫ2���ֽڱ���һ�����֡�*/
    static char lfn[_MAX_LFN * 2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    //��Ŀ¼
    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        i = strlen(path);
        for (;;)
        {
            //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
            res = f_readdir(&dir, &fno);
            //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
            if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            //���ʾ��ǰĿ¼������
            if (*fn == '.') continue;
            //Ŀ¼���ݹ��ȡ
            if (fno.fattrib & AM_DIR)
            {
                //�ϳ�����Ŀ¼��
                sprintf(&path[i], "/%s", fn);
                //�ݹ����
                res = scan_files(path);
                path[i] = 0;
                //��ʧ�ܣ�����ѭ��
                if (res != FR_OK)
                    break;
            }
            else
            {
                printf("%s/%s\r\n", path, fn);								//����ļ���
                /* ������������ȡ�ض���ʽ���ļ�·�� */
            }//else
        } //for
    }
    return res;
}
