#ifndef __MY_MOTOR_H
#define __MY_MOTOR_H
#include "headfile.h"

extern uint8 huandao_count;

#define MINN(a, b) (((a) < (b)) ? (a) : (b))
#define MAXX(a, b) (((a) > (b)) ? (a) : (b))
#define MINMAX(input, low, upper) MINN(MAXX(input, low), upper)

// �����������������
#define BEEP P67

// ���������Ŷ���
#define SPEEDL_PULSE CTIM0_P34 // ������������
#define SPEEDR_PULSE CTIM3_P04
#define SPEEDL_DIR P35 // ���巽������
#define SPEEDR_DIR P53
// ����������Ŷ���
#define LEFT_PWM PWMA_CH4P_P66
#define LEFT_DIR P64
#define RIGHT_PWM PWMA_CH2P_P62
#define RIGHT_DIR P60
// ���������ٶ�

extern uint8 flag2;
extern uint16 cnt_circle_in;

// ����ٶȻ��ṹ��
typedef struct
{
    int16 setspeed;
    int16 actspeed;
    int16 err;          // ��ǰ���
    int16 err1;         // ǰһ�����
    int16 err2;         // ǰ�������
    int16 encoder_data; // ������ֵ
    int16 duty1;        // ���pwmռ�ձ�
    int16 out_p;
    int16 out_i;
    int16 out_d;
    float Kp_motor;
    float Ki_motor;
    float Kd_motor;
    float out_motor_pid;
} motor_struct;

void car_stop_judge();
void encoder_init(void);
void encoder_get(void);
void encoder();
void encoder_clear();
void motor_struct_parameter_init(motor_struct *sptr, int16 sspeed);
void dir_pid (float, float, float);
void dir_pid_sep (float, float);
void gyro_pd_control(void); // �������ٶ�PD���ƺ���
void motor_driver_init_dr(void);
void motor_driver_init_ir(void);
void motor_driver_open_out(void);
int16 motor_closed_loop_control(motor_struct *sptr);
void motor_control(int16 speed_l, int16 speed_r);
void speed_change();
void speed_adjust(int16 c_speed, int16 s_speed);

void suction_fan_init(void);
void suction_fan_control(void);

extern uint8 cout_test;/***************/
/***********�ٶȾ���*********************/
// extern float adjust_speed_after_huandao ;
// extern float adjust_speed_after_block ;
// extern float adjust_speed_after_ramp ;
// extern float adjust_speed_after_shizi ;
/****************************************/
extern float kp_motor; //����ջ�
extern float ki_motor;
extern float kd_motor;

extern int16 test_speed;

// extern float kp_direction; // ���򻷵�pid
// extern float kd_direction;
// extern float kp_direction_2; // ���򻷵�pid
// extern float kd_direction_2;
// extern float kp_direction_3; // ���򻷵�pid
// extern float kd_direction_3;

extern float kpa; //���κ�����Ϸ���
extern float kpb;
extern float kd; // �������֮��
extern float kd_imu; // ������

// extern float kp_gyro; // ���ٶȻ���pd
// extern float kd_gyro;
// extern float target_gyro_z; // �������ٶ�
// extern float gyro_err;      // ���ٶȻ���ǰ���
// extern float gyro_last_err; // ���ٶȻ�ǰһ�����

extern motor_struct motor_left, motor_right;
extern int16 set_leftspeed;
extern int16 set_rightspeed;

extern uint8 flag;
extern uint8 flag_stop;
extern uint8 flag_key_control;
extern uint8 flag_key_fast;
extern uint8 flag_start;

extern uint8 flag_huandao; 
extern float target_angle_in;
extern float target_angle_out;

extern float encoder_ave;
extern float encoder_temp;
extern int16 normal_speed;
extern int16 changed_speed;
extern int16 speed_huandao;
// extern float huandao_hight_speed[4];
// extern float huandao_low_speed[4];
// extern float hightv_huandao;
// extern float lowv_huandao;
// extern float max_speed;
// extern float block_out_encode;
// extern float block_back_encode;
// extern float block_judge; // tof�ϰ���ֵ
// extern float block_speed;
// extern float block_out_angle;
// extern float block_back_angle;

extern float distance_before_huandao;
extern float distance_after_huandao;
extern float g_angle_turn;
extern float angle_in_threshold;
extern float angle_out_threshold;

extern float s;

extern float gyro_z;
extern float last_gyro_z; // ���������ڴ洢��һ�����ڵĽ��ٶ�

#endif
