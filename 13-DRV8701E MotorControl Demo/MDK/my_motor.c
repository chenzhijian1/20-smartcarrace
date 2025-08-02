#include "my_motor.h"
//******************* ��Ҫ���Ĳ���********************/
// ����
#define huandao_num 1 // ��������
uint8 huandao_count = 0; // ��������
uint8 huandao_directions[huandao_num] = {0};      // 0 ��ʾ�󻷵�, 1 ��ʾ�һ���
// float huandao_hight_speed[huandao_num] = {190, 170, 0, 0}; // ��n������������
// float huandao_low_speed[huandao_num] = {65, 75, 75, 0};   // ��n������������
uint16 huandao_r[huandao_num] = {280}; // ��n�������뾶
// const uint16 r_out = 800; // �����뾶
float ratio = 0;
uint8 flag2 = 0;

const uint16 d = 165; // ����

/*
3s   dis_high  dis_low  g_angle_turn_high  g_angle_turn_low
n250    85                     330    
n280    55       50            320             320
n300    40       35            310             300
n320    20       15            310             300
*/
float distance_before_huandao = 0;
float distance_before_huandao_high = 55; // ����ǰ����
float distance_before_huandao_low = 50; // ����ǰ����
float distance_after_huandao = 140; // ���������

float g_angle_turn = 0;
float g_angle_turn_high = 320;
float g_angle_turn_low = 320;

float angle_in_threshold = 30; // ������ڽǶ���ֵ
float angle_out_threshold = 30; // �������ڽǶ���ֵ

// �ٶ�
int16 normal_speed = 0;
int16 normal_speed_pre = 0;
int16 speed_huandao = 0;
int16 normal_speed_cal = 0;

// �ɷ���
float kp_direction = 5; // ���򻷵�pid
float kd_direction = 11;
float kp_direction_2 = 5;	
float kd_direction_2 = 11;
float kp_direction_3 = 5;
float kd_direction_3 = 11;

// �·���
float kpa = 22.0f; // 4   2
float kpb = 20.0f; // 15  10
float kd = 100.0f; // 40  25
float kd_imu = 0.0f;

// ���ٶȻ�
float kp_gyro = 1.3f;
float kd_gyro = 0.7f;
float target_gyro_z = 0.0f; // �������ٶ�
float gyro_err = 0.0f;      // ���ٶȻ���ǰ���
float gyro_last_err = 0.0f; // ���ٶȻ�ǰһ�����

// �ٶȻ�
float kp_motor = 18.0f;
float ki_motor = 9.0f;
float kd_motor = 0.0f;

uint8 flag = 0; // 0: ����ģʽ��1: Ԥ����ģʽ��2: ����ģʽ��3: ����������4: �ϰ�ģʽ��5: �µ�ģʽ
uint8 flag_stop = 0; // 0: δֹͣ��1: ֹͣ
uint8 flag_key_control = 0; // 0������ģʽ��1���ܳ�ģʽ
uint8 flag_key_fast = 0; // 0������ģʽ��1������ģʽ
uint8 flag_start = 0; // 0: δ��ʼ����ѭ����1: ��ʼ����ѭ��
uint8 cnt_start = 0;
uint8 cnt_stop = 0;

int16 test_speed = 0;       // setspeed
int16 changed_speed = 0;
int16 set_leftspeed = 0;
int16 set_rightspeed = 0;
// ����������ֵ
// float encoder_left = 0.0;
// float encoder_right = 0.0;
// float encoder_ave = 0.0;
int32 encoder_left = 0;
int32 encoder_right = 0;
int32 encoder_ave = 0;

float encoder_temp = 0.0;

float lpf_encoder = 0.2; //��������ͨ�˲�ϵ��
float lpf_motor = 0.1;   //�����ͨ�˲�ϵ��
float lpf_gyro = 0.2;   //�����ǵ�ͨ�˲�ϵ��

uint8 flag_huandao = 0;     // 0��ʾ�󻷵���1��ʾ�һ���
uint8 flag_set_angle = 0;
uint16 flag_circle_in = 0, cnt_circle_in = 0;
uint16 flag_circle_out = 0, cnt_circle_out = 0;

float target_angle_in = 0;
float target_angle_out = 0;
motor_struct motor_left, motor_right; // �������ٶȱջ�����

float k;
float s = 0;

float gyro_z;

uint8 time = 0;

uint16 pwm_fan = 0;

void speed_change()
{
    if (flag == 0)
        car_stop_judge();
    if (flag_stop == 0)
    {
        // if (flag_key_fast == 1)  fast_tracking();
        // else
        //     if (normal_speed != 0)  Path_record();
        
        if (imu660ra_gyro_z <= 4 && imu660ra_gyro_z >= -4)
            imu660ra_gyro_z = 0;
        gyro_z = gyro_z * lpf_gyro + (float)imu660ra_gyro_z / 16.4f * (1 - lpf_gyro); // �����ǵ�ͨ�˲�

        switch (flag)
        {
        case 0: // ����ģʽ
//            if(fabs(aaddcc.err_dir) < 1 && fabs(aaddcc.last_err_dir) < 1 && !(aaddcc.err_dir ==0 && aaddcc.last_err_dir ==0 ))
           				// test_speed += 0.001;
            // if (time == 0)
                dir_pid(aaddcc.err_dir, aaddcc.last_err_dir, gyro_z); // ��������������ٶ�target_gyro_z
            // gyro_pd_control(); // ���ٶȻ�����changed_speed
            
            // time = (time + 1) % 2;

            // �ٶȲ���
            // if (flag_start && cnt_start < 100) {
            if (normal_speed_pre == 0 && normal_speed != 0)
                flag_start = 1;
            
            if (flag_start && cnt_start < 100) {
                cnt_start++;
                // normal_speed_cal = (int16)normal_speed / (100 * 100) * cnt_start * cnt_start;
                normal_speed_cal = (int16)((float)normal_speed * cnt_start / 100.0f);
            }
            else {
                flag_start = 0;
                cnt_start = 0;
                normal_speed_cal = (int16)-s * aaddcc.err_dir * aaddcc.err_dir + normal_speed;
            }

            normal_speed_pre = normal_speed;
            test_speed = (int16)normal_speed_cal;

            if (flag_key_fast == 1) {
                // ֱ���������ͬ�Ĳ����޷�
                if (path_points[j].isturn == 1) {
                    changed_speed = MINMAX(changed_speed, -100, 100); // ���ٶȻ������changed_speedҲ��Ҫ�޷�
                
                    // ���ټ���
                    k = fabs(aaddcc.err_dir / 50.0f);
                    if (changed_speed > 0) {
                        set_leftspeed = test_speed - changed_speed * (1 + k);
                        set_rightspeed = test_speed + changed_speed;
                    }
                    else {
                        set_leftspeed = test_speed - changed_speed;
                        set_rightspeed = test_speed + changed_speed * (1 + k);
                    }

                    set_leftspeed = MINMAX(set_leftspeed, -100, 400);
                    set_rightspeed = MINMAX(set_rightspeed, -100, 400);
                }
                else {
                    changed_speed = MINMAX(changed_speed, -300, 300); // ���ٶȻ������changed_speedҲ��Ҫ�޷�

                    set_leftspeed = test_speed - changed_speed;
    			    set_rightspeed = test_speed + changed_speed;

                    set_leftspeed = MINMAX(set_leftspeed, -100, 600);
                    set_rightspeed = MINMAX(set_rightspeed, -100, 600);
                }
            }
            else {
                changed_speed = MINMAX(changed_speed, -110, 110); // ���ٶȻ������changed_speedҲ��Ҫ�޷�
                
                //���ټ���
                k = fabs(aaddcc.err_dir / 100.0f);
                if (changed_speed > 0) {
                    set_leftspeed = test_speed - changed_speed * (1 + k);
                    set_rightspeed = test_speed + changed_speed;
                }
                else {
                    set_leftspeed = test_speed - changed_speed;
                    set_rightspeed = test_speed + changed_speed * (1 + k);
                }

                // set_leftspeed = test_speed - changed_speed;
                // set_rightspeed = test_speed + changed_speed;

                set_leftspeed = MINMAX(set_leftspeed, -100, 500);
                set_rightspeed = MINMAX(set_rightspeed, -100, 500);
            }
			
            break;
        case 1: // Ԥ����ģʽ
            // if (AD_ONE[0] > AD_ONE[4])
            //     flag_huandao = 0; // �󻷵�
            // else
            //     flag_huandao = 1; // �һ���
            flag_huandao = huandao_directions[huandao_count];
            
            distance_before_huandao = voltage > 9000 ? distance_before_huandao_high : distance_before_huandao_low;

            if (encoder_ave - encoder_temp < distance_before_huandao) { //û����������
				//ֱ��
                normal_speed = 0;
                set_leftspeed = normal_speed;
				set_rightspeed = normal_speed;
                // set_leftspeed = 0;
                // set_rightspeed = 0;
            }
            else  flag = 2;
            break;

        case 2: // ����ģʽ
            if (flag_set_angle == 0) { // ��һ�ν���
                target_angle_in = yaw;
                flag_set_angle = 1;
            }

            g_angle_turn = voltage > 9000 ? g_angle_turn_high : g_angle_turn_low;

            // �������ټ���
            ratio = (float)(huandao_r[huandao_count] - (d/2)) / (float)(huandao_r[huandao_count] + (d/2));

            if (flag_huandao == 0) { // �󻷵�
                target_angle_out = target_angle_in - g_angle_turn; // Ŀ������Ƕ�

                set_leftspeed = (int16)(normal_speed * ratio);
                set_rightspeed = normal_speed;

                if (yaw < target_angle_out) {
                    flag = 3; // ����
                    encoder_temp = encoder_ave;
                }
            }
            else { // �һ���
                target_angle_out = target_angle_in + g_angle_turn; // Ŀ������Ƕ�
                
                set_leftspeed = normal_speed;
                set_rightspeed = (int16)(normal_speed * ratio);

                if (yaw > target_angle_out) {
                    flag = 3; // ����
                    encoder_temp = encoder_ave;
                }
            }
            break;

        case 3: // ����
            if (encoder_ave - encoder_temp <= distance_after_huandao) {
                // if (flag_huandao == 0) { // �󻷵�
                //     set_leftspeed = normal_speed * (r_out - (d/2)) / (r_out + (d/2));
                //     // set_leftspeed = normal_speed * 0.9;
				//     set_rightspeed = normal_speed;
                // }
				// else { // �һ���
                //     set_leftspeed = normal_speed;
				//     set_rightspeed = normal_speed * (r_out - (d/2)) / (r_out + (d/2));
                //     // set_rightspeed = normal_speed * 0.9;
				// }
                set_leftspeed = normal_speed;
                set_rightspeed = normal_speed;
			}
			else {
				// �ָ�������ѭ��
				flag = 0;
				flag1 = 0;
                flag_huandao = 0;
                // ���ֱ�־λ����
				flag_set_angle = 0;
				
				// cnt_circle_in = 0;
				flag_circle_in = 0;
				// cnt_circle_out = 0;
				flag_circle_out = 0;

                huandao_count = (huandao_count + 1) % huandao_num; // �л�����
            }
            break;

        case 4: // �𲽷���
            if (encoder_ave >= 133)  flag = 0;

            set_leftspeed = normal_speed;
            set_rightspeed = normal_speed;

            break;

        case 5: // ����ͣ��
            if (cnt_stop < 200) {
                cnt_stop++;
                normal_speed_cal = (int16)normal_speed / 200 * (200 - cnt_stop);
                set_leftspeed = normal_speed_cal;
                set_rightspeed = normal_speed_cal;
            }
            else {
                cnt_stop = 0;
                normal_speed = 0;
                set_leftspeed = 0;
                set_rightspeed = 0;
                flag_key_control = 0;
            }

        default:
            break;
        }
    }
}

//���߱���
void car_stop_judge() {
	if (AD_ONE[0] < 0.5 && AD_ONE[1] < 0.5 && AD_ONE[3] < 0.5 && AD_ONE[4] < 0.5) {
        set_leftspeed = 0;
        set_rightspeed = 0;
		normal_speed = 0;
        // flag_stop = 1;
    }
}

//****************************************
// ������� ����������ٶ�ֵ
// ����˵�� void
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� encoder_get();
// ǰ��ʱ����ֵ��Ϊ��ֵ,��ֵ��motor_left.encoder_data��motor_right.encoder_data
//****************************************
void encoder_get(void) {
    motor_left.encoder_data = (int16)ctimer_count_read(SPEEDL_PULSE);
    motor_right.encoder_data = (int16)ctimer_count_read(SPEEDR_PULSE);

    ctimer_count_clean(SPEEDL_PULSE);
    ctimer_count_clean(SPEEDR_PULSE);

    if (SPEEDL_DIR == 0) //�۲���Ļ�������  1 0�ǳ�ǰհ 0 1�Ƕ�ǰհ
        motor_left.encoder_data = -motor_left.encoder_data;
    if (SPEEDR_DIR == 1)
        motor_right.encoder_data = -motor_right.encoder_data;
}

void encoder() {
    // encoder_left += 0.017 * motor_left.encoder_data;
    // encoder_right += 0.017 * motor_right.encoder_data; // 0.017��ô�������
    // encoder_ave = (encoder_left + encoder_right) / 2;
    encoder_left += motor_left.encoder_data;
    encoder_right += motor_right.encoder_data; // 0.017��ô�������
    encoder_ave = (encoder_left + encoder_right) / 2;
}

void encoder_clear() {
    // encoder_left = 0.0;
    // encoder_right = 0.0;
    // encoder_ave = 0.0;
    encoder_left = encoder_right = encoder_ave = 0;
}

void dir_pid (float error, float last_error, float gyro) {
    int16 p_out, d_out, output;
    
    p_out = (int16)((kpa / 10) * error + (kpb / 10000) * error * error * error);

	d_out = (int16)(kd * (error - last_error)) + (int16)(kd_imu / 100.0 * gyro);
    output = p_out + d_out;

    // target_gyro_z = -(float)output; // ��������������ٶ�
    changed_speed = output;
}

void dir_pid_sep (float error, float last_error) {
    int16 output;
	
    if (fabs(error) <= 20)
        output = kp_direction * error + kd_direction * (error - last_error);
    else if (fabs(error) > 20 && fabs(error) <= 40)
             output = kp_direction_2 * error + kd_direction_2 * (error - last_error);
    else
        output = kp_direction_3 * error + kd_direction_3 * (error - last_error);
	
    changed_speed = output;
}

//****************************************
// ������� ���ٶ�PD�ջ�����
// ����˵�� void
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� gyro_pd_control();
//****************************************
void gyro_pd_control(void) {
    gyro_last_err = gyro_err;
    gyro_err = target_gyro_z - gyro_z; // �������ٶ� - ʵ�ʽ��ٶ�

    changed_speed = (int16)(-kp_gyro * gyro_err - kd_gyro * (gyro_err - gyro_last_err));
}

//****************************************
// ������� ����������
// ����˵�� void
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� motor_driver_open_out_ir();
//****************************************
void motor_driver_open_out_ir(void) {
    if (motor_left.duty1 >= 0) {
        pwm_duty(PWMA_CH1P_P60, (uint32)motor_left.duty1);
		pwm_duty(PWMA_CH3P_P64, 0);
    }
    else {
        pwm_duty(PWMA_CH1P_P60, 0);
        pwm_duty(PWMA_CH3P_P64, (uint32)(-motor_left.duty1));
    }

    // ����
    if (motor_right.duty1 >= 0) {
        pwm_duty(PWMA_CH2P_P62, (uint32)motor_right.duty1);
        pwm_duty(PWMA_CH4P_P66, 0);
    }
    else {
        pwm_duty(PWMA_CH2P_P62, 0);
        pwm_duty(PWMA_CH4P_P66, (uint32)(-motor_right.duty1));
    }
}

//****************************************
// ������� ����������
// ����˵�� void
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� motor_driver_open_out_dr();
void motor_driver_open_out_dr(void) {
    if (motor_left.duty1 >= 0) {
        pwm_duty(PWMA_CH3P_P64, (uint32)motor_left.duty1);  // ��ת PWM
        P60 = 0 ; // ������ƣ�0=��ת
    }
    else {
        pwm_duty(PWMA_CH3P_P64, (uint32)(-motor_left.duty1));  // ��ת PWM��ȡ����ֵ��
        P60 = 1 ;// ������ƣ�1=��ת
    }

    if (motor_right.duty1 >= 0) {
        pwm_duty(PWMA_CH4P_P66, (uint32)motor_right.duty1);  // ��ת PWM
        P62 = 0 ; // ������ƣ�0=��ת
    }
    else {
        pwm_duty(PWMA_CH4P_P66, (uint32)(-motor_right.duty1));  // ��ת PWM��ȡ����ֵ��
        P62 = 1 ;  // ������ƣ�1=��ת
	}
}

//****************************************
// ������� �������ʽPID�ջ�
// ����˵�� void
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� motor_closed_loop_control(motor_left);
//****************************************
int16 motor_closed_loop_control(motor_struct *sptr) {
    int tspeed;
    
    sptr->err = sptr->setspeed - sptr->encoder_data;

    tspeed = (int16)(sptr->Kp_motor * (sptr->err - sptr->err1) + sptr->Ki_motor * sptr->err + sptr->Kd_motor * (sptr->err - 2 * sptr->err1 + sptr->err2));

    sptr->err2 = sptr->err1;
    sptr->err1 = sptr->err;

    // sptr->out_motor_pid = MINMAX(sptr->out_motor_pid + MINMAX(tspeed, -2000, 2000), -10000, 10000);
    sptr->out_motor_pid = MINMAX(sptr->out_motor_pid + tspeed, -10000, 10000);

    return sptr->out_motor_pid;
}

//****************************************
// ������� ����ջ�ʵ�ֺ���
// ����˵�� speed_l�������趨�ٶ� speed_r���ҵ���趨�ٶ�
// ����˵�� ��
// ���ز��� ��
// ʹ��ʾ�� motor_control(motor_left);
//****************************************
void motor_control(int16 speed_l, int16 speed_r) {
	if (normal_speed == 0) {
		motor_left.setspeed = 0;
		motor_right.setspeed = 0;
	}
	else {
		motor_left.setspeed = speed_l;
		motor_right.setspeed = speed_r;
	}

    motor_closed_loop_control(&motor_left);
    motor_closed_loop_control(&motor_right);
	
    // 12mm
    motor_left.duty1 = motor_left.setspeed < 1000 ? 
                       motor_left.setspeed * 1000 / 65 + motor_left.out_motor_pid :
                       1000 + (motor_left.setspeed - 65) * 10 + motor_left.out_motor_pid;
    motor_right.duty1 = motor_right.setspeed < 1000 ? 
                        motor_right.setspeed * 1000 / 65 + motor_right.out_motor_pid :
                        1000 + (motor_right.setspeed - 65) * 10 + motor_right.out_motor_pid;

    // 19mm
    // motor_left.duty1 = motor_left.setspeed < 1000 ? 
    //                     motor_left.setspeed * 1000 / 65 + motor_left.out_motor_pid :
    //                     1000 + (motor_left.setspeed - 65) / 40 * 500 + motor_left.out_motor_pid;
    // motor_right.duty1 = motor_right.setspeed < 1000 ? 
    //                      motor_right.setspeed * 1000 / 45 + motor_right.out_motor_pid :
    //                      1000 + (motor_right.setspeed - 45) / 40 * 500 + motor_right.out_motor_pid;
	
	// motor_left.duty1 = 2000;
	// motor_right.duty1 = 1000;

    motor_left.duty1 = MINMAX(motor_left.duty1 * (12600.0f / voltage), -10000, 10000);
    motor_right.duty1 = MINMAX(motor_right.duty1 * (12600.0f / voltage), -10000, 10000);

    motor_driver_open_out_ir();
	// motor_driver_open_out_dr();
}

// ��������ʼ��
void encoder_init(void) {                                    
    ctimer_count_init(SPEEDL_PULSE); // ��ʼ����ʱ��0��Ϊ�ⲿ����
    ctimer_count_init(SPEEDR_PULSE); // ��ʼ����ʱ��3��Ϊ�ⲿ����
}

// �����ʼ��
void motor_driver_init_ir(void) {                                                
    pwm_init(PWMA_CH1P_P60, 17000, 0);           // ��ʼ��PWM1  ʹ��P60����  ��ʼ��Ƶ��Ϊ17Khz
    pwm_init(PWMA_CH2P_P62, 17000, 0);           // ��ʼ��PWM2  ʹ��P62����  ��ʼ��Ƶ��Ϊ17Khz
    pwm_init(PWMA_CH3P_P64, 17000, 0);           // ��ʼ��PWM3  ʹ��P64����  ��ʼ��Ƶ��Ϊ17Khz
    pwm_init(PWMA_CH4P_P66, 17000, 0);           // ��ʼ��PWM4  ʹ��P66����  ��ʼ��Ƶ��Ϊ17Khz
    motor_struct_parameter_init(&motor_left, 0); // �ٶȻ��ṹ�������ʼ��
    motor_struct_parameter_init(&motor_right, 0);
}

void motor_driver_init_dr(void) {
    motor_struct_parameter_init(&motor_left, 0); // �ٶȻ��ṹ�������ʼ��
    motor_struct_parameter_init(&motor_right, 0);
    pwm_init(PWMA_CH3P_P64, 17000, 0);           // ��ʼ��PWM2  ʹ��P62����  ��ʼ��Ƶ��Ϊ17Khz
    pwm_init(PWMA_CH4P_P66, 17000, 0);           // ��ʼ��PWM4  ʹ��P66����  ��ʼ��Ƶ��Ϊ17Khz
    gpio_mode(P6_0, GPO_PP);
    gpio_mode(P6_2, GPO_PP);
}

// ����ٶȻ���ʼ����*sptr������ṹ����׵�ַ sspeed�����õĳ��ٶ�
void motor_struct_parameter_init(motor_struct *sptr, int16 sspeed) {
    sptr->duty1 = 0; // int32
    
    sptr->setspeed = sspeed; // int16
    sptr->actspeed = 0;      // int16
    sptr->encoder_data = 0;  // int16

    sptr->err = 0;  // int16
    sptr->err1 = 0; // int16
    sptr->err2 = 0;

    
    sptr->out_p = 0;
    sptr->out_i = 0;
    sptr->out_d = 0;
    sptr->out_motor_pid = 0;

    sptr->Kp_motor = kp_motor; // �궨�������ϵ��
    sptr->Ki_motor = ki_motor;
    sptr->Kd_motor = kd_motor;
}

void suction_fan_init(void) {
    gpio_mode(P7_7, GPO_PP);
    pwm_init(PWMA_CH4N_P33, 17000, 0);
}

void suction_fan_control(void) {
    P77 = 1;
    if (pwm_fan < PWM_DUTY_MAX)  pwm_fan += 100;
    else  pwm_fan = PWM_DUTY_MAX;
    pwm_duty(PWMA_CH4N_P33, pwm_fan);
}