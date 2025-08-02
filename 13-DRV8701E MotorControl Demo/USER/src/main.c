#include "headfile.h"

uint8 text[100];
uint32 length;
uint8 send_flag;

extern uint8 timing_started_start;
extern uint32 timer_cnt;

void main(void)
{   
    board_init(); // 初始化寄存器，勿删除
    EA = 0;

    delay_ms(300);
    
    motor_driver_init_ir();      // 电机
    // motor_driver_init_dr();      // 电机
    // suction_fan_init();   // 风扇

    // voltage_init();       // 电压检测
    
    encoder_init();       // 编码器
    direction_adc_init(); // 电感
    
    wireless_uart_init(); // 无线串口
    ips114_init();        // 屏幕初始化
    imu660ra_init();      // 陀螺仪
//	offset_init();        // 零漂

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
    

    // 使能全局中断
    EA = 1;

    pit_timer_ms(TIM_1, 5);  // 电感、陀螺仪、编码器、串口
    pit_timer_ms(TIM_4, 5);  // 电机、电压检测、路径记忆
    // DataInit(); 

    // flag = 4;
    // normal_speed = 280.0f;    // 运行速度
	
    // 电感系数逐飞
	// A_ = 1.0f;
	// B_ = 5.0f;
	// C_ = 0.0f;

    // 电感系数差比和差
    A_ = 1.2f; // 25 25 100
	B_ = 1.7f;
	C_ = 0.5f;

    navigation_memory_test(); // 路径记忆测试

    while(1) {
        if(P75 == 0) // 调参模式 开关在上
        {
            flag_key_control = 1;
            // key_scan();
            // ui_display();
        }
        else // 跑车模式 开关在下
        {
            flag_key_control = 1;
            ips114_show();
        }

        // 此处所有的按键都是调试用，实际需要通过检测磁钢停车来
        if (KEY1_PIN == 0) {
            write_path();
        }

        if (KEY2_PIN == 0) // 切换模式
        {
            refresh();
            j = 1;
            flag_end = 0;

            timing_started_start = 1;
        }

        if (timer_cnt >= 200) { // 1s切换状态
            flag_key_fast = !flag_key_fast;
            flag_start = 1; // 开始跑，初始逐渐加速
            timing_started_start = 0;
            timer_cnt = 0;
        }

        if (KEY3_PIN == 0) // 重置
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
            imu660ra_get_gyro(); // 获取陀螺仪数据
        }
    }
}