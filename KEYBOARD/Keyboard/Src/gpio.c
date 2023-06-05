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
uint8_t txbuffer[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //��
 int KeyBoard_Value[5][10] = {
	// esc 	f1   f2   f3   f10       f11 f12   ;    Back   
	{  0x29,0x3A,0x3B,0x3C,0X43,    0x44,0x45, 0x33,0x2A},
	// 	Q	  W			E  R    T					Y    U     I    O    
	{ 0X14,0X1A,0X08,0X15,0X17,     0X1C,0X18,0X0C,0X12 },
	// A    S     D  F     G         H    J   K    L    
	{  0X04,0X16,0X07,0X09,0X0A,    0X0B,0X0D,0X0E,0X0F},
	// Z    X   C    V     B         N    M    ��     2
	{0x1D,0x1B,0x06,0x19,0x05,      0x11,0x10,0x52, 0x1f,   }, 
	//ctrl shift                           ��    ��   ��      
	{0x01, 0x02,0x06,0x19,0x05,      0x11,0x50,0x51,0x4F   },
	 
 
 };
 //�ް������£��ͷ����԰���
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

 //����д������ֵ�������
//��Ҳ��һ��ȥ���㷨��Ҳ���ǵ����������и�ֵ�ˣ��Ͳ�д��
 void Key_GetValue(int key) 
{
	  
		int flag;
				//���keybuf�����Ѿ���key�Ͳ�д���ֵ
						for(int j=2;j<8;j++) 
						{
								if(keybuf[j] == key)
								{
										flag =1;
										break;
								}
						}
		 
				//û�м�ֵ����д��
				if(flag != 1) 
				{
					  for( int i =2 ;i<8;i++) 
					  {
						
								//����ֵд�뺯���ڲ�
								if(keybuf[i] == 0x00) 
								{
										keybuf[i] = key;	
										flag =0;
										break;
								}
						}
				} 
				
			
		
 
 }

 //�����ɿ�ʱ��ռ�ֵ����ĺ���
 void Key_Release(int key) {
 
	 for(int i =2;i<8;i++) {
	     if(keybuf[i] == key) {
				keybuf[i] =0x00;
				break;
			 }
	 }
 
 }
//���̺���
void KeyBoard_Scan(void) {
	GPIOA->ODR = ~(0X0000001F);
		//������ʱ�������Խ����ƽ����û��ɣ��Ͷ�ȡ���������ݲ�׼ȷ����������
		HAL_Delay(1);
		if ((~GPIOB->IDR) & 0x00000ff8)  //��⵽��������
		{
			
			for (int i = 0; i < 5; i++) {
				GPIOA->ODR = ~(0x00000001 << i);
				HAL_Delay(1);
				//ɨ����
				for(int j=0;j<9;j++) {
					//�������еļ���������
				
					if((~GPIOB->IDR) & (0x00000008 << j)) {
							//������д������͵�������ȥ
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
			//	printf("�ް�������\n");
			  Key_RelseaseAll();
			  USBD_HID_SendReport(&hUsbDeviceFS,txbuffer,8);
		}
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
