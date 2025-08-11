#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

// 路段类型宏定义
#define PATH_STRAIGHT    0      // 直道
#define PATH_TURN        1      // 折线/普通弯道
// #define PATH_S_TURN      2      // 连续几字弯
#define PATH_ROUNDABOUT  2      // 环岛


typedef struct {
    float distance;     // 路段距离 (编码器计数值)
    float yaw_absolute; // 转弯处的绝对航向角 (°)
    uint8 type;         // 路段类型
} PathPoint;

// 路径记忆数组
#define MAX_PATH_POINTS 300
extern PathPoint path_points[MAX_PATH_POINTS];

extern uint16 path_point_count;
extern uint16 j;
extern uint8 s_turn_active; // 新增：S弯激活标志，用于判断是否处于可能升级为S弯的普通弯道中
extern uint16 s_turn_start_point_index; // 新增：S弯起始点的索引
extern uint8 s_turn_start_yaw_direction; // 新增：S弯起始点的yaw方向 (0:左转/负yaw, 1:右转/正yaw)
extern uint8 last_flag; // 新增：用于存储上一个周期的flag值

// extern uint16 path_point_count_threshold; // 路径点计数阈值调试用
extern uint8 current_path_type; // 新增：当前路段类型
extern uint8 flag_end; // 0表示未到达终点,1表示到达终点
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

#define distance_long 80 * distance_ratio  // 路径点距离阈值，用于判断是否为长直道
#define distance_min 5 * distance_ratio  // 最小路径点距离，用于筛除很短的路径点
#define distance_ratio 170 / 30  // 编码器计数值与实际距离的比例系数(计数值/实际距离cm)

extern float speed_high;
extern float speed_low;
extern float speed_90;
extern float speed_S;

// 函数声明
void Path_record(void);
void Point_record(void);
void print_point();
void fast_tracking(void);
void speed_select(uint8 type, float middle, float dis,float angle, float angle_next);

void refresh(void);

void read_path(void);
void write_path(void);
void generate_test_path(void); // 新增：测试路径生成函数
void verify_eeprom_storage(void); // 新增：EEPROM存储验证函数

#endif /* __NAVIGATION_H__ */
