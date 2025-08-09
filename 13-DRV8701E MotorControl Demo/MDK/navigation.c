#include "headfile.h"

PathPoint path_points[MAX_PATH_POINTS] = {0};

uint16 path_point_count = 0; // 第一个点为起点已经初始化为0,0
uint16 j = 1; // 用于移动路径点的索引
uint8 s_turn_active = 0; // 新增：S弯激活标志，用于判断是否处于可能升级为S弯的普通弯道中
uint16 s_turn_start_point_index = 0; // 新增：S弯起始点的索引
uint8 s_turn_start_yaw_direction = 2; // 新增：S弯起始点的yaw方向 (0:左转/负yaw, 1:右转/正yaw, 2:未定义/重置)
uint8 current_path_type = PATH_STRAIGHT; // 新增：当前路段类型
uint8 last_flag = 0; // 新增：用于存储上一个周期的flag值
// uint16 path_point_count_threshold = 100; // 路径点阈值
uint8 flag_end = 0; // 0表示未到达终点,1表示到达终点
uint8 send_flag_nav = 0;
uint8 flag_fast_start = 0;

float speed_high = 320;
float speed_low = 230;
float speed_90 = 160;
float speed_S = 200;

// uint8 a = 0;

void Path_record(void) {
    uint16 i;
    // 环岛判断
    if (flag == 2 && last_flag != 2) { // 第一次进入环岛
        current_path_type = PATH_ROUNDABOUT;
        Point_record();
    }
    else if (flag == 2) { // 正在环岛中
        // 不记录任何点
    }
    else { // 非环岛模式
        // 检查是否从直道进入弯道
        if (current_path_type == PATH_STRAIGHT && fabs(gyro_z) > gyro_threshold_high && fabs(yaw) > angle_turn && encoder_ave > distance_min) {
            current_path_type = PATH_TURN; // 先判断为普通弯道
            Point_record();
            s_turn_active = 1; // 激活S弯检测
            s_turn_start_point_index = path_point_count; // 记录S弯起始点的索引
            s_turn_start_yaw_direction = (yaw > 0) ? 1 : 0; // 记录S弯起始点的yaw方向 (1:正yaw/右转, 0:负yaw/左转)
        }
        // 检查是否从弯道进入直道 (S弯结束条件也一样)
        else if ((current_path_type == PATH_TURN || current_path_type == PATH_S_TURN) && fabs(gyro_z) < gyro_threshold_low && encoder_ave > distance_min) {
            current_path_type = PATH_STRAIGHT; // 进入直道
            Point_record();
            s_turn_active = 0; // 退出S弯检测
            s_turn_start_point_index = 0; // 重置S弯起始点索引
            s_turn_start_yaw_direction = 2; // 重置S弯起始点yaw方向为未定义
        }
        // S弯升级判断：如果当前是普通弯道，且yaw变化超过angle_180，或者当前yaw方向与起始yaw方向相反，则升级为S弯
        else if (s_turn_active && current_path_type == PATH_TURN && (fabs(yaw) >= angle_180 || 
                 ((yaw > 0 && s_turn_start_yaw_direction == 0) || (yaw < 0 && s_turn_start_yaw_direction == 1)))) {
            // 遍历从S弯起始点到当前点，将所有点的类型更新为PATH_S_TURN
            for (i = s_turn_start_point_index; i <= path_point_count; i++)
                path_points[i].type = PATH_S_TURN;
            current_path_type = PATH_S_TURN; // 更新当前路段类型
            // 不需要重新记录点，只是修改了已记录点的类型
        }
        else if (s_turn_active && current_path_type == PATH_S_TURN) {
            // 持续在S弯中，不记录新点，直到出S弯
        }
        // 其他情况，不记录新点
    }
    last_flag = flag;
}

void Point_record(void) {
    path_point_count++;
    path_points[path_point_count].distance = encoder_ave;
    path_points[path_point_count].yaw_relative = yaw;
    path_points[path_point_count].type = current_path_type; // 使用新的枚举类型

    // send_flag_nav = 1;

    refresh(); // 恢复此调用，清零编码器和yaw，为下一段路程做准备
    // flag_type = !flag_type; // 不再需要，由 current_path_type 管理
}

// 快速循迹函数 (第二圈使用)
void fast_tracking(void) {
    // if (flag_fast_start == 0) {
    //     flag_fast_start = 1;
    //     refresh();
    //     delay_ms(3000);
    // }
    // a = 1;
    if (j + 1 < path_point_count) {
        // 直角弯校准
        if (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_relative) > angle_90 && fabs(gyro_z) > gyro_threshold_high) {
            j++;
            refresh();
        }
        // 环岛校准
        else if (path_points[j+1].type == PATH_ROUNDABOUT && flag == 2) {
            j++;
            refresh();
        }
        else if (encoder_ave >= path_points[j].distance) {
            // 移动到下一个路径点
            j++;
            refresh(); // 清零编码器和yaw，开始下一段的距离测量
        }
        // speed_select 函数现在可以直接使用分段数据
        speed_select(path_points[j].type, path_points[j].distance / 2,
                     path_points[j].distance, path_points[j].yaw_relative, path_points[j + 1].yaw_relative);
    }
    else {
        if (flag_end == 0) { // 第一次到达终点
            if (encoder_ave >= path_points[j].distance) {
                // 移动到下一个路径点
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
        case PATH_STRAIGHT: // 直道
            if (dis > distance_long) { // 长直线
                if (encoder_ave < middle) normal_speed = speed_high;
                else {
                    if (fabs(angle_next) >= angle_90) normal_speed = speed_90;
                    else normal_speed = speed_low;
                }
            }
            else { // 短直道
                if (fabs(angle_next) >= angle_90) normal_speed = speed_90;
                else normal_speed = speed_low;
            }
            break;
        case PATH_TURN: // 普通弯道
            if (fabs(angle) >= angle_90) normal_speed = speed_90; // 直角
            else normal_speed = speed_low; // 小折线
            break;
        case PATH_S_TURN: // 连续几字弯
            normal_speed = speed_S;
            break;
        case PATH_ROUNDABOUT: // 环岛
            normal_speed = speed_high;
            break;
        default:
            normal_speed = speed_low; // 默认速度
            break;
    }
}

void refresh(void) {
    encoder_clear();
    yaw = 0;
}

void write_path(void) {
    // 将路径数据写入 EEPROM，起始地址选择 0x200，避免与 my_peripheral.c 中参数区冲突
    uint16 addr = 0x200;
    uint16 page_addr;
    uint16 i;

    // 计算最多可能用到的页并提前擦除，最多 500 个点，大约 10 kB
    // 循环擦除 EEPROM 页面
    for (page_addr = 0x200; page_addr <= 0x2600; page_addr += 0x200)
        iap_erase_page(page_addr);

    /*
     * 先存储路径点数量，方便读取复原
     * 采用 extern_iap_write_float 写入：整数位 3 位，小数 0 位
     * 长度 = num + pointnum + 3 = 3 + 0 + 3 = 6 字节
     */
    extern_iap_write_float((float)path_point_count, 3, 0, addr);
    addr += 6;

    /*
     * 依次写入各个路径点：
     * distance : 整数 5 位，小数 1 位  -> 5 + 1 + 3 = 9 字节
     * yaw      : 整数 3 位，小数 1 位  -> 3 + 1 + 3 = 7 字节
     * type     : 整数 1 位，小数 0 位  -> 1 + 0 + 3 = 4 字节
     * 总计每点 20 字节
     */
    for (i = 0; i < path_point_count; i++) {
        // 距离
        extern_iap_write_float((float)path_points[i].distance, 5, 1, addr);
        addr += 9;

        // 角度
        extern_iap_write_float((float)path_points[i].yaw_relative, 3, 1, addr);
        addr += 7;

        // 是否转弯标志（0/1）
        extern_iap_write_float((float)path_points[i].type, 1, 0, addr);
        addr += 4;
    }
}

void read_path(void) {
    uint16 addr = 0x200;
    uint16 i;

    /* 读取路径点数量 */
    // path_point_count = (uint16)iap_read_float(6, addr);
    if (path_point_count > MAX_PATH_POINTS) {
        path_point_count = MAX_PATH_POINTS;
        flag_key_control = 0;
    }
    addr += 6;

    /* 按顺序读取各路径点数据 */
    for (i = 0; i < path_point_count; i++) {
        /* distance 5 整 1 小 : 9 字节 */
        path_points[i].distance = iap_read_float(9, addr);
        addr += 9;

        /* yaw 3 整 1 小 : 7 字节 */
        path_points[i].yaw_relative = iap_read_float(7, addr);
        addr += 7;

        /* type 1 整 0 小 : 4 字节 */
        path_points[i].type = (uint8)iap_read_float(4, addr);
        addr += 4;
    }
}
