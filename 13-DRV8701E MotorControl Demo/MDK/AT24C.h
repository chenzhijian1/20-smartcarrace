#ifndef __AT24C_H__
#define __AT24C_H__

//����AT24C128����ͬ�ڴ��С��оƬ�ĵ�ַ�ֽ�����ͬ
//AT24C128��64���ֽ�ÿҳ
#define AT24C_ADDRESS 0xA0              //�ӻ���ַ

// EEPROMͨ�ø���������洢����ʼ��ַ
#define EEPROM_FLOAT_ARRAY_BASE_ADDR 0x0000

#define gps_para_page              0
#define control_para_page          1
#define gps_position_page          2
#define pid_para_page              3
/**************************************************************************AT24C��IICͨ��**************************************************************************/
/**************************************config**************************************/
//IIC��ƽ����
#define AT24C_SCL   P26
#define AT24C_SDA   P15
//AT24C_IIC�ӳ�
#define AT24C_IIC_DELAY 1
void AT24C_IIC_Delay(void);
/**************************************AT24C_IIC**************************************/
void AT24C_IIC_Start(void);
void AT24C_IIC_Stop(void);
bit AT24C_IIC_Slave_Ack(void);
void AT24C_IIC_No_Ack(void);
void AT24C_IIC_Master_Ack(void);
void AT24C_IIC_Send_Byte(uint8 dat);           //AT24C_IIC����һ���ֽ�
uint8 AT24C_IIC_Receive_Byte(void);

/**************************************AT24C**************************************/
void AT24C_Write_Byte(uint16 Address, uint8 dat);            //дһ������
uint8 AT24C_Read_Byte(uint16 Address);                       //��һ������
void AT24C_Write_Page(uint16 Address, uint8 * dat);           //дһ��ҳ��64λ������
void AT24C_Read(uint16 Address, uint8 * dat, uint16 len);   //������
void AT24C_Write(uint16 Address, uint8 * dat, uint16 len);  //�������ҳд����

/**************************************para_set**************************************/
// extern float gps_para[32];
// extern float control_para[10];
// extern float pid_para[10];

// extern float turning_distance ; //��ͷʱ��ɲ������
// extern float stop_distance    ; //ͣ��ʱ��ɲ������
// extern float mindistance      ; //��׶Ͱʱ��ɲ������
// extern float slowdistance     ; //���ٵľ���
// extern float high_speed       ; //�����ٶ� 
// extern float low_speed        ; //����
// extern float Speed_at_stop    ; //���ٽ������ٶ�

// extern uint8  stop_num        ; //���ս���ͣ���ĵ�λ
// extern uint8  turning_num     ; //ɲ����ͷ�ĵ�λ
// extern uint8  start_num1      ; //ȥ��ʱ�����ĵ�λ
// extern uint8  start_num2      ; //�ص�ʱ�����ĵ�λ  

// extern float straight_kp      ;//ֱ�߲���
// extern float straight_ki      ;
// extern float turn_kp          ;//�������
// extern float turn_ki          ;
// extern float bottom_kp        ;//�ײ�����
// extern float bottom_ki        ;

/**************************************************************************control_para**************************************************************************/
// extern float yaw1                 ; //��ʼ���ֵ�yaw
// extern float middle_output_back   ; //�������ȵ���ֵ
// extern float turn_output_up       ; //������������޷�
// extern float turn_output_down     ; //����������С�޷�
// extern uint16 bottom_up           ; //�ײ�����������޷�
// extern uint16 bottom_down         ; //�ײ���������С�޷�
// extern uint16 bottom_brake        ; //ɲ��ʱ�ײ�����
// extern uint16 bottom_middle       ;

// void para_init(void);
// void para_save(void);

// void gps_para_set(void);
// void control_para_set(void);
// void pid_para_set(void);

// ͨ�ø�����������亯������
void AT24C_Save_Float_Array(uint16 eeprom_addr, float *data_array, uint16 num_elements);
void AT24C_Read_Float_Array(uint16 eeprom_addr, float *data_array, uint16 max_elements);

#endif /*__AT24C_H__*/
