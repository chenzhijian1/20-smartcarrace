#include "headfile.h"

PathPoint path_points[MAX_PATH_POINTS] = {0};

uint16 path_point_count = 0; // ��һ����Ϊ����Ѿ���ʼ��Ϊ0,0
uint16 j = 1; // �����ƶ�·���������
uint8 s_turn_active = 0; // ������S�伤���־�������ж��Ƿ��ڿ�������ΪS�����ͨ�����
uint16 s_turn_start_point_index = 0; // ������S����ʼ�������
uint8 s_turn_start_yaw_direction = 2; // ������S����ʼ���yaw���� (0:��ת/��yaw, 1:��ת/��yaw, 2:δ����/����)
uint8 current_path_type = PATH_STRAIGHT; // ��������ǰ·������
uint8 last_flag = 0; // ���������ڴ洢��һ�����ڵ�flagֵ
// uint16 path_point_count_threshold = 100; // ·������ֵ
uint8 flag_end = 0; // 0��ʾδ�����յ�,1��ʾ�����յ�
uint8 send_flag_nav = 0;
uint8 flag_fast_start = 0;

float speed_high = 320;
float speed_low = 230;
float speed_90 = 160;
float speed_S = 200;

// uint8 a = 0;

void Path_record(void) {
    uint16 i;
    // �����ж�
    if (flag == 2 && last_flag != 2) { // ��һ�ν��뻷��
        current_path_type = PATH_ROUNDABOUT;
        Point_record();
    }
    else if (flag == 2) { // ���ڻ�����
        // ����¼�κε�
    }
    else { // �ǻ���ģʽ
        // ����Ƿ��ֱ���������
        if (current_path_type == PATH_STRAIGHT && fabs(gyro_z) > gyro_threshold_high && fabs(yaw) > angle_turn && encoder_ave > distance_min) {
            current_path_type = PATH_TURN; // ���ж�Ϊ��ͨ���
            Point_record();
            s_turn_active = 1; // ����S����
            s_turn_start_point_index = path_point_count; // ��¼S����ʼ�������
            s_turn_start_yaw_direction = (yaw > 0) ? 1 : 0; // ��¼S����ʼ���yaw���� (1:��yaw/��ת, 0:��yaw/��ת)
        }
        // ����Ƿ���������ֱ�� (S���������Ҳһ��)
        else if ((current_path_type == PATH_TURN || current_path_type == PATH_S_TURN) && fabs(gyro_z) < gyro_threshold_low && encoder_ave > distance_min) {
            current_path_type = PATH_STRAIGHT; // ����ֱ��
            Point_record();
            s_turn_active = 0; // �˳�S����
            s_turn_start_point_index = 0; // ����S����ʼ������
            s_turn_start_yaw_direction = 2; // ����S����ʼ��yaw����Ϊδ����
        }
        // S�������жϣ������ǰ����ͨ�������yaw�仯����angle_180�����ߵ�ǰyaw��������ʼyaw�����෴��������ΪS��
        else if (s_turn_active && current_path_type == PATH_TURN && (fabs(yaw) >= angle_180 || 
                 ((yaw > 0 && s_turn_start_yaw_direction == 0) || (yaw < 0 && s_turn_start_yaw_direction == 1)))) {
            // ������S����ʼ�㵽��ǰ�㣬�����е�����͸���ΪPATH_S_TURN
            for (i = s_turn_start_point_index; i <= path_point_count; i++)
                path_points[i].type = PATH_S_TURN;
            current_path_type = PATH_S_TURN; // ���µ�ǰ·������
            // ����Ҫ���¼�¼�㣬ֻ���޸����Ѽ�¼�������
        }
        else if (s_turn_active && current_path_type == PATH_S_TURN) {
            // ������S���У�����¼�µ㣬ֱ����S��
        }
        // �������������¼�µ�
    }
    last_flag = flag;
}

void Point_record(void) {
    path_point_count++;
    path_points[path_point_count].distance = encoder_ave;
    path_points[path_point_count].yaw_relative = yaw;
    path_points[path_point_count].type = current_path_type; // ʹ���µ�ö������

    // send_flag_nav = 1;

    refresh(); // �ָ��˵��ã������������yaw��Ϊ��һ��·����׼��
    // flag_type = !flag_type; // ������Ҫ���� current_path_type ����
}

// ����ѭ������ (�ڶ�Ȧʹ��)
void fast_tracking(void) {
    // if (flag_fast_start == 0) {
    //     flag_fast_start = 1;
    //     refresh();
    //     delay_ms(3000);
    // }
    // a = 1;
    if (j + 1 < path_point_count) {
        // ֱ����У׼
        if (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_relative) > angle_90 && fabs(gyro_z) > gyro_threshold_high) {
            j++;
            refresh();
        }
        // ����У׼
        else if (path_points[j+1].type == PATH_ROUNDABOUT && flag == 2) {
            j++;
            refresh();
        }
        else if (encoder_ave >= path_points[j].distance) {
            // �ƶ�����һ��·����
            j++;
            refresh(); // �����������yaw����ʼ��һ�εľ������
        }
        // speed_select �������ڿ���ֱ��ʹ�÷ֶ�����
        speed_select(path_points[j].type, path_points[j].distance / 2,
                     path_points[j].distance, path_points[j].yaw_relative, path_points[j + 1].yaw_relative);
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
void speed_select(uint8 type, float middle, float dis, float angle, float angle_next) {
    switch (type) {
        case PATH_STRAIGHT: // ֱ��
            if (dis > distance_long) { // ��ֱ��
                if (encoder_ave < middle) normal_speed = speed_high;
                else {
                    if (fabs(angle_next) >= angle_90) normal_speed = speed_90;
                    else normal_speed = speed_low;
                }
            }
            else { // ��ֱ��
                if (fabs(angle_next) >= angle_90) normal_speed = speed_90;
                else normal_speed = speed_low;
            }
            break;
        case PATH_TURN: // ��ͨ���
            if (fabs(angle) >= angle_90) normal_speed = speed_90; // ֱ��
            else normal_speed = speed_low; // С����
            break;
        case PATH_S_TURN: // ����������
            normal_speed = speed_S;
            break;
        case PATH_ROUNDABOUT: // ����
            normal_speed = speed_high;
            break;
        default:
            normal_speed = speed_low; // Ĭ���ٶ�
            break;
    }
}

void refresh(void) {
    encoder_clear();
    yaw = 0;
}

void write_path(void) {
    // ��·������д�� EEPROM����ʼ��ַѡ�� 0x200�������� my_peripheral.c �в�������ͻ
    uint16 addr = 0x200;
    uint16 page_addr;
    uint16 i;

    // �����������õ���ҳ����ǰ��������� 500 ���㣬��Լ 10 kB
    // ѭ������ EEPROM ҳ��
    for (page_addr = 0x200; page_addr <= 0x2600; page_addr += 0x200)
        iap_erase_page(page_addr);

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
     * type     : ���� 1 λ��С�� 0 λ  -> 1 + 0 + 3 = 4 �ֽ�
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
        extern_iap_write_float((float)path_points[i].type, 1, 0, addr);
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

        /* type 1 �� 0 С : 4 �ֽ� */
        path_points[i].type = (uint8)iap_read_float(4, addr);
        addr += 4;
    }
}
