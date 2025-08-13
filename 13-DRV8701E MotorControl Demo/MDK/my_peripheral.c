#include "my_peripheral.h"
#include "my_motor.h"

float Gyro_offset_z;
extern uint32 timer_cnt;
extern uint8 timing_started;

#define GYRO_OFFSET 0.412046

#define THRESHOLD_JUMP 500 // tof�˲�Խ��ֵ
// �洢�滻��
// float kp_direction_iap = 5;
// float kd_direction_iap = 11;
// float kp_direction_2_iap = 5;
// float kd_direction_2_iap = 11;
// float kp_direction_3_iap = 5;
// float kd_direction_3_iap = 11;
float kpa_iap = 6;
float kpb_iap = 14;
float kd_iap = 40;
float kd_imu_iap = 0;

float kp_motor_iap = 22;
float ki_motor_iap = 4;

// float block_speed_iap = 100;
// float hightv_huandao_iap = 170;
// float lowv_huandao_iap = 75;
// float max_speed_iap = 230;

float speed_high_iap = 500;
float speed_low_iap = 400;
float speed_90_iap = 300;
float speed_S_iap = 200;
float normal_speed_iap = 0;

// float block_judge_iap = 600;
// float block_out_encode_iap = 150;
// float block_back_encode_iap = 250;
// float block_out_angle_iap = 35;
// float block_back_angle_iap = 20;

float distance_before_huandao_iap = 150;
float distance_after_huandao_iap = 150;
float angle_in_threshold_iap = 30;
float angle_out_threshold_iap = 30;

// ���뿪��״̬����
uint8 sw1_status;
uint8 sw2_status;
uint8 sw3_status;
uint8 sw4_status;
// ����״̬����
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;
// ��һ�ο���״̬����
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;
// ���ر�־λ
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

uint16 tof[2] = {0};    // tof�ı�������
uint8 count_tof = 0;    // ����tof����
uint8 count_flag_4 = 0; // tof�жϴ���
float yaw = 0;
ui_struct ui = {&ui_display, 1, 0, 0};

uint8 flag_gyro_z = 0;

//void block_judgement()
//{
//    // ��ȡ5��TOF������ֵ
//    int adtemp;
//    float adtemp2;    // ��¼����ǰ�ٶ�
//    int i, j, k;      // ð����jk
//    int tof_value[5]; // ���ڴ洢5��TOF������ֵ

//    for (i = 0; i < 5; i++)
//    {
//        tof_value[i] = adc_once(ADC_P16, ADC_12BIT); // TOF
//    }

//    // ��5��TOF������ֵ����ð������
//    for (j = 0; j < 4; j++)
//    {
//        for (k = 0; k < 4 - j; k++)
//        {
//            if (tof_value[k] > tof_value[k + 1])
//            {
//                adtemp = tof_value[k];
//                tof_value[k] = tof_value[k + 1];
//                tof_value[k + 1] = adtemp;
//            }
//        }
//    }

//    // ����TOFֵ����
//    tof[1] = tof[0];
//    tof[0] = (tof_value[1] + tof_value[2] + tof_value[3]) / 3;

//    // �ж��ϰ�
//    if (tof[0] > 300)
//    {
//        adtemp2 = test_speed;
//        test_speed = block_speed;
//        if ((tof[0] > block_judge) && (flag == 0) &&
//            (tof[1] > (block_judge - 200)) &&
//            (abs(tof[0] - tof[1]) < THRESHOLD_JUMP) &&
//            ((tof[0] - tof[1]) > 50))
//        {
//            encoder_clear();
//            yaw = 0;
//            BEEP = 1;
//            if (P77 == 0)
//            {
//                flag = 4;
//                count_flag_4 += 1;
//                if (count_flag_4 == 2)
//                {
//                    flag = 5;
//                }
//            }
//            else
//            {
//                flag = 5;
//                count_flag_4 += 1;
//                if (count_flag_4 == 2)
//                {
//                    flag = 4;
//                }
//            }
//        }
//    }
//    else if (tof[0] < 300)
//        test_speed = adtemp2;
//}

void DataInit() {// eeprom��ʼ������
    iap_init(); // eeprom��ʼ��
//	iap_erase_page(0xff) ;
//	extern_iap_write_float(kp_direction, 3, 1, 0x07);   // kp_direction
//  extern_iap_write_float(kd_direction, 3, 1, 0x10);  // kd_direction
//	extern_iap_write_float(kp_direction_2, 3, 1, 0x90);
//	extern_iap_write_float(kd_direction_2, 3, 1, 0xa0);
//	extern_iap_write_float(kp_direction_3, 3, 1, 0xa6);
//	extern_iap_write_float(kd_direction_3, 3, 1, 0xb0);
//  extern_iap_write_float(block_speed, 3, 1, 0x17); // block_speed
//  extern_iap_write_float(huandao_hight_speed [0], 3, 1, 0x20); // hightv_huandao
//  extern_iap_write_float(huandao_low_speed [0], 3, 1, 0x27);  // lowv_huandao
//  extern_iap_write_float(max_speed, 3, 1, 0x30); // max_speed
//   extern_iap_write_float(block_judge, 3, 1, 0x50); // block_judge
//    extern_iap_write_float(block_out_encode, 3, 1, 0x56); // block_out_encode
//    extern_iap_write_float(block_back_encode, 3, 1, 0x80); // block_back_encode
//    extern_iap_write_float(normal_speed, 3, 1, 0x65); // normal_speed
//	  extern_iap_write_float(block_out_angle, 3, 1, 0xd0);
//	  extern_iap_write_float(block_back_angle, 3, 1, 0xd7);
    
    // pid����
    kpa = iap_read_float(7, 0x07) > 0 ? iap_read_float(7, 0x07) : kpa_iap;
    kpb = iap_read_float(7, 0x10) > 0 ? iap_read_float(7, 0x10) : kpb_iap;
    kd = iap_read_float(7, 0x90) > 0 ? iap_read_float(7, 0x90) : kd_iap;
    kd_imu = iap_read_float(7, 0xa0) > 0 ? iap_read_float(7, 0xa0) : kd_imu_iap;
    kp_motor = iap_read_float(7, 0xa6) > 0 ? iap_read_float(7, 0xa6) : kp_motor_iap;
    ki_motor = iap_read_float(7, 0xb0) > 0 ? iap_read_float(7, 0xb0) : ki_motor_iap;

//     block_speed = iap_read_float(7, 0x17);
// 	huandao_hight_speed [0]= iap_read_float(7, 0x20);
// 	huandao_low_speed [0]= iap_read_float(7, 0x27);
// // hightv_huandao = iap_read_float(7, 0x20);
// //lowv_huandao = iap_read_float(7, 0x27);
//     max_speed = iap_read_float(7, 0x30);
    
    // �ٶȲ���
    speed_high = iap_read_float(7, 0x17) > 0 ? iap_read_float(7, 0x17) : speed_high_iap;
    speed_low = iap_read_float(7, 0x20) > 0 ? iap_read_float(7, 0x20) : speed_low_iap;
    speed_90 = iap_read_float(7, 0x27) > 0 ? iap_read_float(7, 0x27) : speed_90_iap;
    speed_S = iap_read_float(7, 0x30) > 0 ? iap_read_float(7, 0x30) : speed_S_iap;
    normal_speed = iap_read_float(7, 0x65) > 0 ? iap_read_float(7, 0x65) : normal_speed_iap;

    // ��ʱ��δ���ĳɻ����еĿɵ�����������Ҳ��Ҫ�������ʽһ��
    // block_judge = iap_read_float(7, 0x50);
    // block_out_encode = iap_read_float(7, 0x56);
    // block_back_encode = iap_read_float(7, 0x80);
    // block_out_angle = iap_read_float(7, 0xd0);
    // block_back_angle = iap_read_float(7, 0xd7); // �����û����

    // ��������
    distance_before_huandao = iap_read_float(7, 0x50) > 0 ? iap_read_float(7, 0x50) : distance_before_huandao_iap;
    distance_after_huandao = iap_read_float(7, 0x56) > 0 ? iap_read_float(7, 0x56) : distance_after_huandao_iap;
    angle_in_threshold = iap_read_float(7, 0x80) > 0 ? iap_read_float(7, 0x80) : angle_in_threshold_iap;
    angle_out_threshold = iap_read_float(7, 0xd0) > 0 ? iap_read_float(7, 0xd0) : angle_out_threshold_iap; 

    // ��·����ʱ����ȡ·���㣬������ flag_key_fast Ϊ 1 �������ѭ��ģʽ
    path_point_count = iap_read_float(6, 0x200) > 0 ?(uint16)iap_read_float(6, 0x200) : 0;
    if (path_point_count != 0) {
        read_path();
        flag_key_fast = 1;
        send_flag_nav = 1;
    }
    else {
        flag_key_fast = 0;
    }
}

// gpio����Ĭ������

//****************************************
//  �������      ui����page�ı���������
//  ����˵��      ��
//  ����˵��      ��
//  ���ز���      ��
//  ʹ��ʾ��
//****************************************
unsigned char xdata ui_page0[8][30] =
    {
        "  <INCREDIBLE_KING>   <page0>",
        "  pid",
        "  speed",
        "  circle",
        "",
        "",
        "",
        "  <LOAD_IN_DATA>"};

unsigned char xdata ui_page1[8][30] =
    {
        "  <pid>               <page1>",
        "  kpa",
        "  kpb",
        "  kd",
        "  kd_imu",
        "  kp_motor",
        "  ki_motor",
        "  <EXIT>---------------------"};

unsigned char xdata ui_page2[8][30] =
    {
        "  <speed>             <page2>",
        "  normal_speed",
        "  speed_high",
        "  speed_low",
        "  speed_90",
        "  speed_S",
        "",
        "  <EXIT>---------------------"};

unsigned char xdata ui_page3[8][30] =
    {
        "  <circle>            <page3>",
        "  dis_before",
        "  dis_after",
        "  angle_in",
        "  angle_out",
        "",
        "",
        "  <EXIT>---------------------"};

unsigned char xdata ui_page4[8][30] =
    {
        "  <LOAD_IN_DATA>      <page4>",
        "   ****   ****  *    * *****",
        "   *   * *    * **   * *",
        "   *   * *    * * *  * *****",
        "   *   * *    * *  * * *",
        "   *   * *    * *   ** *",
        "   ****   ****  *    * *****",
        "  <EXIT>---------------------"};
//****************************************
//  �������      ui������ʾ����ͷ��ҳ������
//  ����˵��      string[8][17]
//  ����˵��      ��
//  ���ز���      ��
//  ʹ��ʾ��
//****************************************
void UI_DispUIStrings(uint8 strings[8][30]) {
    uint8 i;
    for (i = 0; i < 8; i++) {
        // ��ͷ������ui.cursor��
        if (i == ui.cursor) {
            strings[i][0] = '=';
            strings[i][1] = '>';
        }
        // �������ǿո�
        else if (i != 0)
            strings[i][0] = strings[i][1] = ' ';
        // ��8*30������������ʾ
        ips114_showstr(0, i, strings[i]);
    }
}
//****************************************
//  �������      ui�������к���
//  ����˵��      void
//  ����˵��      ��
//  ���ز���      ��
//  ʹ��ʾ��      ui_display();
//****************************************
void ui_display(void) {
    switch (ui.page) {// ��page����ѡ��
    case 0:
        if (ui.last == ui.page)
            UI_DispUIStrings(ui_page0);
        else if (ui.last != ui.page) {
            ui.cursor = 1; // ��ͷ�ص�page0������
            ui.last = ui.page;
            ips114_clear(BLACK);
            UI_DispUIStrings(ui_page0);
        }
        break;

    case 1:
        if (ui.last == ui.page) {
            UI_DispUIStrings(ui_page1);

            //
            ips114_showfloat(150, 1, kpa, 2, 2);
            ips114_showfloat(150, 2, kpb, 2, 2);
            ips114_showfloat(150, 3, kd, 2, 2);
            ips114_showfloat(150, 4, kd_imu, 2, 2);
            ips114_showfloat(150, 5, kp_motor, 2, 2);
            ips114_showfloat(150, 6, ki_motor, 2, 2);
            //
        }
        else if (ui.last != ui.page) {
            ui.cursor = 1; // ��ͷ�ص�page1������
            ui.last = ui.page;
            ips114_clear(BLACK);
            UI_DispUIStrings(ui_page1);

            //
            ips114_showfloat(150, 1, kpa, 2, 2);
            ips114_showfloat(150, 2, kpb, 2, 2);
            ips114_showfloat(150, 3, kd, 2, 2);
            ips114_showfloat(150, 4, kd_imu, 2, 2);
            ips114_showfloat(150, 5, kp_motor, 2, 2);
            ips114_showfloat(150, 6, ki_motor, 2, 2);
            //
        }
        break;
    case 2:
        if (ui.last == ui.page) {
            UI_DispUIStrings(ui_page2);

            //
            ips114_showfloat(150, 1, normal_speed, 3, 2);
            ips114_showfloat(150, 2, speed_high, 3, 2);
            ips114_showfloat(150, 3, speed_low, 3, 2);
            ips114_showfloat(150, 4, speed_90, 3, 2);
            ips114_showfloat(150, 5, speed_S, 3, 2);
            //
        }
        else if (ui.last != ui.page) {
            ui.cursor = 1; // ��ͷ�ص�page2������
            ui.last = ui.page;
            ips114_clear(BLACK);
            UI_DispUIStrings(ui_page2);

            //
            ips114_showfloat(150, 1, normal_speed, 3, 2);
            ips114_showfloat(150, 2, speed_high, 3, 2);
            ips114_showfloat(150, 3, speed_low, 3, 2);
            ips114_showfloat(150, 4, speed_90, 3, 2);
            ips114_showfloat(150, 5, speed_S, 3, 2);
            //
        }
        break;
    case 3:
        if (ui.last == ui.page) {
            UI_DispUIStrings(ui_page3);

            //
            // ips114_showfloat(155, 1, block_judge, 3, 2);
            // ips114_showfloat(155, 2, block_out_encode, 3, 2);
            // ips114_showfloat(155, 3, block_back_encode, 3, 2);
            // ips114_showfloat(155, 4, block_out_angle, 3, 2);
            ips114_showfloat(155, 1, distance_before_huandao, 3, 2);
            ips114_showfloat(155, 2, distance_after_huandao, 3, 2);
            ips114_showfloat(155, 3, angle_in_threshold, 3, 2);
            ips114_showfloat(155, 4, angle_out_threshold, 3, 2);
            // ips114_showfloat(155, 5, block_back_angle, 3, 2); // ��û����

            //
        }
        else if (ui.last != ui.page) {
            ui.cursor = 1; // ��ͷ�ص�page3������
            ui.last = ui.page;
            ips114_clear(BLACK);
            UI_DispUIStrings(ui_page3);

            //
            // ips114_showfloat(155, 1, block_judge, 3, 2);
            // ips114_showfloat(155, 2, block_out_encode, 3, 2);
            // ips114_showfloat(155, 3, block_back_encode, 3, 2);
            // ips114_showfloat(155, 4, block_out_angle, 3, 2);
            ips114_showfloat(155, 1, distance_before_huandao, 3, 2);
            ips114_showfloat(155, 2, distance_after_huandao, 3, 2);
            ips114_showfloat(155, 3, angle_in_threshold, 3, 2);
            ips114_showfloat(155, 4, angle_out_threshold, 3, 2);
            // ips114_showfloat(155, 5, block_back_angle, 3, 2); // ��û����

            //
        }
        break;
    case 4:
        if (ui.last == ui.page) {
            UI_DispUIStrings(ui_page4);

            //

            //
        }
        else if (ui.last != ui.page) {
            ui.cursor = 1; // ��ͷ�ص�page3������
            ui.last = ui.page;
            ips114_clear(BLACK);
            UI_DispUIStrings(ui_page4);

            //
            //
        }
        break;
    default:
        break;
    }
}
//****************************************
//  �������      �˵�����ɨ��
//  ����˵��      void
//  ����˵��      ��
//  ���ز���      ��
//  ʹ��ʾ��      keyScan();
//****************************************
void key_scan(void) {
    // KEY_DOW
    if (KEY2_PIN == 0) {
        ui.cursor++;
        if (ui.cursor > 7) {
            ui.cursor = 1;
            ui.page = ui.last;
        }
        while (1) {
            if (KEY3_PIN == 1)
                break;
        }
    }
    // KEY_UP
    if (KEY1_PIN == 0) {
        ui.cursor--;
        if (ui.cursor < 1) {
            ui.cursor = 7;
            ui.page = ui.last;
        }
        while (1) {
            if (KEY4_PIN == 1)
                break;
        }
    }

    // KEY_ENT
    if (KEY4_PIN == 0) {
        switch (ui.page) {
        case 0:
            if (ui.cursor == 1)
                ui.page = 1;
            else if (ui.cursor == 2)
                ui.page = 2;
            else if (ui.cursor == 3)
                ui.page = 3;
            else if (ui.cursor == 7) {
                ui.page = 4;

                // kp_direction_iap = kp_direction;
                // kd_direction_iap = kd_direction;
                // kp_direction_2_iap = kp_direction_2;
                // kd_direction_2_iap = kd_direction_2;
                // kp_direction_3_iap = kp_direction_3;
                // kd_direction_3_iap = kd_direction_3;
                kpa_iap = kpa;
                kpb_iap = kpb;
                kd_iap = kd;
                kd_imu_iap = kd_imu;
                kp_motor_iap = kp_motor;
                ki_motor_iap = ki_motor;

                normal_speed_iap = normal_speed;

                // ��ǣ�������������navigation.c�ٶȹ滮���ĸ��ٶ�
                // block_speed_iap = block_speed;
                // hightv_huandao_iap = huandao_hight_speed[0];
                // lowv_huandao_iap = huandao_low_speed[0];
                // max_speed_iap = max_speed;
                speed_high_iap = speed_high;
                speed_low_iap = speed_low;
                speed_90_iap = speed_90;
                speed_S_iap = speed_S;

                // block_judge_iap = block_judge;
                // block_out_encode_iap = block_out_encode;
                // block_back_encode_iap = block_back_encode;
                // block_out_angle_iap = block_out_angle;
                distance_before_huandao_iap = distance_before_huandao;
                distance_after_huandao_iap = distance_after_huandao;
                angle_in_threshold_iap = angle_in_threshold;
                angle_out_threshold_iap = angle_out_threshold;
                // block_back_angle_iap = block_back_angle;

                extern_iap_write_float(kpa_iap, 3, 1, 0x07);
                extern_iap_write_float(kpb_iap, 3, 1, 0x10);
                extern_iap_write_float(kd_iap, 3, 1, 0x90);
                extern_iap_write_float(kd_imu_iap, 3, 1, 0xa0);
                extern_iap_write_float(kp_motor_iap, 3, 1, 0xa6);
                extern_iap_write_float(ki_motor_iap, 3, 1, 0xb0);

                extern_iap_write_float(speed_high_iap, 3, 1, 0x17);
                extern_iap_write_float(speed_low_iap, 3, 1, 0x20);
                extern_iap_write_float(speed_90_iap, 3, 1, 0x27);
                extern_iap_write_float(speed_S_iap, 3, 1, 0x30);
                extern_iap_write_float(normal_speed_iap, 3, 1, 0x65);

                // extern_iap_write_float(block_judge_iap, 3, 1, 0x50);
                // extern_iap_write_float(block_out_encode_iap, 3, 1, 0x56);
                // extern_iap_write_float(block_back_encode_iap, 3, 1, 0x80);
                // extern_iap_write_float(block_out_angle_iap, 3, 1, 0xd0);
                extern_iap_write_float(distance_before_huandao_iap, 3, 1, 0x50);
                extern_iap_write_float(distance_after_huandao_iap, 3, 1, 0x56);
                extern_iap_write_float(angle_in_threshold_iap, 3, 1, 0x80);
                extern_iap_write_float(angle_out_threshold_iap, 3, 1, 0xd0);
                // extern_iap_write_float(block_back_angle_iap, 3, 1, 0xd7); // ��û����
            }

            break;
        case 1:
            if (ui.cursor == 1)
                kpa += 0.5f;
            else if (ui.cursor == 2)
                kpb += 1.0f;
            else if (ui.cursor == 3)
                kd += 1.0f;
            else if (ui.cursor == 4)
                kd_imu += 0.5f;
            else if (ui.cursor == 5)
                kp_motor += 1.0f;
            else if (ui.cursor == 6)
                ki_motor += 0.1f;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 2:
            if (ui.cursor == 1)
                normal_speed += 5;
            else if (ui.cursor == 2)
                speed_high += 5;
            else if (ui.cursor == 3) 
                speed_low += 5;
            else if (ui.cursor == 4)
                speed_90 += 5;
            else if (ui.cursor == 5)
                speed_S += 5;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 3:
            if (ui.cursor == 1)
                distance_before_huandao += 1;
            else if (ui.cursor == 2)
                distance_after_huandao += 1;
            else if (ui.cursor == 3)
                angle_in_threshold += 1;
            else if (ui.cursor == 4)
                angle_out_threshold += 1;
            else if (ui.cursor == 5)
                ;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 4:
            if (ui.cursor == 7)
                ui.page = 0;
            break;
        default:
            break;
            // ÿһҳ�����Լ���Ӧ����תҳ��
        }
        while (1) {
            if (KEY1_PIN == 1)
                break;
        }
    }
    // KEY_OUT
    if (KEY3_PIN == 0) {
        switch (ui.page) {
        case 0:
            ui.page = 0;
            break;
        case 1:
            if (ui.cursor == 1)
                kpa -= 0.5f;
            else if (ui.cursor == 2)
                kpb -= 1.0f;
            else if (ui.cursor == 3)
                kd -= 1.0f;
            else if (ui.cursor == 4)
                kd_imu -= 0.5f;
            else if (ui.cursor == 5)
                kp_motor -= 1.0f;
            else if (ui.cursor == 6)
                ki_motor -= 0.1f;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 2:
            if (ui.cursor == 1)
                normal_speed -= 5;
            else if (ui.cursor == 2)
                speed_high -= 5;
            else if (ui.cursor == 3)
                speed_low -= 5;
            else if (ui.cursor == 4)
                speed_90 -= 5;
            else if (ui.cursor == 5)
                speed_S -= 5;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 3:
            if (ui.cursor == 1)
                distance_before_huandao -= 1;
            else if (ui.cursor == 2)
                distance_after_huandao -= 1;
            else if (ui.cursor == 3)
                angle_in_threshold -= 1;
            else if (ui.cursor == 4)
                angle_out_threshold -= 1;
            else if (ui.cursor == 5)
                ;
            else if (ui.cursor == 7)
                ui.page = 0;
            break;
        case 4:
            if (ui.cursor == 7)
                ui.page = 0;
            break;
        default:
            break;
        }
        while (1) {
            if (KEY2_PIN == 1)
                break;
        }
    }
}

void ips114_show(void) { // ips114��Ļ��ʾ
    // ips114_showstr(0, 0, "tof");
    // ips114_showuint16(40, 0, tof[0]); // ��ʾtofֵ
    ips114_showstr(0, 0, "n");
    ips114_showfloat(30, 0, normal_speed, 3, 2); // ��ʾ�ٶ�ֵ

    ips114_showstr(100, 0, "encoder");
    ips114_showfloat(160, 0, encoder_ave, 5, 1); // ��ʾencoderֵ
    // 0: ����ģʽ��1: Ԥ����ģʽ��2: ����ģʽ��3: ����������4: �ϰ�ģʽ��5: �µ�ģʽ

    // ��ʾ��м���������
//    ips114_showstr(0, 1, "left");
//    ips114_showuint16(50, 1, ad_ave[0]);  // ��ʾ����ԭʼֵ
//    ips114_showuint16(100, 1, ad_ave[1]); // ��ʾ��ǰ���ԭʼֵ

//    ips114_showstr(0, 2, "right");
//    ips114_showuint16(50, 2, ad_ave[3]);  // ��ʾ�Һ���ԭʼֵ
//    ips114_showuint16(100, 2, ad_ave[4]); // ��ʾ��ǰ���ԭʼֵ
    // ips114_showuint16(150, 2, ad_ave[6]);
    ips114_showstr(0, 1, "l");
    ips114_showfloat(30, 1, AD_ONE[0], 3, 2);  // ��ʾ���й�һ��ֵ
    ips114_showfloat(80, 1, AD_ONE[1], 3, 2); // ��ʾ��ǰ��й�һ��ֵ
    // ips114_showfloat(30, 1, ad_ave[0], 3, 2);  // ��ʾ���й�һ��ֵ
    // ips114_showfloat(80, 1, ad_ave[1], 3, 2); // ��ʾ��ǰ��й�һ��ֵ

    ips114_showstr(140, 1, "m");
    ips114_showfloat(170, 1, AD_ONE[2], 3, 2);  // ��ʾ�е�й�һ��ֵ
    // ips114_showfloat(170, 1, ad_ave[2], 3, 2);  // ��ʾ�е�й�һ��ֵ

    ips114_showstr(0, 2, "r");
    ips114_showfloat(30, 2, AD_ONE[3], 3, 2);  // ��ʾ�Һ��й�һ��ֵ
    ips114_showfloat(80, 2, AD_ONE[4], 3, 2); // ��ʾ��ǰ��й�һ��ֵ
    // ips114_showfloat(30, 2, ad_ave[3], 3, 2);  // ��ʾ�Һ��й�һ��ֵ
    // ips114_showfloat(80, 2, ad_ave[4], 3, 2); // ��ʾ��ǰ��й�һ��ֵ

//    ips114_showstr(0, 3, "cnt");
//    ips114_showuint8(50, 3, cnt_circle_in); 		   // ��ʾcnt

    ips114_showstr(140, 2, "flag");
    ips114_showuint8(170, 2, flag); 		   // ��ʾflag

    //	ips114_showstr(0, 3, "flag1");
    //    ips114_showuint8(50, 3, flag1); 		   // ��ʾflag1

	ips114_showstr(0, 3, "fast");
    ips114_showuint8(50, 3, flag_key_fast); 		   // ��ʾflag_key_fast

	
//	ips114_showstr(0, 3, "flagh");
//    ips114_showuint8(50, 3, flag_huandao); 		   // ��ʾflag_huandao

//	ips114_showstr(0, 3, "flago");
//    ips114_showuint16(50, 3, flag_circle_out); 		   // ��ʾflag_huandao

    // ��ʾ����������ֵ
    // ips114_showstr(110, 3, "type");
    // ips114_showuint8(170, 3, path_points[j].type);

    // ��ʾ��Ⱥʹ�����errֵ
    ips114_showstr(0, 4, "err");
    ips114_showfloat(50, 4, aaddcc.err_dir, 4, 2);

    ips114_showstr(110, 4, "hd_cnt");
    ips114_showuint8(170, 4, huandao_count);

    ips114_showstr(0, 5, "target");
    ips114_showint16(60, 5, motor_left.setspeed);
    ips114_showint16(120, 5, motor_right.setspeed);

    ips114_showstr(0, 6, "actual");
    ips114_showint16(60, 6, motor_left.encoder_data);
    ips114_showint16(120, 6, motor_right.encoder_data);

//    ips114_showstr(0, 7, "speed");
////  ips114_showuint8(50, 7, count_flag_4);
//    ips114_showfloat(50, 7, test_speed * 0.017336, 3, 3);
	
// 	ips114_showstr(0, 7, "offset");
// //  ips114_showuint8(50, 7, count_flag_4);
//     ips114_showfloat(50, 7, Gyro_offset_z, 4, 2);
    if (flag_key_fast == 1) {
        ips114_showstr(0, 7, "j");
        ips114_showuint16(50, 7, j);
    }
    else {
        ips114_showstr(0, 7, "j");
        ips114_showuint16(50, 7, path_point_count);
    }

    ips114_showstr(110, 7, "angle");
    ips114_showfloat(150, 7, yaw, 4, 2);

//	ips114_showstr(110, 7, "angle");
//    ips114_showfloat(150, 7, (imu660ra_gyro_z - Gyro_offset_z) / 65.6, 4, 2);
}

void angle_gyro() {
	if (imu660ra_gyro_z > 4 || imu660ra_gyro_z < -4)
		// yaw += (((float)imu660ra_gyro_z - GYRO_OFFSET) / 16.4f) * 0.005;
	yaw += (gyro_z) * 0.005;
}

void pit_callback(void) { // ���������ݻ�ȡ
    // imu660ra_get_acc();  // ��ȡ���ٶ�����
    imu660ra_get_gyro(); // ��ȡ����������
    flag_gyro_z = 1;
//	gyro_z = (float)imu660ra_gyro_z - Gyro_offset_z;
}

void beep_init(void) { // ������
    gpio_mode(BEEP, GPO_PP);
    BEEP = 1;
}

void beep_test(void) { // ���������0.5s����һ��
    BEEP = !BEEP;
    delay_ms(500);
}

float StrToDouble(const char *s) {
    int i = 0;
    int k = 0;
    float j;
    int symbol = 1;
    float result = 0.0;
    if (s[i] == '+')
    {
        i++;
    }
    if (s[i] == '-')
    {
        i++;
        symbol = -1;
    }
    while (s[i] != '\0' && s[i] != '.')
    {
        j = (s[i] - '0') * 1.0;
        result = result * 10 + j;
        i++;
    }
    if (s[i] == '.')
    {
        i++;
        while (s[i] != '\0' && s[i] != ' ')
        {
            k++;
            j = s[i] - '0';
            result = result + (1.0 * j) / pow(10.0, k);
            i++;
        }
    }
    result = symbol * result;
    return result;
}

float iap_read_float(uint8 len, uint16 addr) {
    uint8 buf[34];
    iap_read_bytes(addr, buf, len);
    return StrToDouble(buf);
}