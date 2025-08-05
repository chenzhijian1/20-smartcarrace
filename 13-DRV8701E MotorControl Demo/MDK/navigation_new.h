#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

#define MAX_POINTS 200

extern int32 distance_memory[MAX_POINTS];
extern float yaw_memory[MAX_POINTS];

extern uint16 total_points_saved; // 记录已保存的总点数 (在记录模式下使用，最终会保存到Flash)
extern uint16 current_car_index; // 记录当前小车在整个路径中的索引 (在回放模式下使用)
extern uint16 flash_read_pointer; // 记录Flash中下一个待读取的点的索引 (在回放模式下使用)

// /**
//   * @brief  保存指定段的导航数据到AT24C EEPROM
//   * @param  segment_index: 要保存的段的索引 (从0开始)
//   * @param  distances: 包含要保存的距离数据的数组指针
//   * @param  yaws: 包含要保存的偏航角数据的数组指针
//   * @param  num_points_in_segment: 当前段中实际要保存的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void save_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_in_segment);

// /**
//   * @brief  从AT24C EEPROM读取指定段的导航数据
//   * @param  segment_index: 要读取的段的索引 (从0开始)
//   * @param  distances: 用于存储读取到的距离数据的数组指针
//   * @param  yaws: 用于存储读取到的偏航角数据的数组指针
//   * @param  num_points_to_read: 当前段中实际要读取的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void read_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_to_read);

// /**
//   * @brief  保存总点数index到AT24C EEPROM的0x0000地址
//   * @param  无
//   * @retval 无
//   */
// void save_navigation_index(void);

// /**
//   * @brief  从AT24C EEPROM的0x0000地址读取总点数index
//   * @param  无
//   * @retval 无
//   */
// void read_navigation_index(void);

// void navigation_memory_test(void);

// --- 新增W25Q128相关定义和声明 ---
#include "my_spi.h" // 包含my_spi硬件驱动头文件

// 每个导航数据点的大小 (int32距离 + float偏航角 = 4 + 4 = 8字节)
#define NAVIGATION_POINT_SIZE 8 
// Flash中存储总点数的地址 (使用扇区0的起始地址)
#define TOTAL_POINTS_ADDR 0x000000 
// 导航数据在Flash中的起始地址 (避开总点数存储地址，从下一个扇区开始，即4KB)
#define NAVIGATION_DATA_FLASH_START_ADDR 0x001000 

// Flash操作函数 (内部使用，不对外暴露)
void save_navigation_point_to_flash(int32 distance, float yaw, uint16 point_index);
void read_navigation_point_from_flash(int32 *distance, float *yaw, uint16 point_index);
void save_total_points_to_flash(uint16 total_points);
void read_total_points_from_flash(uint16 *total_points);

// 慢速循迹 (记录模式) API
void navigation_start_record_mode(void);
void navigation_record_point(int32 distance, float yaw);
void navigation_stop_record_mode(void);

// 快速循迹 (回放模式) API
void navigation_start_playback_mode(void);
void navigation_update_current_index(void); // 更新小车当前索引，并触发数据预加载
void navigation_get_current_point(int32 *distance, float *yaw); // 获取当前点数据
void navigation_get_lookahead_point(int32 *distance, float *yaw, uint16 lookahead_offset); // 获取前瞻点数据
void navigation_memory_test(void);

#endif /* __NAVIGATION_H__ */
