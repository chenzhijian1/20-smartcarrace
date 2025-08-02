#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

// ·����ṹ��
typedef struct {
    float distance;     // ·�ξ��� (����������ֵ)
    float yaw_relative; // ת�䴦�ľ��Ժ���� (��)
    uint8 isturn;       // �Ƿ�Ϊת���
} PathPoint;

// ·����������
#define MAX_PATH_POINTS 270
extern PathPoint path_points[MAX_PATH_POINTS];

extern uint16 path_point_count;
extern uint16 j;
extern uint16 path_point_count_threshold; // ·���������ֵ������
extern uint8 flag_isturn;
extern uint8 flag_end;
extern uint8 send_flag_nav;
extern uint8 flag_fast_start;

extern uint8 a;

#define gyro_threshold_high 100
#define gyro_threshold_low 10

#define angle_turn 10
#define angle_90 70

#define err_straight 15
#define err_turn 30

#define distance_threshold 50 * distance_ratio  // ·���������ֵ�������ж��Ƿ�Ϊ��ֱ��
#define distance_min 10 * distance_ratio  // ��С·������룬����ɸ���̵ܶ�·����
#define distance_ratio 134 / 30  // ����������ֵ��ʵ�ʾ���ı���ϵ��(����ֵ/ʵ�ʾ���cm)

extern float speed_high;
extern float speed_low;
extern float speed_turn;
extern float speed_adjust;

// ��������
void Path_record(void);
void Point_record(void);
void print_point();
void fast_tracking(void);
void speed_select(uint8 f_isturn, float middle, float dis,float angle, float angle_next);

void refresh(void);

extern void read_path(void);
extern void write_path(void);

#endif /* __NAVIGATION_H__ */
