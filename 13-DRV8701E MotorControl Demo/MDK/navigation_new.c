#include "headfile.h"

int32 distance_memory[5]; // 实际数组大小可能大于5，此处仅为示例
float yaw_memory[5];      // 实际数组大小可能大于5，此处仅为示例

uint16 index = 0; // 记录已保存的总点数

// 导航数据在EEPROM中的起始地址（在index之后）
#define NAVIGATION_DATA_START_ADDR 0x0002
// 单个数据点（距离 + 偏航角）的大小
#define NAVIGATION_POINT_SIZE (sizeof(int32) + sizeof(float))

// 定义每个导航数据段包含的点数
#define POINTS_PER_SEGMENT 200








/**
  * @brief  保存指定段的导航数据到AT24C EEPROM
  * @param  segment_index: 要保存的段的索引 (从0开始)
  * @param  distances: 包含要保存的距离数据的数组指针
  * @param  yaws: 包含要保存的偏航角数据的数组指针
  * @param  num_points_in_segment: 当前段中实际要保存的点数 (最大为POINTS_PER_SEGMENT)
  * @retval 无
  */
void save_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_in_segment) {
    // 计算当前段在EEPROM中的起始地址
    uint16 current_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
    uint16 i;

    for (i = 0; i < num_points_in_segment; i++) {
        // 保存距离数据
        AT24C_Write(current_address, (uint8 *)&distances[i], sizeof(int32));
        current_address += sizeof(int32);

        // 保存偏航角数据
        AT24C_Write(current_address, (uint8 *)&yaws[i], sizeof(float));
        current_address += sizeof(float);
    }
}

/**
  * @brief  从AT24C EEPROM读取指定段的导航数据
  * @param  segment_index: 要读取的段的索引 (从0开始)
  * @param  distances: 用于存储读取到的距离数据的数组指针
  * @param  yaws: 用于存储读取到的偏航角数据的数组指针
  * @param  num_points_to_read: 当前段中实际要读取的点数 (最大为POINTS_PER_SEGMENT)
  * @retval 无
  */
void read_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_to_read)
{
    // 计算当前段在EEPROM中的起始地址
    uint16 current_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
    uint16 i;

    for (i = 0; i < num_points_to_read; i++) {
        // 读取距离数据
        AT24C_Read(current_address, (uint8 *)&distances[i], sizeof(int32));
        current_address += sizeof(int32);

        // 读取偏航角数据
        AT24C_Read(current_address, (uint8 *)&yaws[i], sizeof(float));
        current_address += sizeof(float);
    }
}

/**
  * @brief  保存总点数index到AT24C EEPROM的0x0000地址
  * @param  无
  * @retval 无
  */
void save_navigation_index(void)
{
    AT24C_Write(0x0000, (uint8 *)&index, sizeof(index));
}

/**
  * @brief  从AT24C EEPROM的0x0000地址读取总点数index
  * @param  无
  * @retval 无
  */
void read_navigation_index(void)
{
    AT24C_Read(0x0000, (uint8 *)&index, sizeof(index));
}

void navigation_memory_test(void) {
    uint8 i;
    for (i = 0; i < 200; i++) {
        distance_memory[i] = i;
        yaw_memory[i] = (float)i;
    }
    P52 = 0;
    save_navigation_segment(0, distance_memory, yaw_memory, 200);
    P52 = 1;
    // read_navigation_segment(0, distance_memory, yaw_memory, 200);
    // P52 = 0;
}