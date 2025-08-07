#include "headfile.h"
#include "AT24C.h"

//  flash��ʼ��
//	gpio_init(IO_P24, GPO, 1, GPO_PUSH_PULL);
//	gpio_init(IO_P51, GPO, 1, GPO_PUSH_PULL);

/**************************************************************************AT24C��IICͨ��**************************************************************************/
void AT24C_IIC_Delay(void)
{
    volatile uint32 i ;
    for ( i = 0; i < AT24C_IIC_DELAY; ++i);
}

void AT24C_IIC_Start(void)
{
    AT24C_SDA = 1;
    AT24C_IIC_Delay();
    AT24C_SCL = 1;
    AT24C_IIC_Delay();
    AT24C_SDA = 0;
    AT24C_IIC_Delay();
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
}

void AT24C_IIC_Stop(void)
{
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
    AT24C_SDA = 0;
    AT24C_IIC_Delay();
    AT24C_SCL = 1;
    AT24C_IIC_Delay();
    AT24C_SDA = 1;
    AT24C_IIC_Delay();
}

bit AT24C_IIC_Slave_Ack(void)
{
    uint8 ucErrTime=0;
    
    AT24C_SCL = 1;
    AT24C_IIC_Delay();
    while (AT24C_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            AT24C_IIC_Stop();
            return 1;
        }
    }
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
    return 0;
}

void AT24C_IIC_No_Ack(void)
{
    AT24C_SDA = 1;
    AT24C_IIC_Delay();
    AT24C_IIC_Delay();
    AT24C_SCL = 1;
    AT24C_IIC_Delay();
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
}

void AT24C_IIC_Master_Ack(void)
{
    AT24C_SDA = 0;
    AT24C_IIC_Delay();
    AT24C_IIC_Delay();
    AT24C_SCL = 1;
    AT24C_IIC_Delay();
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
    AT24C_SDA = 1;
    AT24C_IIC_Delay();
}

void AT24C_IIC_Send_Byte(uint8 dat)
{
    uint8 i = 8;

    while (i--)
    {
        AT24C_SCL = 0;
        AT24C_IIC_Delay();
        AT24C_SDA = dat & 0x80;
        dat <<= 1;
        AT24C_IIC_Delay();
        AT24C_SCL = 1;
        AT24C_IIC_Delay();
    }
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
}

uint8 AT24C_IIC_Receive_Byte(void)
{
    uint8 i = 8, dat = 0;

    while(i--)
    {
        dat <<= 1;
        AT24C_SCL = 0;
        AT24C_IIC_Delay();
        AT24C_SCL = 1;
        AT24C_IIC_Delay();
        dat |= AT24C_SDA;
    }
    AT24C_SCL = 0;
    AT24C_IIC_Delay();
    
    return dat;
}

/**************************************************************************AT24C**************************************************************************/
/**
  * @brief  дһ������
  * @param  Address: 0x00~0xff������ʼ��ַ
  * @param  dat: д�������
  * @retval ��
  */
void AT24C_Write_Byte(uint16 Address, uint8 dat)
{
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address >> 8));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte(dat);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Stop();
}

/**
  * @brief  ��һ������
  * @param  Address: 0x00~0xff������ʼ��ַ
  * @retval dat: ����������
  */
uint8 AT24C_Read_Byte(uint16 Address)
{
    uint8 dat;
    
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address >> 8));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS | 0x01);
    AT24C_IIC_Slave_Ack();
    dat = AT24C_IIC_Receive_Byte();
    AT24C_IIC_No_Ack();
    AT24C_IIC_Stop();
    
    return dat;
}

/**
  * @brief  дһ��ҳ��64λ������
  * @param  Address: 0x00~0xff������ʼ��ַ
  * @param  *dat: д������ݵ���ʼ��ַ
  * @retval ��
  */
void AT24C_Write_Page(uint16 Address, uint8 * dat)
{
    uint8 i;
    
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address >> 8));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address));
    AT24C_IIC_Slave_Ack();
    for (i = 0; i < 64; i++, dat++)
    {
        AT24C_IIC_Send_Byte(*dat);
        AT24C_IIC_Slave_Ack();
    }
    AT24C_IIC_Stop();
}

/**
  * @brief  ������
  * @param  Address: 0x00~0xff������ʼ��ַ
  * @param  *dat: �������ݵ�ָ��
  * @param  len: �������ݵ�λ��
  * @retval ��
  */
void AT24C_Read(uint16 Address, uint8 * dat, uint16 len)
{
    uint16 i;

    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address >> 8));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS | 0x01);
    AT24C_IIC_Slave_Ack();
    for (i = 0; i < len - 1; i++, dat++)
    {
        *dat = AT24C_IIC_Receive_Byte();
        AT24C_IIC_Master_Ack();
    }
    *dat = AT24C_IIC_Receive_Byte();
    AT24C_IIC_No_Ack();
    AT24C_IIC_Stop();
}

/**
  * @brief  �������ҳд����
  * @param  Address: 0x00~0xff������ʼ��ַ
  * @param  dat: д�������
  * @param  len:���ݵ�λ��
  * @retval ��
  */
void AT24C_Write(uint16 Address, uint8 * dat, uint16 len)
{
    uint16 i;
    
    AT24C_IIC_Start();
    AT24C_IIC_Send_Byte(AT24C_ADDRESS);
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address >> 8));
    AT24C_IIC_Slave_Ack();
    AT24C_IIC_Send_Byte((uint8)(Address));
    AT24C_IIC_Slave_Ack();
    for (i = 0; i < len; i++, Address++, dat++)
    {
        if (Address % 64 == 0)
        {
            AT24C_IIC_Stop();
            delay_ms(4);
            AT24C_IIC_Start();
            AT24C_IIC_Send_Byte(AT24C_ADDRESS);
            AT24C_IIC_Slave_Ack();
            AT24C_IIC_Send_Byte((uint8)(Address >> 8));
            AT24C_IIC_Slave_Ack();
            AT24C_IIC_Send_Byte((uint8)(Address));
            AT24C_IIC_Slave_Ack();
            AT24C_IIC_Send_Byte(*dat);
            AT24C_IIC_Slave_Ack();
        }
        else
        {
            AT24C_IIC_Send_Byte(*dat);
            AT24C_IIC_Slave_Ack();
        }
    }
    AT24C_IIC_Stop();
    delay_ms(4);
}
/**************************************************************************gps_para**************************************************************************/
// float gps_para[32] = {0};
// float control_para[10] = {0};
// float pid_para[10] = {0};

// float turning_distance = 2.5; //��ͷʱ��ɲ������
// float stop_distance    = 2.5; //ͣ��ʱ��ɲ������
// float mindistance      = 2.5; //��׶Ͱʱ��ɲ������
// float slowdistance     = 6;   //���ٵľ���
// float high_speed       = 5;   //�����ٶ� 
// float low_speed        = 2;   //����
// float Speed_at_stop    = 1;   //���ٽ������ٶ�

// uint8  stop_num        = 9;   //���ս���ͣ���ĵ�λ
// uint8  turning_num     = 4;   //ɲ����ͷ�ĵ�λ
// uint8  start_num1      = 0;   //ȥ��ʱ�����ĵ�λ
// uint8  start_num2      = 1;   //�ص�ʱ�����ĵ�λ  

/**************************************************************************pid_para**************************************************************************/
// float straight_kp      = 300;//ֱ�߲���
// float straight_ki      = 55.5;
// float turn_kp          = 450 ;//�������
// float turn_ki          = 20.5;
// float bottom_kp        = 640 ;//�ײ�����
// float bottom_ki        = 90  ;

/**************************************************************************control_para**************************************************************************/
// float yaw1                 =  90; //��ʼ���ֵ�yaw
// float middle_output_back   = 750; //�������ȵ���ֵ
// float turn_output_up       = 125; //������������޷�
// float turn_output_down     =-125; //����������С�޷�
// uint16 bottom_up           = 225; //�ײ�����������޷�
// uint16 bottom_down         =  60; //�ײ���������С�޷�
// uint16 bottom_brake        =  50; //ɲ��ʱ�ײ�����
// uint16 bottom_middle       = 500; //�ײ����ȵ���ֵ
/**************************************************************************para_set**************************************************************************/
void para_init(void)
{
	// gpio_init(AT24C_SCL, GPO, 1, GPO_PUSH_PULL);
	// gpio_init(AT24C_SDA, GPO, 1, GPO_PUSH_PULL);

    gpio_mode(P2_6, GPO_PP);
    gpio_mode(P1_5, GPO_PP);
		
// 		read_float(gps_para,32,gps_para_page);
// 		read_float(control_para,10,control_para_page);
// 		read_float(enu_position,48,gps_position_page);
// 		read_float(pid_para,10,pid_para_page);
	
// 		left_output = middle_output_back;
// 		right_output = middle_output_back;
		
// //		gps_para[0] = turning_distance;
// //		gps_para[1] = stop_distance;
// //		gps_para[2] = mindistance;
// //		gps_para[3] = high_speed;
// //		gps_para[4] = low_speed;
// //		gps_para[5] = Speed_at_stop;

// //		gps_para[6] = (float)stop_num;
// //		gps_para[7] = (float)turning_num;
// //		gps_para[8] = (float)start_num1;
// //		gps_para[9] = (float)start_num1;

// //		/**************************************************************************control_para**************************************************************************/
// //		control_para[0] = yaw1; //��ʼ���ֵ�yaw
// //		control_para[1] = middle_output_back; //�������ȵ���ֵ
// //		control_para[2] = turn_output_up; //������������޷�
// //		control_para[3] = turn_output_down; //����������С�޷�

// //		control_para[4] = (float)bottom_up; //�ײ�����������޷�
// //		control_para[5] = (float)bottom_down; //�ײ���������С�޷�
// //		control_para[6] = (float)bottom_brake; //ɲ��ʱ�ײ�����
// //		control_para[7] = (float)bottom_middle; //�ײ����ȵ���ֵ

// //		/**************************************************************************pid_para**************************************************************************/
// //		pid_para[0] = straight_kp; //ֱ�߲���
// //		pid_para[1] = straight_ki;
// //		pid_para[2] = turn_kp; //�������
// //		pid_para[3] = turn_ki;
// //		pid_para[4] = bottom_kp; //�ײ�����
// //		pid_para[5] = bottom_ki;

}

void para_save(void)
{
	
// /**************************************************************************gps_para**************************************************************************/		
// //	gps_para[0] = turning_distance;
// //	gps_para[1] = stop_distance;
// //	gps_para[2] = mindistance;
// //	gps_para[3] = high_speed;
// //	gps_para[4] = low_speed;
// //	gps_para[5] = Speed_at_stop;

// //	gps_para[6] = (float)stop_num;
// //	gps_para[7] = (float)turning_num;
// //	gps_para[8] = (float)start_num1;
// //	gps_para[9] = (float)start_num1;
	
// 	save_float(gps_para,32,gps_para_page);
// 	save_float(control_para,10,control_para_page);
// 	save_float(enu_position,48,gps_position_page);
// 	save_float(pid_para,10,pid_para_page);

// /**************************************************************************gps_para**************************************************************************/		
// 		turning_distance = gps_para[0];
// 		stop_distance    = gps_para[1];
// 		mindistance      = gps_para[2];
// 		high_speed       = gps_para[3];
// 		low_speed        = gps_para[4];
// 		Speed_at_stop    = gps_para[5];
	
// 		stop_num         = (uint8)gps_para[6];
// 		turning_num      = (uint8)gps_para[7];
// 		start_num1       = (uint8)gps_para[8];
// 		start_num1       = (uint8)gps_para[9];	

// /**************************************************************************control_para**************************************************************************/	
// 		yaw1                 = control_para[0]; //��ʼ���ֵ�yaw
// 		middle_output_back   = control_para[1]; //�������ȵ���ֵ
// 		turn_output_up       = control_para[2]; //������������޷�
// 		turn_output_down     = control_para[3]; //����������С�޷�
		
// 		bottom_up           = (uint16)control_para[4]; //�ײ�����������޷�
// 		bottom_down         = (uint16)control_para[5]; //�ײ���������С�޷�
// 		bottom_brake        = (uint16)control_para[6]; //ɲ��ʱ�ײ�����
// 		bottom_middle       = (uint16)control_para[7]; //�ײ����ȵ���ֵ
		
// /**************************************************************************pid_para**************************************************************************/
// 	  straight_kp      = pid_para[0];//ֱ�߲���
// 	  straight_ki      = pid_para[1];
// 	  turn_kp          = pid_para[2];//�������
// 	  turn_ki          = pid_para[3];
// 	  bottom_kp        = pid_para[4];//�ײ�����
// 	  bottom_ki        = pid_para[5];		
	
}
