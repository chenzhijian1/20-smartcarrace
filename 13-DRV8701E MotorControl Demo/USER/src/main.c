#include "headfile.h"

uint8 text[100];
uint32 length;
uint8 send_flag;

extern uint8 timing_started_start;
extern uint32 timer_cnt;

void main(void)
{   
    board_init(); // ��ʼ���Ĵ�������ɾ��
    EA = 0;

    delay_ms(300);
    
    motor_driver_init_ir();      // ���
    // motor_driver_init_dr();      // ���
    // suction_fan_init();   // ����

    // voltage_init();       // ��ѹ���
    
    encoder_init();       // ������
    direction_adc_init(); // ���
    
    wireless_uart_init(); // ���ߴ���
    ips114_init();        // ��Ļ��ʼ��
    imu660ra_init();      // ������
//	offset_init();        // ��Ư

    // while (1)
    //     P52 = !P52;
    // while (1) {
    //     voltage = read_voltage();
    //     if (voltage > 11500)
    //         break;
    // }

    // delay_ms(1599); // 899 60F 

    // while (1) {
    //     voltage = read_voltage();
    //     if (voltage > 11500)
    //         break;
    // }
    

    // ʹ��ȫ���ж�
    EA = 1;

    pit_timer_ms(TIM_1, 5);  // ��С������ǡ�������������
    pit_timer_ms(TIM_4, 5);  // �������ѹ��⡢·������
    // DataInit(); 

    // flag = 4;
    // normal_speed = 280.0f;    // �����ٶ�
	
    // ���ϵ�����
	// A_ = 1.0f;
	// B_ = 5.0f;
	// C_ = 0.0f;

    // ���ϵ����ȺͲ�
    A_ = 1.2f; // 25 25 100
	B_ = 1.7f;
	C_ = 0.5f;

    navigation_memory_test(); // ·���������

    while(1) {
        if(P75 == 0) // ����ģʽ ��������
        {
            flag_key_control = 1;
            // key_scan();
            // ui_display();
        }
        else // �ܳ�ģʽ ��������
        {
            flag_key_control = 1;
            ips114_show();
        }

        // �˴����еİ������ǵ����ã�ʵ����Ҫͨ�����Ÿ�ͣ����
        if (KEY1_PIN == 0) {
            write_path();
        }

        if (KEY2_PIN == 0) // �л�ģʽ
        {
            refresh();
            j = 1;
            flag_end = 0;

            timing_started_start = 1;
        }

        if (timer_cnt >= 200) { // 1s�л�״̬
            flag_key_fast = !flag_key_fast;
            flag_start = 1; // ��ʼ�ܣ���ʼ�𽥼���
            timing_started_start = 0;
            timer_cnt = 0;
        }

        if (KEY3_PIN == 0) // ����
        {
        	refresh();
            huandao_count = 0;
            j = 1;
            flag_end = 0;
        }

        if (KEY4_PIN == 0)  flag_stop = !flag_stop; 
		
 		// if (send_flag) {
 		// 	send_flag = 0;
        //     printf("%.1f,%.1f,%.1f,%.1f,%.1f,",
        //           kpa, kpb, kd,
        //           motor_left.Kp_motor, motor_left.Ki_motor);

        //     printf("%.2f,", aaddcc.err_dir);

        //     printf("%d,%d,%d,%d,%d,%d,%d,",
        //             motor_left.setspeed, motor_left.encoder_data,
        //             motor_right.setspeed, motor_right.encoder_data,
        //             motor_left.duty1,
        //             motor_right.duty1,
        //             normal_speed);
			
 		// 	printf("%d,", voltage);
					
 		// 	printf("%.1f,%.1f,%.1f,%.1f,%.1f,", AD_ONE[0],AD_ONE[1],AD_ONE[2],AD_ONE[3],AD_ONE[4]);
        //     // printf("%d,%d,%d,%d,%d,", ad_ave[0], ad_ave[1], ad_ave[2], ad_ave[3], ad_ave[4]);

        //     printf("%.1f,", gyro_z);

        //     printf("%.1f,%.1f,%.1f,", distance_before_huandao, distance_after_huandao, g_angle_turn);

        //     printf("%.1f,%.1f,%.1f,", A_, B_, C_);

        //     printf("%d\r\n", changed_speed);

        //     // printf("%.1f,%.1f,%.1f,%.1f,", motor_left.Kp_motor * motor_left.out_p, motor_left.Ki_motor * motor_left.out_i, motor_left.Kp_motor * motor_right.out_p, motor_left.Ki_motor * motor_right.out_i);

        //     // printf("%.1f\r\n", AD_ONE[0] + AD_ONE[3]);
        //     // printf("%.2f,%.6f,%d\r\n", yaw, Gyro_offset_z, imu660ra_gyro_z / 16.4);
 		// }

        if (send_flag_nav && path_point_count < MAX_PATH_POINTS) {
            send_flag_nav = 0;
            printf("%d,%.2f,%.2f,%d,%.2f\r\n", path_point_count, path_points[path_point_count].distance,
                path_points[path_point_count].yaw_relative, path_points[path_point_count].isturn, imu660ra_gyro_z / 16.4f);
        }

        if (flag_gyro_z) {
            flag_gyro_z = 0;
            imu660ra_get_gyro(); // ��ȡ����������
        }
    }
}