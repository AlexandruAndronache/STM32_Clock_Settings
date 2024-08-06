/*
 * main.c
 *
 *  Created on: 06-Aug-2024
 *      Author: alexa
 */

/*---------My modified template-----------*/


#include<string.h>
#include<stdio.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

#define TRUE 1
#define FALSE 0


void UART2_Init(void);
void Error_handler(void);
void menu(void);


UART_HandleTypeDef huart2;
uint8_t data_buffer[100];
uint8_t recvd_data;
uint32_t count=0;
uint8_t reception_complete = FALSE;



int main(void)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	char msg[100];

	HAL_Init();

	UART2_Init();

	memset(&osc_init,0,sizeof(osc_init));
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_init.HSEState = RCC_HSE_BYPASS;
    if ( HAL_RCC_OscConfig(&osc_init) != HAL_OK)
    {
    	Error_handler();
    }

    clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
    					RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
    clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
    clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    if( HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK)
    {
    	Error_handler();
    }

 /*---------------------------- AFTER THIS LINE SYSCLK is SOURCED BY HSE------------------*/

    __HAL_RCC_HSI_DISABLE(); //Saves some current

    /* LETS REDO THE SYSTICK CONFIGURATION */

     HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

     HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

     UART2_Init();

//	memset(msg,0,sizeof(msg));
//	sprintf(msg,"SYSCLK : %ldHz\r\n",HAL_RCC_GetSysClockFreq());
//	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
//
//	memset(msg,0,sizeof(msg));
//	sprintf(msg,"HCLK   : %ldHz\r\n",HAL_RCC_GetHCLKFreq());
//	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
//
//	memset(msg,0,sizeof(msg));
//	sprintf(msg,"PCLK1  : %ldHz\r\n",HAL_RCC_GetPCLK1Freq());
//	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
//
//	memset(msg,0,sizeof(msg));
//	sprintf(msg,"PCLK2  : %ldHz\r\n",HAL_RCC_GetPCLK2Freq());
//	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);

	menu();
    while(reception_complete != TRUE)
    {
    	HAL_UART_Receive_IT(&huart2,&recvd_data,1);
    }

	while(1);


	return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	 if(recvd_data == '\r')
	 {
		 reception_complete = TRUE;
		 data_buffer[count++]='\r';
		 HAL_UART_Transmit(huart,data_buffer,count,HAL_MAX_DELAY);
	 }
	 else
	 {
		 data_buffer[count++] = recvd_data;
		 if(data_buffer[count] == '1')
		 {
			 char pressed1[20] = "You pressed 1";
			 HAL_UART_Transmit(huart,pressed1,count,HAL_MAX_DELAY);

		 }
	 }


}

void menu(void)
{
	char buffer[100];

	memset(buffer, 0, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';
	sprintf(buffer, "Welcome to select SYSCLK frequency\r\n\r\n"
			"1. Press 1 for 4 MHz\r\n"
			"2. Press 2 for 8 MHz\r\n");

	HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

}


void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if ( HAL_UART_Init(&huart2) != HAL_OK )
	{
		//There is a problem
		Error_handler();
	}


}


void Error_handler(void)
{
	while(1);
}

