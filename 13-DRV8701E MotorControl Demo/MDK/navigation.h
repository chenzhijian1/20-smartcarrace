#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

// ·�����ͺ궨��
#define PATH_STRAIGHT    0      // ֱ��
#define PATH_TURN        1      // ����/��ͨ���
// #define PATH_S_TURN      2      // ����������
#define PATH_ROUNDABOUT  2      // ����


typedef struct {
    float distance;     // ·�ξ��� (����������ֵ)
    float yaw_absolute; // ת�䴦�ľ��Ժ���� (��)
    uint8 type;         // ·������
} PathPoint;

// ·����������
#define MAX_PATH_POINTS 300
extern PathPoint path_points[MAX_PATH_POINTS];

extern uint16 path_point_count;
extern uint16 j;
extern uint8 s_turn_active; // ������S�伤���־�������ж��Ƿ��ڿ�������ΪS�����ͨ�����
extern uint16 s_turn_start_point_index; // ������S����ʼ�������
extern uint8 s_turn_start_yaw_direction; // ������S����ʼ���yaw���� (0:��ת/��yaw, 1:��ת/��yaw)
extern uint8 last_flag; // ���������ڴ洢��һ�����ڵ�flagֵ

// extern uint16 path_point_count_threshold; // ·���������ֵ������
extern uint8 current_path_type; // ��������ǰ·������
extern uint8 flag_end; // 0��ʾδ�����յ�,1��ʾ�����յ�
extern uint8 send_flag_nav;
extern uint8 flag_fast_start;

// extern uint8 a;

#define gyro_threshold_high 150
#define gyro_threshold_low 40

#define angle_turn 15
#define angle_90 70
#define angle_180 150

#define err_straight 15
#define err_turn 30

#define distance_long 80 * distance_ratio  // ·���������ֵ�������ж��Ƿ�Ϊ��ֱ��
#define distance_min 5 * distance_ratio  // ��С·������룬����ɸ���̵ܶ�·����
#define distance_ratio 170 / 30  // ����������ֵ��ʵ�ʾ���ı���ϵ��(����ֵ/ʵ�ʾ���cm)

extern float speed_high;
extern float speed_low;
extern float speed_90;
extern float speed_S;

// ��������
void Path_record(void);
void Point_record(void);
void print_point();
void fast_tracking(void);
void speed_select(uint8 type, float middle, float dis,float angle, float angle_next);

void refresh(void);

void read_path(void);
void write_path(void);
void generate_test_path(void); // ����������·�����ɺ���
void verify_eeprom_storage(void); // ������EEPROM�洢��֤����

#endif /* __NAVIGATION_H__ */
