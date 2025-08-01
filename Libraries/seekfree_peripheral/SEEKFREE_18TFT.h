/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		TFT
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 * @note		
					���߶��壺
					------------------------------------ 
					ģ��ܽ�            ��Ƭ���ܽ�
					SCL                 �鿴SEEKFREE_18FTF.h�ļ��е� TFT_SCL  �궨�������     Ӳ��SPI���Ų��������л�
					SDA                 �鿴SEEKFREE_18FTF.h�ļ��е� TFT_SDA  �궨�������     Ӳ��SPI���Ų��������л�
					RES                 �鿴SEEKFREE_18FTF.h�ļ��е� REST_PIN �궨�������    
					DC                  �鿴SEEKFREE_18FTF.h�ļ��е� DC_PIN   �궨�������  
					CS                  �鿴SEEKFREE_18FTF.h�ļ��е� TFT_CS   �궨�������      Ӳ��SPI���Ų��������л�
					
					��Դ����
					BL  3.3V��Դ������������ţ�Ҳ���Խ�PWM���������ȣ�
					VCC 3.3V��Դ
					GND ��Դ��
					���ֱ���128*160
					------------------------------------ 

 ********************************************************************************************************************/



#ifndef _SEEKFREE_18TFT_h
#define _SEEKFREE_18TFT_h
#include "common.h"
#include "board.h"



//--------------------Ӳ��SPI--------------------


#define TFT_SPIN 		SPI_CH2			//����ʹ�õ�SPI��
#define	TFT_SCL_PIN 	SPI_CH2_SCLK_P25	//����SPI_SCK����
#define	TFT_SDA_PIN		SPI_CH2_MOSI_P23   //����SPI_MOSI����
#define	TFT_SDA_IN_PIN	SPI_NULL_PIN   //����SPI_MISO����  TFT��Ļû��MISO���ţ�����������Ȼ��Ҫ���壬��spi�ĳ�ʼ��ʱ��Ҫʹ��
#define TFT_REST_PIN  	P20
#define TFT_DC_PIN	  	P21				//Һ������λ���Ŷ���
#define TFT_CS_PIN	  	P22				//����SPI_CS����
#define TFT_BL_PIN	  	P27				//Һ���������Ŷ���  


#define TFT_REST(x) (TFT_REST_PIN= x)	
#define TFT_DC(x)   (TFT_DC_PIN	 = x)
#define TFT_CS(x) 	(TFT_CS_PIN	 = x)
#define TFT_BL(x) 	(TFT_BL_PIN	 = x)

//-----------------------------------------------    



//����д�ֱʵ���ɫ
#define TFT_PENCOLOR    RED

//���屳����ɫ
#define TFT_BGCOLOR	    WHITE

     
     
//������ʾ����
//0 ����ģʽ
//1 ����ģʽ  ��ת180
//2 ����ģʽ
//3 ����ģʽ  ��ת180
#define TFT_DISPLAY_DIR 3

#if (0==TFT_DISPLAY_DIR || 1==TFT_DISPLAY_DIR)
	#define	TFT_X_MAX	128	//Һ��X�����
	#define TFT_Y_MAX	160 //Һ��Y�����
#elif (2==TFT_DISPLAY_DIR || 3==TFT_DISPLAY_DIR)
	#define	TFT_X_MAX	160	//Һ��X�����
	#define TFT_Y_MAX	128 //Һ��Y�����
#else
#error "TFT_DISPLAY_DIR �������"
     
#endif


void lcd_init(void);
void lcd_clear(int color);
void lcd_drawpoint(uint16 x,uint16 y,uint16 color);
void lcd_showchar(uint16 x,uint16 y,const int8 dat);
void lcd_showstr(uint16 x,uint16 y,const int8 dat[]);
void lcd_showint8(uint16 x,uint16 y,int8 dat);
void lcd_showuint8(uint16 x,uint16 y,uint8 dat);
void lcd_showint16(uint16 x,uint16 y,int16 dat);
void lcd_showuint16(uint16 x,uint16 y,uint16 dat);
void lcd_showint32(uint16 x,uint16 y,int32 dat,uint8 num);
void lcd_showfloat(uint16 x,uint16 y,double dat,uint8 num,uint8 pointnum);
void showimage(const unsigned char *p);


#endif
