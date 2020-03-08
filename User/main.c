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
void Delay(__IO u32 nCount);
void LED_test11(void);
void KEY_test12(void);
void RCC_test15(void);
void EXTI_test17(void);
void UART_test18_1(void);
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
    /* LED �˿ڳ�ʼ�� */
    LED_GPIO_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    // LED_test11(); // ��11�µĴ��� 
    // KEY_test12(); // ��12�µĴ��� 
    // RCC_test15(); // ��15�µĴ��� 
    //EXTI_test17(); // ��17�µĴ��� 
		UART_test18_1(); // ��18_1�µĴ��� 
    return 0;

}

void LED_test11(void)	 // LED��˸�Ĵ���
{
    for (;;) {
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
    /* ��ѯ����״̬��������������תLED */ 
    for (;;) {	   
        if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  ) {
			/*LED1��ת*/
			LED1_TOGGLE;
		}   
    
        if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  ) {
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
	
	// LED �˿ڳ�ʼ�� 
	LED_GPIO_Config();
	
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
	
	/* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
	while(1)                            
	{
	}
}
void UART_test18_1(void)	 // USART_USART1�ӷ�
{
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
    Debug_USART_Config();
	
	/* ����һ���ַ��� */
	Usart_SendString( DEBUG_USART,"����һ�������жϽ��ջ���ʵ��\n");
	// printf("����һ�������жϽ��ջ���ʵ��\n");
	
    while(1)
	{	
		
	}
}
void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


