#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

// ·�����������С
#define MAX_PATH_POINTS 666

typedef struct {
    float yaw_absolute; // ת�䴦�ľ��Ժ���� (��)
} PathPoint;

// �������� - ���ڱ�����170=30cm�ı���
#define DISTANCE_RATIO (170.0f / 30.0f)        // ������������5.67
#define DISTANCE_SEG    6.0f
#define SAMPLE_DISTANCE (DISTANCE_SEG * DISTANCE_RATIO) // 6cm���������Լ34������ֵ

// ·����¼״̬�궨�壬��ʡ�ڴ�
#define RECORD_SAMPLING 0    // �����׶�
#define RECORD_COMPLETE 1    // ��¼���

extern PathPoint path_points[MAX_PATH_POINTS];

extern uint16 path_point_count;
extern uint16 j; // ���ڻط�ʱ��·��������

extern uint8 flag_end; // 0��ʾδ�����յ�,1��ʾ�����յ�
extern uint8 send_flag_nav;
extern uint8 send_curvature_flag; // ���ڷ�����������
extern uint8 flag_fast_start;

// �ٶ�����
extern float speed_high;
extern float speed_low;
extern float speed_90;
extern float speed_S;

// ��������
void Path_record(void);
void fast_tracking(void);
void refresh(void);
void write_path(void);
void read_path(void);

// ���ʷ�����غ���
float calculate_simple_angle_ratio(float yaw1, float yaw2);
float angle_ratio_to_speed(float ratio);

#endif /* __NAVIGATION_H__ */
