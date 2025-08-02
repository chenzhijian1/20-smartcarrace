#ifndef __AT24C_H__
#define __AT24C_H__

//用于AT24C128，不同内存大小的芯片的地址字节数不同
//AT24C128有64个字节每页
#define AT24C_ADDRESS 0xA0              //从机地址

// EEPROM通用浮点数数组存储的起始地址
#define EEPROM_FLOAT_ARRAY_BASE_ADDR 0x0000

#define gps_para_page              0
#define control_para_page          1
#define gps_position_page          2
#define pid_para_page              3
/**************************************************************************AT24C，IIC通信**************************************************************************/
/**************************************config**************************************/
//IIC电平控制
#define AT24C_SCL   P26
#define AT24C_SDA   P15
//AT24C_IIC延迟
#define AT24C_IIC_DELAY 1
void AT24C_IIC_Delay(void);
/**************************************AT24C_IIC**************************************/
void AT24C_IIC_Start(void);
void AT24C_IIC_Stop(void);
bit AT24C_IIC_Slave_Ack(void);
void AT24C_IIC_No_Ack(void);
void AT24C_IIC_Master_Ack(void);
void AT24C_IIC_Send_Byte(uint8 dat);           //AT24C_IIC发送一个字节
uint8 AT24C_IIC_Receive_Byte(void);

/**************************************AT24C**************************************/
void AT24C_Write_Byte(uint16 Address, uint8 dat);            //写一个数据
uint8 AT24C_Read_Byte(uint16 Address);                       //读一个数据
void AT24C_Write_Page(uint16 Address, uint8 * dat);           //写一整页（64位）数据
void AT24C_Read(uint16 Address, uint8 * dat, uint16 len);   //读数据
void AT24C_Write(uint16 Address, uint8 * dat, uint16 len);  //随机长度页写数据

/**************************************para_set**************************************/
// extern float gps_para[32];
// extern float control_para[10];
// extern float pid_para[10];

// extern float turning_distance ; //掉头时的刹车距离
// extern float stop_distance    ; //停车时的刹车距离
// extern float mindistance      ; //绕锥桶时的刹车距离
// extern float slowdistance     ; //降速的距离
// extern float high_speed       ; //高速速度 
// extern float low_speed        ; //低速
// extern float Speed_at_stop    ; //降速降到的速度

// extern uint8  stop_num        ; //最终结束停车的点位
// extern uint8  turning_num     ; //刹车掉头的点位
// extern uint8  start_num1      ; //去的时候进入的点位
// extern uint8  start_num2      ; //回的时候进入的点位  

// extern float straight_kp      ;//直线参数
// extern float straight_ki      ;
// extern float turn_kp          ;//弯道参数
// extern float turn_ki          ;
// extern float bottom_kp        ;//底部参数
// extern float bottom_ki        ;

/**************************************************************************control_para**************************************************************************/
// extern float yaw1                 ; //初始积分的yaw
// extern float middle_output_back   ; //后部推力扇的中值
// extern float turn_output_up       ; //后部推力的最大限幅
// extern float turn_output_down     ; //后部推力的最小限幅
// extern uint16 bottom_up           ; //底部推力的最大限幅
// extern uint16 bottom_down         ; //底部推力的最小限幅
// extern uint16 bottom_brake        ; //刹车时底部推力
// extern uint16 bottom_middle       ;

// void para_init(void);
// void para_save(void);

// void gps_para_set(void);
// void control_para_set(void);
// void pid_para_set(void);

// 通用浮点数数组记忆函数声明
void AT24C_Save_Float_Array(uint16 eeprom_addr, float *data_array, uint16 num_elements);
void AT24C_Read_Float_Array(uint16 eeprom_addr, float *data_array, uint16 max_elements);

#endif /*__AT24C_H__*/
