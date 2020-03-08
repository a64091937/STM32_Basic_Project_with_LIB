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
void Delay(__IO u32 nCount);
void LED_test11(void);
void KEY_test12(void);
void RCC_test15(void);
void EXTI_test17(void);
void UART_test18_1(void);
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* LED 端口初始化 */
    LED_GPIO_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    // LED_test11(); // 第11章的代码 
    // KEY_test12(); // 第12章的代码 
    // RCC_test15(); // 第15章的代码 
    //EXTI_test17(); // 第17章的代码 
		UART_test18_1(); // 第18_1章的代码 
    return 0;

}

void LED_test11(void)	 // LED闪烁的代码
{
    for (;;) {
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
    /* 轮询按键状态，若按键按下则反转LED */ 
    for (;;) {	   
        if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  ) {
			/*LED1反转*/
			LED1_TOGGLE;
		}   
    
        if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  ) {
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
	
	// LED 端口初始化 
	LED_GPIO_Config();
	
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
	
	/* 等待中断，由于使用中断方式，CPU不用轮询按键 */
	while(1)                            
	{
	}
}
void UART_test18_1(void)	 // USART_USART1接发
{
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    Debug_USART_Config();
	
	/* 发送一个字符串 */
	Usart_SendString( DEBUG_USART,"这是一个串口中断接收回显实验\n");
	// printf("这是一个串口中断接收回显实验\n");
	
    while(1)
	{	
		
	}
}
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


