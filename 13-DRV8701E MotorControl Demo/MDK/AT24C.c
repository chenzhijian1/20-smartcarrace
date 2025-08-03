#include "headfile.h"
#include "AT24C.h"

//  flash初始化
//	gpio_init(IO_P24, GPO, 1, GPO_PUSH_PULL);
//	gpio_init(IO_P51, GPO, 1, GPO_PUSH_PULL);

/**************************************************************************AT24C，IIC通信**************************************************************************/
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
  * @brief  写一个数据
  * @param  Address: 0x00~0xff数据起始地址
  * @param  dat: 写入的数据
  * @retval 无
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
  * @brief  读一个数据
  * @param  Address: 0x00~0xff数据起始地址
  * @retval dat: 读出的数据
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
  * @brief  写一整页（64位）数据
  * @param  Address: 0x00~0xff数据起始地址
  * @param  *dat: 写入的数据的起始地址
  * @retval 无
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
  * @brief  读数据
  * @param  Address: 0x00~0xff数据起始地址
  * @param  *dat: 读出数据的指针
  * @param  len: 读出数据的位数
  * @retval 无
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
  * @brief  随机长度页写数据
  * @param  Address: 0x00~0xff数据起始地址
  * @param  dat: 写入的数据
  * @param  len:数据的位数
  * @retval 无
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

// float turning_distance = 2.5; //掉头时的刹车距离
// float stop_distance    = 2.5; //停车时的刹车距离
// float mindistance      = 2.5; //绕锥桶时的刹车距离
// float slowdistance     = 6;   //降速的距离
// float high_speed       = 5;   //高速速度 
// float low_speed        = 2;   //低速
// float Speed_at_stop    = 1;   //降速降到的速度

// uint8  stop_num        = 9;   //最终结束停车的点位
// uint8  turning_num     = 4;   //刹车掉头的点位
// uint8  start_num1      = 0;   //去的时候进入的点位
// uint8  start_num2      = 1;   //回的时候进入的点位  

/**************************************************************************pid_para**************************************************************************/
// float straight_kp      = 300;//直线参数
// float straight_ki      = 55.5;
// float turn_kp          = 450 ;//弯道参数
// float turn_ki          = 20.5;
// float bottom_kp        = 640 ;//底部参数
// float bottom_ki        = 90  ;

/**************************************************************************control_para**************************************************************************/
// float yaw1                 =  90; //初始积分的yaw
// float middle_output_back   = 750; //后部推力扇的中值
// float turn_output_up       = 125; //后部推力的最大限幅
// float turn_output_down     =-125; //后部推力的最小限幅
// uint16 bottom_up           = 225; //底部推力的最大限幅
// uint16 bottom_down         =  60; //底部推力的最小限幅
// uint16 bottom_brake        =  50; //刹车时底部推力
// uint16 bottom_middle       = 500; //底部风扇的中值
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
// //		control_para[0] = yaw1; //初始积分的yaw
// //		control_para[1] = middle_output_back; //后部推力扇的中值
// //		control_para[2] = turn_output_up; //后部推力的最大限幅
// //		control_para[3] = turn_output_down; //后部推力的最小限幅

// //		control_para[4] = (float)bottom_up; //底部推力的最大限幅
// //		control_para[5] = (float)bottom_down; //底部推力的最小限幅
// //		control_para[6] = (float)bottom_brake; //刹车时底部推力
// //		control_para[7] = (float)bottom_middle; //底部风扇的中值

// //		/**************************************************************************pid_para**************************************************************************/
// //		pid_para[0] = straight_kp; //直线参数
// //		pid_para[1] = straight_ki;
// //		pid_para[2] = turn_kp; //弯道参数
// //		pid_para[3] = turn_ki;
// //		pid_para[4] = bottom_kp; //底部参数
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
// 		yaw1                 = control_para[0]; //初始积分的yaw
// 		middle_output_back   = control_para[1]; //后部推力扇的中值
// 		turn_output_up       = control_para[2]; //后部推力的最大限幅
// 		turn_output_down     = control_para[3]; //后部推力的最小限幅
		
// 		bottom_up           = (uint16)control_para[4]; //底部推力的最大限幅
// 		bottom_down         = (uint16)control_para[5]; //底部推力的最小限幅
// 		bottom_brake        = (uint16)control_para[6]; //刹车时底部推力
// 		bottom_middle       = (uint16)control_para[7]; //底部风扇的中值
		
// /**************************************************************************pid_para**************************************************************************/
// 	  straight_kp      = pid_para[0];//直线参数
// 	  straight_ki      = pid_para[1];
// 	  turn_kp          = pid_para[2];//弯道参数
// 	  turn_ki          = pid_para[3];
// 	  bottom_kp        = pid_para[4];//底部参数
// 	  bottom_ki        = pid_para[5];		
	
}
