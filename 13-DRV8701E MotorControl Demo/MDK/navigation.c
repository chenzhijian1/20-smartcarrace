#include "headfile.h"

PathPoint path_points[MAX_PATH_POINTS] = {0};

uint16 path_point_count = 0; // 第一个点为起点已经初始化为0,0
uint16 j = 0; // 用于移动路径点的索引
// uint8 s_turn_active = 0; // 新增：S弯激活标志，用于判断是否处于可能升级为S弯的普通弯道中
// uint16 s_turn_start_point_index = 0; // 新增：S弯起始点的索引
// uint8 s_turn_start_yaw_direction = 2; // 新增：S弯起始点的yaw方向 (0:左转/负yaw, 1:右转/正yaw, 2:未定义/重置)
uint8 current_path_type = PATH_STRAIGHT; // 新增：当前路段类型
uint8 last_flag = 0; // 新增：用于存储上一个周期的flag值
// uint16 path_point_count_threshold = 100; // 路径点阈值
uint8 flag_end = 0; // 0表示未到达终点,1表示到达终点
uint8 send_flag_nav = 0;
uint8 flag_fast_start = 0;

static uint8 is_waiting_for_turn_confirmation = 0; // 新增：等待转弯确认标志
static uint16 waiting_start_index = 0; // 新增：等待状态开始时的路径点索引

float speed_high = 600;
float speed_low = 450;
float speed_90 = 350;
float speed_S = 200;

// uint8 a = 0;

void Path_record(void) {
    float prev_yaw_for_comparison; // 声明在函数开头

    // 环岛判断
    if (flag == 2 && last_flag != 2) { // 第一次进入环岛
        Point_record();
        current_path_type = PATH_ROUNDABOUT;
    }
    else if (flag == 2) { // 正在环岛中
        // 不记录任何点
    }
    else { // 非环岛模式
        // 检查是否从直道进入弯道
        // 使用当前yaw与上一个路径点yaw的差值来判断转弯
        prev_yaw_for_comparison = (path_point_count == 0) ? 0.0f : path_points[path_point_count - 1].yaw_absolute;
        if (current_path_type == PATH_STRAIGHT && fabs(gyro_z) > gyro_threshold_high && fabs(yaw - prev_yaw_for_comparison) > angle_turn && encoder_ave > distance_min) {
            Point_record();
            current_path_type = PATH_TURN; // 先判断为普通弯道
            // s_turn_active = 1; // 激活S弯检测
            // s_turn_start_point_index = path_point_count; // 记录S弯起始点的索引
            // s_turn_start_yaw_direction = (yaw > 0) ? 1 : 0; // 记录S弯起始点的yaw方向 (1:正yaw/右转, 0:负yaw/左转)
        }
        // 检查是否从弯道进入直道 (S弯结束条件也一样)
        else if (current_path_type == PATH_TURN && fabs(gyro_z) < gyro_threshold_low && encoder_ave > distance_min) { // 移除了 || current_path_type == PATH_S_TURN
            Point_record();
            current_path_type = PATH_STRAIGHT; // 进入直道
            // s_turn_active = 0; // 退出S弯检测
            // s_turn_start_point_index = 0; // 重置S弯起始点索引
            // s_turn_start_yaw_direction = 2; // 重置S弯起始点yaw方向为未定义
        }
        // S弯升级判断：如果当前是普通弯道，且yaw变化超过angle_180，或者当前yaw方向与起始yaw方向相反，则升级为S弯
        // else if (s_turn_active && current_path_type == PATH_TURN && (fabs(yaw) >= angle_180 || 
        //          ((yaw > angle_90 && s_turn_start_yaw_direction == 0) || (yaw < -angle_90 && s_turn_start_yaw_direction == 1)))) {
        //     // 遍历从S弯起始点到当前点，将所有点的类型更新为PATH_S_TURN
        //     for (i = s_turn_start_point_index; i <= path_point_count; i++)
        //         path_points[i].type = PATH_S_TURN;
        //     current_path_type = PATH_S_TURN; // 更新当前路段类型
        //     // 不需要重新记录点，只是修改了已记录点的类型
        // }
        // else if (s_turn_active && current_path_type == PATH_S_TURN) {
        //     // 持续在S弯中，不记录新点，直到出S弯
        // }
        // 其他情况，不记录新点
    }
    last_flag = flag;
}

void Point_record(void) {
    path_points[path_point_count].distance = encoder_ave;
    path_points[path_point_count].yaw_absolute = yaw;
    path_points[path_point_count].type = current_path_type; // 使用新的枚举类型
    path_point_count++; // 先赋值再自增，使path_points[0]存储第一个点

    send_flag_nav = 1;

    // refresh(); // 移除此调用，实现全局里程记忆
    // flag_type = !flag_type; // 不再需要，由 current_path_type 管理
}

// 快速循迹函数 (第二圈使用)
void fast_tracking(void) {
    // 处理里程偏大等待确认状态
    if (is_waiting_for_turn_confirmation) {
        // 检查是否满足转弯或环岛条件
        uint8 turn_condition_met = 0;
        if (waiting_start_index + 1 < path_point_count) {
            // 检查是否是直角弯
            if (path_points[waiting_start_index+1].type == PATH_TURN && fabs(path_points[waiting_start_index+1].yaw_absolute - path_points[waiting_start_index].yaw_absolute) > angle_90 && fabs(gyro_z) > gyro_threshold_high) {
                turn_condition_met = 1;
            }
            // 检查是否是环岛
            else if (path_points[waiting_start_index+1].type == PATH_ROUNDABOUT && flag == 2) {
                turn_condition_met = 1;
            }
        }

        if (turn_condition_met) {
            // 条件满足，回退j和encoder_ave
            j = waiting_start_index;
            encoder_ave = path_points[j].distance;
            is_waiting_for_turn_confirmation = 0; // 退出等待状态
            // 继续执行后续逻辑，让里程偏短的校准生效
        }
        else {
            // 条件不满足，继续等待，保持慢速
            normal_speed = speed_90;
            return; // 立即返回，等待下一个周期
        }
    }

    // if (flag_fast_start == 0) {
    //     flag_fast_start = 1;
    //     refresh();
    //     delay_ms(3000);
    // }
    // a = 1;
    // j 代表当前正在行驶的路段的索引
    if (j + 1 < path_point_count) {
        float current_segment_start_distance = (j == 0) ? 0.0f : path_points[j-1].distance;
        float current_segment_start_yaw = (j == 0) ? 0.0f : path_points[j-1].yaw_absolute;

        // 直角弯校准 (如果当前路段是直角弯，且满足条件，则跳过当前路段，直接进入下一路段)
        // 注意：这里的j+1是下一路段的终点，如果当前路段是j，那么下一路段是j+1
        if (j + 1 < path_point_count && (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_absolute - path_points[j].yaw_absolute) > angle_90) && fabs(gyro_z) > gyro_threshold_high) {
            encoder_ave = path_points[j].distance; // 里程校准
            j++; // 跳过当前路段，进入下一路段
            // refresh(); // 移除此调用
        }
        // 环岛校准 (如果当前路段是环岛，且满足条件，则跳过当前路段，直接进入下一路段)
        else if (j + 1 < path_point_count && path_points[j+1].type == PATH_ROUNDABOUT && flag == 2) {
            encoder_ave = path_points[j].distance; // 里程校准
            j++; // 跳过当前路段，进入下一路段
            // refresh(); // 移除此调用
        }
        // 如果当前里程达到当前路段的终点，但陀螺仪或标志位未确认，则进入等待状态
        else if (encoder_ave >= path_points[j].distance) {
            uint8 should_wait = 0;
            if (j + 1 < path_point_count) { // 确保有下一路段可以判断
                // 如果下一路段是直角弯，但陀螺仪未大幅偏转
                if (path_points[j+1].type == PATH_TURN && fabs(path_points[j+1].yaw_absolute - path_points[j].yaw_absolute) > angle_90 && fabs(gyro_z) < gyro_threshold_high) {
                    should_wait = 1;
                }
                // 如果下一路段是环岛，但flag不是2
                else if (path_points[j+1].type == PATH_ROUNDABOUT && flag != 2) {
                    should_wait = 1;
                }
            }

            if (should_wait) {
                is_waiting_for_turn_confirmation = 1;
                waiting_start_index = j; // 记录当前j的值
                normal_speed = speed_90; // 慢速通过
                return; // 立即返回，等待确认
            }
            else {
                // 正常进入下一路段
                j++; // 移动到下一个路径点（即下一路段的终点）
                // refresh(); // 移除此调用
            }
        }

        // 确保 j 在有效范围内，防止越界访问
        // if (j < path_point_count) {
            // 对最后一段路进行特殊判断，直接设定为高速
            if (j == path_point_count - 1) { // 如果是最后一段路
                normal_speed = speed_high;
            }
            else {
                // speed_select 函数现在使用全局数据计算分段数据，j代表当前路段的终点
                speed_select(path_points[j].type, // 当前路段的类型
                            //  (path_points[j].distance + current_segment_start_distance) / 2.0f, // middle
                            1.0f / 3.0f * current_segment_start_distance + 2.0f / 3.0f * path_points[j].distance, // 2/3
                             (path_points[j].distance - current_segment_start_distance),       // dis
                             (path_points[j].yaw_absolute - current_segment_start_yaw), // angle (当前路段的相对偏航角)
                             (j + 1 < path_point_count) ? path_points[j+1].yaw_absolute : path_points[j].yaw_absolute); // angle_next (下一路段起点的全局偏航角，如果无下一路段则用当前点)
            }
        // } else {
            // 如果 j 已经超出范围，表示已到达路径末尾，可以停止或执行其他逻辑
            // flag = 5; // 示例：设置结束标志
        // }
    }
    else {
        if (flag_end == 0) { // 第一次到达终点
            if (encoder_ave >= path_points[path_point_count - 1].distance) { // 检查是否到达最后一个记录点
                // 移动到下一个路径点
                j = 0; // 重置j为0，从头开始循迹
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
    middle = middle < 40.0f * distance_ratio ? 40.0f * distance_ratio : middle; // 限制最小减速距离为 40 cm
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
        // case PATH_S_TURN: // 连续几字弯
        //     normal_speed = speed_S;
        //     break;
        case PATH_ROUNDABOUT: // 环岛
            normal_speed = speed_low;
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
        extern_iap_write_float((float)path_points[i].yaw_absolute, 3, 1, addr);
        addr += 7;

        // 是否转弯标志（0/1）
        extern_iap_write_float((float)path_points[i].type, 1, 0, addr);
        addr += 4;

        // 每写入25个点后，强制跳转到下一个EEPROM页的起始地址（加上初始偏移）
        // i+1 是当前已写入点的数量
        if ((i + 1) % 25 == 0 && (i + 1) < path_point_count) {
            addr = 0x200 + ((i + 1) / 25) * 0x200 + 6;
        }
    }
}

void read_path(void) {
    uint16 addr = 0x200;
    uint16 i;

    /* 读取路径点数量 */
    path_point_count = (uint16)iap_read_float(6, addr); // 取消注释，确保读取路径点数量
    if (path_point_count > MAX_PATH_POINTS) {
        path_point_count = MAX_PATH_POINTS;
        // 移除 flag_key_control = 0; 因为其未定义
    }
    addr += 6;

    /* 按顺序读取各路径点数据 */
    for (i = 0; i < path_point_count; i++) {
        /* distance 5 整 1 小 : 9 字节 */
        path_points[i].distance = iap_read_float(9, addr);
        addr += 9;

        /* yaw 3 整 1 小 : 7 字节 */
        path_points[i].yaw_absolute = iap_read_float(7, addr);
        addr += 7;

        /* type 1 整 0 小 : 4 字节 */
        path_points[i].type = (uint8)iap_read_float(4, addr);
        addr += 4;

        // 每读取25个点后，强制跳转到下一个EEPROM页的起始地址（加上初始偏移）
        // i+1 是当前已读取点的数量
        if ((i + 1) % 25 == 0 && (i + 1) < path_point_count) {
            addr = 0x200 + ((i + 1) / 25) * 0x200 + 6;
        }
    }
}
/*
// 测试路径生成函数
void generate_test_path(void) {
    path_point_count = 29; // 设定测试路径点的数量

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

    path_points[25].distance = 15608.10f; // 注意这里的数据，看起来像一个很大的值
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

    // 1. 将当前内存中的 path_points 数组写入EEPROM
    write_path();

    // 2. 清零内存中的 path_points 数组，确保读取的是EEPROM中的数据
    for (i = 0; i < MAX_PATH_POINTS; i++) {
        path_points[i].distance = 0.0f;
        path_points[i].yaw_absolute = 0.0f;
        path_points[i].type = 0;
    }
    path_point_count = 0; // 重置点计数

    // 3. 从EEPROM读回数据
    read_path();

    // 4. 打印从EEPROM读回的数据
    printf("--- Data read from EEPROM ---\n");
    printf("Path Point Count: %d\n", path_point_count);
    for (i = 0; i < path_point_count; i++) {
        printf("Point %d: Distance=%.2f, Yaw=%.2f, Type=%d\n",
               i, path_points[i].distance, path_points[i].yaw_absolute, path_points[i].type);
    }
    printf("-----------------------------\n");
}
*/