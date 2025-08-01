#include "headfile.h"

PathPoint path_points[MAX_PATH_POINTS] = {0};

uint16 path_point_count = 0; // ��һ����Ϊ����Ѿ���ʼ��Ϊ0,0
uint16 j = 1; // �����ƶ�·���������
uint16 path_point_count_threshold = 100; // ·������ֵ
uint8 flag_isturn = 0; // 0��ʾ��һ����ֱ��,1��ʾ��һ�������
uint8 flag_end = 0; // 0��ʾδ�����յ�,1��ʾ�����յ�
uint8 send_flag_nav = 0;
uint8 flag_fast_start = 0;

float speed_high = 320;
float speed_low = 230;
float speed_turn = 160;
float speed_adjust = 200;

uint8 a = 0;

void Path_record(void)
{
    // �ж��Ƿ���Ҫ��¼·���� (���ٶȳ�����ֵ)
    if (flag_isturn == 0) {
        if ((path_point_count < MAX_PATH_POINTS - 1) && (abs(imu660ra_gyro_z) / 16.4f > gyro_threshold_high)
             && fabs(yaw) >= angle_turn && encoder_ave > distance_min) {
            Point_record();
        }
    }
    else {
        if ((path_point_count < MAX_PATH_POINTS - 1) && (abs(imu660ra_gyro_z) / 16.4f < gyro_threshold_low)
             && encoder_ave > distance_min) {
            Point_record();
        }
    }

    // ������ ��¼·����ﵽ��ֵʱֹͣ��¼
    // if (path_point_count >= path_point_count_threshold) {
    //     flag = 4;
    //     set_leftspeed = 0;
    //     set_rightspeed = 0;
    //     normal_speed = 0;
    // }
}

void Point_record(void)
{
    path_point_count++;
    path_points[path_point_count].distance = encoder_ave;
    path_points[path_point_count].yaw_relative = yaw;
    path_points[path_point_count].isturn = flag_isturn;

    send_flag_nav = 1;

    // refresh();
    flag_isturn = !flag_isturn;
}

// ����ѭ������ (�ڶ�Ȧʹ��)
void fast_tracking(void)
{
    // if (flag_fast_start == 0) {
    //     flag_fast_start = 1;
    //     refresh();
    //     delay_ms(3000);
    // }
    // a = 1;
    if (j + 1 < path_point_count) {
        if (encoder_ave >= path_points[j].distance) {
            // �ƶ�����һ��·����
            j++;
            // refresh();
        }
        speed_select(path_points[j].isturn, (path_points[j-1].distance + path_points[j].distance) / 2,
                     path_points[j].distance - path_points[j-1].distance, path_points[j].yaw_relative, path_points[j + 1].yaw_relative);
    }
    else {
        if (flag_end == 0) { // ��һ�ε����յ�
            if (encoder_ave >= path_points[j].distance) {
                // �ƶ�����һ��·����
                j = 1;
                flag_end = 1;
                refresh();
            }
        }
        else {
            flag = 5;
        }
    }
}

void speed_select(uint8 isturn, float middle, float dis, float angle, float angle_next)
{
    // ������ֱ��
    // if (isturn == 1) // ���
    // {
    //     if (fabs(angle) >= 70)  normal_speed = speed_turn;
    //     else  normal_speed = speed_low;
    // }
    // else  normal_speed = speed_high;

    if (isturn == 1) // ���
    {
        if (fabs(angle) >= angle_90)  normal_speed = speed_turn;
        else  normal_speed = speed_low;
    }
    else // ֱ��
    {
        if (dis > distance_threshold) {// ֱ�߾���ﵽ50cm����Ϊ��ֱ��
            if (encoder_ave < middle)  normal_speed = speed_high;
            else {
                if (fabs(angle_next) >= angle_90)  normal_speed = speed_turn;
                else  normal_speed = speed_low;
            }
        }
        else {
            if (fabs(angle_next) >= angle_90)  normal_speed = speed_turn;
            else  normal_speed = speed_low;
        }
        
        // if (fabs(angle_next) >= angle_90)  normal_speed = speed_turn;
        // else {
        //     if (middle > distance_threshold / 2) {// ֱ�߾���ﵽ50cm����
        //         if (encoder_ave < middle)  normal_speed = speed_high;
        //         else  normal_speed = speed_low;
        //     }
        // }
    }
}

void refresh(void) {
    encoder_clear();
    yaw = 0;
}

void write_path(void) {
    // ��·������д�� EEPROM����ʼ��ַѡ�� 0x200�������� my_peripheral.c �в�������ͻ
    uint16 addr = 0x200;
    uint16 i;

    // �����������õ���ҳ����ǰ��������� 500 ���㣬��Լ 10 kB
    iap_erase_page(0x200);   // 0x200 �C 0x3FF
    iap_erase_page(0x400);   // 0x400 �C 0x5FF
    iap_erase_page(0x600);   // 0x600 �C 0x7FF
    iap_erase_page(0x800);   // 0x800 �C 0x9FF
    iap_erase_page(0xA00);   // 0xA00 �C 0xBFF
    iap_erase_page(0xC00);   // 0xC00 �C 0xDFF
    iap_erase_page(0xE00);   // 0xE00 �C 0xFFF
    iap_erase_page(0x1000);   // 0x1000 �C 0x11FF
    iap_erase_page(0x1200);   // 0x1200 �C 0x13FF
    iap_erase_page(0x1400);   // 0x1400 �C 0x15FF
    iap_erase_page(0x1600);   // 0x1600 �C 0x17FF
    iap_erase_page(0x1800);   // 0x1800 �C 0x19FF
    iap_erase_page(0x1A00);   // 0x1A00 �C 0x1BFF
    iap_erase_page(0x1C00);   // 0x1C00 �C 0x1DFF
    iap_erase_page(0x1E00);   // 0x1E00 �C 0x1FFF
    iap_erase_page(0x2000);   // 0x2000 �C 0x21FF
    iap_erase_page(0x2200);   // 0x2200 �C 0x23FF
    iap_erase_page(0x2400);   // 0x2400 �C 0x25FF
    iap_erase_page(0x2600);   // 0x2600 �C 0x27FF

    /*
     * �ȴ洢·���������������ȡ��ԭ
     * ���� extern_iap_write_float д�룺����λ 3 λ��С�� 0 λ
     * ���� = num + pointnum + 3 = 3 + 0 + 3 = 6 �ֽ�
     */
    extern_iap_write_float((float)path_point_count, 3, 0, addr);
    addr += 6;

    /*
     * ����д�����·���㣺
     * distance : ���� 5 λ��С�� 1 λ  -> 5 + 1 + 3 = 9 �ֽ�
     * yaw      : ���� 3 λ��С�� 1 λ  -> 3 + 1 + 3 = 7 �ֽ�
     * isturn   : ���� 1 λ��С�� 0 λ  -> 1 + 0 + 3 = 4 �ֽ�
     * �ܼ�ÿ�� 20 �ֽ�
     */
    for (i = 0; i < path_point_count; i++) {
        // ����
        extern_iap_write_float((float)path_points[i].distance, 5, 1, addr);
        addr += 9;

        // �Ƕ�
        extern_iap_write_float((float)path_points[i].yaw_relative, 3, 1, addr);
        addr += 7;

        // �Ƿ�ת���־��0/1��
        extern_iap_write_float((float)path_points[i].isturn, 1, 0, addr);
        addr += 4;
    }
}

void read_path(void) {
    uint16 addr = 0x200;
    uint16 i;

    /* ��ȡ·�������� */
    // path_point_count = (uint16)iap_read_float(6, addr);
    if (path_point_count > MAX_PATH_POINTS) {
        path_point_count = MAX_PATH_POINTS;
        flag_key_control = 0;
    }
    addr += 6;

    /* ��˳���ȡ��·�������� */
    for (i = 0; i < path_point_count; i++) {
        /* distance 5 �� 1 С : 9 �ֽ� */
        path_points[i].distance = iap_read_float(9, addr);
        addr += 9;

        /* yaw 3 �� 1 С : 7 �ֽ� */
        path_points[i].yaw_relative = iap_read_float(7, addr);
        addr += 7;

        /* isturn 1 �� 0 С : 4 �ֽ� */
        path_points[i].isturn = (uint8)iap_read_float(4, addr);
        addr += 4;
    }
}