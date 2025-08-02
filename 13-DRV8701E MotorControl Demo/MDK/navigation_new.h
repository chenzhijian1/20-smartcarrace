#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "headfile.h"

// #define MAX_PATH_POINTS 285

extern int32 distance_memory[5];
extern float yaw_memory[5];

extern uint16 index; // 记录已保存的总点数

/**
  * @brief  保存指定段的导航数据到AT24C EEPROM
  * @param  segment_index: 要保存的段的索引 (从0开始)
  * @param  distances: 包含要保存的距离数据的数组指针
  * @param  yaws: 包含要保存的偏航角数据的数组指针
  * @param  num_points_in_segment: 当前段中实际要保存的点数 (最大为POINTS_PER_SEGMENT)
  * @retval 无
  */
void save_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_in_segment);

/**
  * @brief  从AT24C EEPROM读取指定段的导航数据
  * @param  segment_index: 要读取的段的索引 (从0开始)
  * @param  distances: 用于存储读取到的距离数据的数组指针
  * @param  yaws: 用于存储读取到的偏航角数据的数组指针
  * @param  num_points_to_read: 当前段中实际要读取的点数 (最大为POINTS_PER_SEGMENT)
  * @retval 无
  */
void read_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_to_read);

/**
  * @brief  保存总点数index到AT24C EEPROM的0x0000地址
  * @param  无
  * @retval 无
  */
void save_navigation_index(void);

/**
  * @brief  从AT24C EEPROM的0x0000地址读取总点数index
  * @param  无
  * @retval 无
  */
void read_navigation_index(void);

void navigation_memory_test(void);

#endif /* __NAVIGATION_H__ */
