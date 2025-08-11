#include "headfile.h"

PathPoint path_points[MAX_PATH_POINTS] = {0};

uint16 path_point_count = 0; // ��һ����Ϊ����Ѿ���ʼ��Ϊ0,0
uint16 j = 0; // �����ƶ�·���������
// uint8 s_turn_active = 0; // ������S�伤���־�������ж��Ƿ��ڿ�������ΪS�����ͨ�����
// uint16 s_turn_start_point_index = 0; // ������S����ʼ�������
// uint8 s_turn_start_yaw_direction = 2; // ������S����ʼ���yaw���� (0:��ת/��yaw, 1:��ת/��yaw, 2:δ����/����)
uint8 current_path_type = PATH_STRAIGHT; // ��������ǰ·������
uint8 last_flag = 0; // ���������ڴ洢��һ�����ڵ�flagֵ
// uint16 path_point_count_threshold = 100; // ·������ֵ
uint8 flag_end = 0; // 0��ʾδ�����յ�,1��ʾ�����յ�
uint8 send_flag_nav = 0;
uint8 flag_fast_start = 0;

static uint8 is_waiting_for_turn_confirmation = 0; // �������ȴ�ת��ȷ�ϱ�־
static uint16 waiting_start_index = 0; // �������ȴ�״̬��ʼʱ��·��������

float speed_high = 600;
float speed_low = 450;
float speed_90 = 350;
float speed_S = 200;

// uint8 a = 0;

void Path_record(void) {
    float prev_yaw_for_comparison; // �����ں�����ͷ

    // �����ж�
    if (flag == 2 && last_flag != 2) { // ��һ�ν��뻷��
        Point_record();
        current_path_type = PATH_ROUNDABOUT;
    }
    else if (flag == 2) { // ���ڻ�����
        // ����¼�κε�
    }
    else { // �ǻ���ģʽ
        // ����Ƿ��ֱ���������
        // ʹ�õ�ǰyaw����һ��·����yaw�Ĳ�ֵ���ж�ת��
        prev_yaw_for_comparison = (path_point_count == 0) ? 0.0f : path_points[path_point_count - 1].yaw_absolute;
        if (current_path_type == PATH_STRAIGHT && fabs(gyro_z) > gyro_threshold_high && fabs(yaw - prev_yaw_for_comparison) > angle_turn && encoder_ave > distance_min) {
            Point_record();
            current_path_type = PATH_TURN; // ���ж�Ϊ��ͨ���
            // s_turn_active = 1; // ����S����
            // s_turn_start_point_index = path_point_count; // ��¼S����ʼ�������
            // s_turn_start_yaw_direction = (yaw > 0) ? 1 : 0; // ��¼S����ʼ���yaw���� (1:��yaw/��ת, 0:��yaw/��ת)
        }
        // ����Ƿ���������ֱ�� (S���������Ҳһ��)
        else if (current_path_type == PATH_TURN && fabs(gyro_z) < gyro_threshold_low && encoder_ave > distance_min) { // �Ƴ��� || current_path_type == PATH_S_TURN
            Point_record();
            current_path_type = PATH_STRAIGHT; // ����ֱ��
            // s_turn_active = 0; // �˳�S����
            // s_turn_start_point_index = 0; // ����S����ʼ������
            // s_turn_start_yaw_direction = 2; // ����S����ʼ��yaw����Ϊδ����
        }
        // S�������жϣ������ǰ����ͨ�������yaw�仯����angle_180�����ߵ�ǰyaw��������ʼyaw�����෴��������ΪS��
        // else if (s_turn_active && current_path_type == PATH_TURN && (fabs(yaw) >= angle_180 || 
        //          ((yaw > angle_90 && s_turn_start_yaw_direction == 0) || (yaw < -angle_90 && s_turn_start_yaw_direction == 1)))) {
        //     // ������S����ʼ�㵽��ǰ�㣬�����е�����͸���ΪPATH_S_TURN
        //     for (i = s_turn_start_point_index; i <= path_point_count; i++)
        //         path_points[i].type = PATH_S_TURN;
        //     current_path_type = PATH_S_TURN; // ���µ�ǰ·������
        //     // ����Ҫ���¼�¼�㣬ֻ���޸����Ѽ�¼�������
        // }
        // else if (s_turn_active && current_path_type == PATH_S_TURN) {
        //     // ������S���У�����¼�µ㣬ֱ����S��
        // }
        // �������������¼�µ�
    }
    last_flag = flag;
}

void Point_record(void) {
    path_points[path_point_count].distance = encoder_ave;
    path_points[path_point_count].yaw_absolute = yaw;
    path_points[path_point_count].type = current_path_type; // ʹ���µ�ö������
    path_point_count++; // �ȸ�ֵ��������ʹpath_points[0]�洢��һ����

    send_flag_nav = 1;

    // refresh(); // �Ƴ��˵��ã�ʵ��ȫ����̼���
    // flag_type = !flag_type; // ������Ҫ���� current_path_type ����
}

// ����ѭ������ (�ڶ�Ȧʹ��)
void fast_tracking(void) {
    // �������ƫ��ȴ�ȷ��״̬
    if (is_waiting_for_turn_confirmation) {
        // ����Ƿ�����ת��򻷵�����
        uint8 turn_condition_met = 0;
        if (waiting_start_index + 1 < path_point_count) {
            // ����Ƿ���ֱ����
            if (path_points[waiting_start_index+1].type == PATH_TURN && fabs(path_points[waiting_start_index+1].yaw_absolute - path_points[waiting_start_index].yaw_absolute) > angle_90 && fabs(gyro_z) > gyro_threshold_high) {
                turn_condition_met = 1;
            }
            // ����Ƿ��ǻ���
            else if (path_points[waiting_start_index+1].type == PATH_ROUNDABOUT && flag == 2) {
                turn_condition_met = 1;
            }
        }

        if (turn_condition_met) {
            // �������㣬����j��encoder_ave
            j = waiting_start_index;
            encoder_ave = path_points[j].distance;
            is_waiting_for_turn_confirmation = 0; // �˳��ȴ�״̬
            // ����ִ�к����߼��������ƫ�̵�У׼��Ч
        }
        else {
            // ���������㣬�����ȴ�����������
            normal_speed = speed_90;
            return; // �������أ��ȴ���һ������
        }
    }

    // if (flag_fast_start == 0) {
    //     flag_fast_start = 1;
    //     refresh();
    //     delay_ms(3000);
    // }
    // a = 1;
    // j ����ǰ������ʻ��·�ε�����
    if (j + 1 < path_point_count) {
        float current_segment_start_distance = (j == 0) ? 0.0f : path_points[j-1].distance;
        float current_segment_start_yaw = (j == 0) ? 0.0f : path_points[j-1].yaw_absolute;

        // ֱ����У׼ (�����ǰ·����ֱ���䣬��������������������ǰ·�Σ�ֱ�ӽ�����һ·��)
        // ע�⣺�����j+1����һ·�ε��յ㣬�����ǰ·����j����ô��һ·����j+1
        if (j + 1 < path_point_count && (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_absolute - path_points[j].yaw_absolute) > angle_90) && fabs(gyro_z) > gyro_threshold_high) {
            encoder_ave = path_points[j].distance; // ���У׼
            j++; // ������ǰ·�Σ�������һ·��
            // refresh(); // �Ƴ��˵���
        }
        // ����У׼ (�����ǰ·���ǻ�������������������������ǰ·�Σ�ֱ�ӽ�����һ·��)
        else if (j + 1 < path_point_count && path_points[j+1].type == PATH_ROUNDABOUT && flag == 2) {
            encoder_ave = path_points[j].distance; // ���У׼
            j++; // ������ǰ·�Σ�������һ·��
            // refresh(); // �Ƴ��˵���
        }
        // �����ǰ��̴ﵽ��ǰ·�ε��յ㣬�������ǻ��־λδȷ�ϣ������ȴ�״̬
        else if (encoder_ave >= path_points[j].distance) {
            uint8 should_wait = 0;
            if (j + 1 < path_point_count) { // ȷ������һ·�ο����ж�
                // �����һ·����ֱ���䣬��������δ���ƫת
                if (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_absolute - path_points[j].yaw_absolute) > angle_90 && fabs(gyro_z) < gyro_threshold_high) {
                    should_wait = 1;
                }
                // �����һ·���ǻ�������flag����2
                else if (path_points[j+1].type == PATH_ROUNDABOUT && flag != 2) {
                    should_wait = 1;
                }
            }

            if (should_wait) {
                is_waiting_for_turn_confirmation = 1;
                waiting_start_index = j; // ��¼��ǰj��ֵ
                normal_speed = speed_90; // ����ͨ��
                return; // �������أ��ȴ�ȷ��
            }
            else {
                // ����������һ·��
                j++; // �ƶ�����һ��·���㣨����һ·�ε��յ㣩
                // refresh(); // �Ƴ��˵���
            }
        }

        // ȷ�� j ����Ч��Χ�ڣ���ֹԽ�����
        // if (j < path_point_count) {
            // �����һ��·���������жϣ�ֱ���趨Ϊ����
            if (j == path_point_count - 1) { // ��������һ��·
                normal_speed = speed_high;
            }
            else {
                // speed_select ��������ʹ��ȫ�����ݼ���ֶ����ݣ�j����ǰ·�ε��յ�
                speed_select(path_points[j].type, // ��ǰ·�ε�����
                            //  (path_points[j].distance + current_segment_start_distance) / 2.0f, // middle
                            1.0f / 3.0f * current_segment_start_distance + 2.0f / 3.0f * path_points[j].distance, // 2/3
                             (path_points[j].distance - current_segment_start_distance),       // dis
                             (path_points[j].yaw_absolute - current_segment_start_yaw), // angle (��ǰ·�ε����ƫ����)
                             (j + 1 < path_point_count) ? path_points[j+1].yaw_absolute : path_points[j].yaw_absolute); // angle_next (��һ·������ȫ��ƫ���ǣ��������һ·�����õ�ǰ��)
            }
        // } else {
            // ��� j �Ѿ�������Χ����ʾ�ѵ���·��ĩβ������ֹͣ��ִ�������߼�
            // flag = 5; // ʾ�������ý�����־
        // }
    }
    else {
        if (flag_end == 0) { // ��һ�ε����յ�
            if (encoder_ave >= path_points[path_point_count - 1].distance) { // ����Ƿ񵽴����һ����¼��
                // �ƶ�����һ��·����
                j = 0; // ����jΪ0����ͷ��ʼѭ��
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
    middle = middle < 40.0f * distance_ratio ? 40.0f * distance_ratio : middle; // ������С���پ���Ϊ 40 cm
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
        // case PATH_S_TURN: // ����������
        //     normal_speed = speed_S;
        //     break;
        case PATH_ROUNDABOUT: // ����
            normal_speed = speed_low;
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
        extern_iap_write_float((float)path_points[i].yaw_absolute, 3, 1, addr);
        addr += 7;

        // �Ƿ�ת���־��0/1��
        extern_iap_write_float((float)path_points[i].type, 1, 0, addr);
        addr += 4;

        // ÿд��25�����ǿ����ת����һ��EEPROMҳ����ʼ��ַ�����ϳ�ʼƫ�ƣ�
        // i+1 �ǵ�ǰ��д��������
        if ((i + 1) % 25 == 0 && (i + 1) < path_point_count) {
            addr = 0x200 + ((i + 1) / 25) * 0x200 + 6;
        }
    }
}

void read_path(void) {
    uint16 addr = 0x200;
    uint16 i;

    /* ��ȡ·�������� */
    path_point_count = (uint16)iap_read_float(6, addr); // ȡ��ע�ͣ�ȷ����ȡ·��������
    if (path_point_count > MAX_PATH_POINTS) {
        path_point_count = MAX_PATH_POINTS;
        // �Ƴ� flag_key_control = 0; ��Ϊ��δ����
    }
    addr += 6;

    /* ��˳���ȡ��·�������� */
    for (i = 0; i < path_point_count; i++) {
        /* distance 5 �� 1 С : 9 �ֽ� */
        path_points[i].distance = iap_read_float(9, addr);
        addr += 9;

        /* yaw 3 �� 1 С : 7 �ֽ� */
        path_points[i].yaw_absolute = iap_read_float(7, addr);
        addr += 7;

        /* type 1 �� 0 С : 4 �ֽ� */
        path_points[i].type = (uint8)iap_read_float(4, addr);
        addr += 4;

        // ÿ��ȡ25�����ǿ����ת����һ��EEPROMҳ����ʼ��ַ�����ϳ�ʼƫ�ƣ�
        // i+1 �ǵ�ǰ�Ѷ�ȡ�������
        if ((i + 1) % 25 == 0 && (i + 1) < path_point_count) {
            addr = 0x200 + ((i + 1) / 25) * 0x200 + 6;
        }
    }
}
/*
// ����·�����ɺ���
void generate_test_path(void) {
    path_point_count = 29; // �趨����·���������

    path_points[0].distance = 1780.20f;
    path_points[0].yaw_absolute = 22.10f;
    path_points[0].type = 0;

    path_points[1].distance = 1994.90f;
    path_points[1].yaw_absolute = 72.00f;
    path_points[1].type = 1;

    path_points[2].distance = 2211.40f;
    path_points[2].yaw_absolute = 92.10f;
    path_points[2].type = 0;

    path_points[3].distance = 2990.20f;
    path_points[3].yaw_absolute = 166.00f;
    path_points[3].type = 1;

    path_points[4].distance = 3052.30f;
    path_points[4].yaw_absolute = 146.00f;
    path_points[4].type = 0;

    path_points[5].distance = 3809.20f;
    path_points[5].yaw_absolute = 168.00f;
    path_points[5].type = 1;

    path_points[6].distance = 4626.40f;
    path_points[6].yaw_absolute = 188.00f;
    path_points[6].type = 0;

    path_points[7].distance = 4950.20f;
    path_points[7].yaw_absolute = 309.00f;
    path_points[7].type = 1;

    path_points[8].distance = 5586.60f;
    path_points[8].yaw_absolute = 288.00f;
    path_points[8].type = 0;

    path_points[9].distance = 5860.10f;
    path_points[9].yaw_absolute = 177.00f;
    path_points[9].type = 1;

    path_points[10].distance = 6309.50f;
    path_points[10].yaw_absolute = 157.00f;
    path_points[10].type = 0;

    path_points[11].distance = 6943.00f;
    path_points[11].yaw_absolute = -87.10f;
    path_points[11].type = 1;

    path_points[12].distance = 7459.50f;
    path_points[12].yaw_absolute = -64.30f;
    path_points[12].type = 0;

    path_points[13].distance = 7981.00f;
    path_points[13].yaw_absolute = 161.00f;
    path_points[13].type = 1;

    path_points[14].distance = 9804.70f;
    path_points[14].yaw_absolute = 183.00f;
    path_points[14].type = 0;

    path_points[15].distance = 10555.00f;
    path_points[15].yaw_absolute = 437.00f;
    path_points[15].type = 1;

    path_points[16].distance = 10841.00f;
    path_points[16].yaw_absolute = 416.00f;
    path_points[16].type = 0;

    path_points[17].distance = 11619.00f;
    path_points[17].yaw_absolute = 172.00f;
    path_points[17].type = 1;

    path_points[18].distance = 12045.00f;
    path_points[18].yaw_absolute = 151.00f;
    path_points[18].type = 0;

    path_points[19].distance = 12521.00f;
    path_points[19].yaw_absolute = 19.00f;
    path_points[19].type = 1;

    path_points[20].distance = 13018.00f;
    path_points[20].yaw_absolute = 39.30f;
    path_points[20].type = 0;

    path_points[21].distance = 13316.00f;
    path_points[21].yaw_absolute = 160.00f;
    path_points[21].type = 1;

    path_points[22].distance = 14223.00f;
    path_points[22].yaw_absolute = 180.00f;
    path_points[22].type = 0;

    path_points[23].distance = 15293.00f;
    path_points[23].yaw_absolute = 327.00f;
    path_points[23].type = 1;

    path_points[24].distance = 15342.00f;
    path_points[24].yaw_absolute = 306.00f;
    path_points[24].type = 0;

    path_points[25].distance = 15608.10f; // ע����������ݣ���������һ���ܴ��ֵ
    path_points[25].yaw_absolute = 193.00f;
    path_points[25].type = 1;
    
    path_points[26].distance = 15655.00f;
    path_points[26].yaw_absolute = 213.00f;
    path_points[26].type = 0;

    path_points[27].distance = 16324.00f;
    path_points[27].yaw_absolute = 342.00f;
    path_points[27].type = 1;

    path_points[28].distance = 17706.00f;
    path_points[28].yaw_absolute = 362.00f;
    path_points[28].type = 0;
}

void verify_eeprom_storage(void) {
    uint16 i;

    // 1. ����ǰ�ڴ��е� path_points ����д��EEPROM
    write_path();

    // 2. �����ڴ��е� path_points ���飬ȷ����ȡ����EEPROM�е�����
    for (i = 0; i < MAX_PATH_POINTS; i++) {
        path_points[i].distance = 0.0f;
        path_points[i].yaw_absolute = 0.0f;
        path_points[i].type = 0;
    }
    path_point_count = 0; // ���õ����

    // 3. ��EEPROM��������
    read_path();

    // 4. ��ӡ��EEPROM���ص�����
    printf("--- Data read from EEPROM ---\n");
    printf("Path Point Count: %d\n", path_point_count);
    for (i = 0; i < path_point_count; i++) {
        printf("Point %d: Distance=%.2f, Yaw=%.2f, Type=%d\n",
               i, path_points[i].distance, path_points[i].yaw_absolute, path_points[i].type);
    }
    printf("-----------------------------\n");
}
*/