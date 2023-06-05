/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include <stdio.h>
#include "usb_device.h"
#include "usbd_hid.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t  keybuf[8]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t txbuffer[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //无
 int KeyBoard_Value[5][10] = {
	// esc 	f1   f2   f3   f10       f11 f12   ;    Back   
	{  0x29,0x3A,0x3B,0x3C,0X43,    0x44,0x45, 0x33,0x2A},
	// 	Q	  W			E  R    T					Y    U     I    O    
	{ 0X14,0X1A,0X08,0X15,0X17,     0X1C,0X18,0X0C,0X12 },
	// A    S     D  F     G         H    J   K    L    
	{  0X04,0X16,0X07,0X09,0X0A,    0X0B,0X0D,0X0E,0X0F},
	// Z    X   C    V     B         N    M    上     2
	{0x1D,0x1B,0x06,0x19,0x05,      0x11,0x10,0x52, 0x1f,   }, 
	//ctrl shift                           左    下   右      
	{0x01, 0x02,0x06,0x19,0x05,      0x11,0x50,0x51,0x4F   },
	 
 
 };
 //无按键按下，释放所以按键
 void Key_RelseaseAll(void) {
 
	 for(int i=0;i<8;i++) {
		 keybuf[i] =0x00;
	 }
		 
 
 }
 
/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct ;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4 
                           PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB12 PB13 
                           PB5 PB6 PB7 PB8 
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

 //连续写入多个键值进入键盘
//这也是一种去重算法，也就是当数据里面有该值了，就不写入
 void Key_GetValue(int key) 
{
	  
		int flag;
				//如果keybuf里面已经有key就不写入该值
						for(int j=2;j<8;j++) 
						{
								if(keybuf[j] == key)
								{
										flag =1;
										break;
								}
						}
		 
				//没有键值可以写入
				if(flag != 1) 
				{
					  for( int i =2 ;i<8;i++) 
					  {
						
								//将键值写入函数内部
								if(keybuf[i] == 0x00) 
								{
										keybuf[i] = key;	
										flag =0;
										break;
								}
						}
				} 
				
			
		
 
 }

 //按键松开时情空键值里面的函数
 void Key_Release(int key) {
 
	 for(int i =2;i<8;i++) {
	     if(keybuf[i] == key) {
				keybuf[i] =0x00;
				break;
			 }
	 }
 
 }
//键盘函数
void KeyBoard_Scan(void) {
	GPIOA->ODR = ~(0X0000001F);
		//加上延时函数可以解决电平跳变没完成，就读取，容易数据不准确，纠正数据
		HAL_Delay(1);
		if ((~GPIOB->IDR) & 0x00000ff8)  //检测到按键按下
		{
			
			for (int i = 0; i < 5; i++) {
				GPIOA->ODR = ~(0x00000001 << i);
				HAL_Delay(1);
				//扫描列
				for(int j=0;j<9;j++) {
					//那行哪列的键被按下了
				
					if((~GPIOB->IDR) & (0x00000008 << j)) {
							//将按键写入带发送的数组中去
						  int key = KeyBoard_Value[i][j];
							Key_GetValue(key);

					}else  {
							Key_Release(KeyBoard_Value[i][j]);
					}
				}
			}
			USBD_HID_SendReport(&hUsbDeviceFS,keybuf,8);
		}
		else {
			//	printf("无按键按下\n");
			  Key_RelseaseAll();
			  USBD_HID_SendReport(&hUsbDeviceFS,txbuffer,8);
		}
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
