#include "headfile.h"


// 定义一个联合，用于安全地处理浮点数的字节表示
typedef union {
    float f;
    uint8 b[4];
} FloatUnion;

// RAM中的双缓冲区
int32 distance_memory[MAX_POINTS] = {0};
float yaw_memory[MAX_POINTS] = {0};

// 全局变量定义
uint16 total_points_saved = 0;  // 记录已保存的总点数
uint16 current_car_index = 0;   // 记录当前小车在整个路径中的索引
uint16 flash_read_pointer = 0;  // 记录Flash中下一个待读取的点的索引

// 定义每个导航数据段包含的点数 (此宏在W25Q128方案中不再直接使用，但保留以防万一)
#define POINTS_PER_SEGMENT 200
#define PULSE_PER_CM 280 // 1cm = 280个脉冲

// --- 慢速循迹 (记录模式) API 实现 ---

/**
	* @brief  开始记录模式，初始化Flash和点数计数器
	* @param  无
	* @retval 无
	*/
/**
	* @brief  开始记录模式，初始化Flash和点数计数器 (新)
	* @param  无
	* @retval 无
	*/
void navigation_start_record_mode(void) {
	my_spi_init(); // 初始化SPI
    // flash_disable_write_protection();
    // easrse_all_spi(); // 全片擦除，确保数据干净，内部会等待擦除完成
    // 擦除第一个扇区 (Sector 0)
    easrse_spi(0x00, 0x00, 0x00);
    // 擦除第二个扇区 (Sector 1)
    easrse_spi(0x00, 0x10, 0x00);
	total_points_saved = 0; // 重置总点数
}

// /**
//   * @brief  开始记录模式，初始化Flash和点数计数器 (旧)
//   * @param  无
//   * @retval 无
//   */
// void navigation_start_record_mode_old(void) {
//     HW_SPI_W25Q128_init(); // 初始化W25Q128
//     HW_SPI_FlashChipErase(); // 全片擦除，确保数据干净
//     HW_SPI_CheckFlashBusy(); // 等待擦除完成
// 	total_points_saved = 0; // 重置总点数
// }

// void Run_Nag_Save() {
// 	if (encoder_section >= PULSE_PER_CM) {
// 		if ()
// 	}
// }









// --- W25Q128 Flash操作辅助函数实现 ---

/**
	* @brief  保存单个导航点数据到W25Q128 Flash
	* @param  distance: 距离数据
	* @param  yaw: 偏航角数据
	* @param  point_index: 点的索引 (从0开始)
	* @retval 无
	*/
/**
	* @brief  保存单个导航点数据到Flash (新)
	* @param  distance: 距离数据
	* @param  yaw: 偏航角数据
	* @param  point_index: 点的索引 (从0开始)
	* @retval 无
	*/
void save_navigation_point_to_flash(int32 distance, float yaw, uint16 point_index) {
	uint8 buffer[NAVIGATION_POINT_SIZE];
	uint32 flash_addr = NAVIGATION_DATA_FLASH_START_ADDR + point_index * NAVIGATION_POINT_SIZE;
	// 将32位地址分解为高、中、低三个8位地址
	uint8 addr_h = (uint8)((uint32)flash_addr >> 16);
	uint8 addr_m = (uint8)((uint32)flash_addr >> 8);
	uint8 addr_l = (uint8)((uint32)flash_addr);

	// 将int32距离数据复制到缓冲区
	memcpy(&buffer[0], &distance, sizeof(int32));
	// 将float偏航角数据复制到缓冲区，并进行字节序反转
	memcpy(&buffer[sizeof(int32)], &yaw, sizeof(float));

	// 调用my_spi的写入函数
	write_spi(addr_h, addr_m, addr_l, NAVIGATION_POINT_SIZE, buffer);
	// write_spi内部会处理写入使能和等待忙碌状态
}

// /**
//   * @brief  保存单个导航点数据到W25Q128 Flash (旧)
//   * @param  distance: 距离数据
//   * @param  yaw: 偏航角数据
//   * @param  point_index: 点的索引 (从0开始)
//   * @retval 无
//   */
// void save_navigation_point_to_flash_old(int32 distance, float yaw, uint16 point_index) {
//     uint8 buffer[NAVIGATION_POINT_SIZE];
//     uint32 flash_addr = NAVIGATION_DATA_FLASH_START_ADDR + point_index * NAVIGATION_POINT_SIZE;

//     // 将int32距离数据复制到缓冲区
//     memcpy(&buffer[0], &distance, sizeof(int32));
//     // 将float偏航角数据复制到缓冲区
//     memcpy(&buffer[sizeof(int32)], &yaw, sizeof(float));

//     // 写入Flash，W25Q128_Write_Nbytes内部会处理页编程和擦除
//     // 注意：W25Q128写入前需要擦除，这里假设上层调用者会确保Flash区域已擦除
//     // 或者在每次写入前进行扇区擦除 (效率较低，但简单)
//     // 为了简化，这里直接调用写入，实际应用中可能需要更精细的擦除管理
//     HW_SPI_FlashWriteEnable(); // 写入使能
// 	HW_SPI_Write_Nbytes(flash_addr, buffer, NAVIGATION_POINT_SIZE);
	
// 	HW_SPI_CheckFlashBusy(); // 等待写入完成
	
// }

/**
	* @brief  从W25Q128 Flash读取单个导航点数据
	* @param  distance: 用于存储读取到的距离数据的指针
	* @param  yaw: 用于存储读取到的偏航角数据的指针
	* @param  point_index: 点的索引 (从0开始)
	* @retval 无
	*/
/**
	* @brief  从Flash读取单个导航点数据 (新)
	* @param  distance: 用于存储读取到的距离数据的指针
	* @param  yaw: 用于存储读取到的偏航角数据的指针
	* @param  point_index: 点的索引 (从0开始)
	* @retval 无
	*/
void read_navigation_point_from_flash(int32 *distance, float *yaw, uint16 point_index) {
	uint8 buffer[NAVIGATION_POINT_SIZE];
	uint32 flash_addr = NAVIGATION_DATA_FLASH_START_ADDR + point_index * NAVIGATION_POINT_SIZE;

	// 将32位地址分解为高、中、低三个8位地址
	uint8 addr_h = (uint8)(flash_addr >> 16);
	uint8 addr_m = (uint8)(flash_addr >> 8);
	uint8 addr_l = (uint8)(flash_addr);

	FloatUnion fu;

	// 调用my_spi的读取函数
	read_spi(addr_h, addr_m, addr_l, NAVIGATION_POINT_SIZE, buffer);

	// 从缓冲区中解析int32距离数据
    memcpy(distance, &buffer[0], sizeof(int32));

	// 使用联合来安全地解析float偏航角数据
	
	memcpy(fu.b, &buffer[sizeof(int32)], sizeof(float));
	*yaw = fu.f;
}

// /**
//   * @brief  从W25Q128 Flash读取单个导航点数据 (旧)
//   * @param  distance: 用于存储读取到的距离数据的指针
//   * @param  yaw: 用于存储读取到的偏航角数据的指针
//   * @param  point_index: 点的索引 (从0开始)
//   * @retval 无
//   */
// void read_navigation_point_from_flash_old(int32 *distance, float *yaw, uint16 point_index) {
//     uint8 buffer[NAVIGATION_POINT_SIZE];
//     uint32 flash_addr = NAVIGATION_DATA_FLASH_START_ADDR + point_index * NAVIGATION_POINT_SIZE;

//     HW_SPI_Read_Nbytes(flash_addr, buffer, NAVIGATION_POINT_SIZE);

//     // 从缓冲区中解析int32距离数据
//     memcpy(distance, &buffer[0], sizeof(int32));
//     // 从缓冲区中解析float偏航角数据
//     memcpy(yaw, &buffer[sizeof(int32)], sizeof(float));
// }

/**
	* @brief  保存总点数到W25Q128 Flash的指定地址
	* @param  total_points: 要保存的总点数
	* @retval 无
	*/
/**
	* @brief  保存总点数到Flash的指定地址 (新)
	* @param  total_points: 要保存的总点数
	* @retval 无
	*/
void save_total_points_to_flash(uint16 total_points) {
	// 将32位地址分解为高、中、低三个8位地址
	uint8 addr_h = (uint8)((uint32)TOTAL_POINTS_ADDR >> 16);
	uint8 addr_m = (uint8)((uint32)TOTAL_POINTS_ADDR >> 8);
	uint8 addr_l = (uint8)((uint32)TOTAL_POINTS_ADDR);

	// 写入总点数
	write_spi(addr_h, addr_m, addr_l, sizeof(uint16), (uint8 *)&total_points);
	// write_spi内部会处理写入使能和等待忙碌状态
}

// /**
//   * @brief  保存总点数到W25Q128 Flash的指定地址 (旧)
//   * @param  total_points: 要保存的总点数
//   * @retval 无
//   */
// void save_total_points_to_flash_old(uint16 total_points) {
//     // 擦除存储总点数的扇区 (通常是扇区0)
//     HW_SPI_FlashSectorErase(TOTAL_POINTS_ADDR); 
// 	P63 = 0;
//     HW_SPI_CheckFlashBusy(); // 等待擦除完成

//     HW_SPI_FlashWriteEnable(); // 写入使能
//     HW_SPI_Write_Nbytes(TOTAL_POINTS_ADDR, (uint8 *)&total_points, sizeof(uint16));
//     HW_SPI_CheckFlashBusy(); // 等待写入完成
// }

/**
	* @brief  从W25Q128 Flash的指定地址读取总点数
	* @param  total_points: 用于存储读取到的总点数的指针
	* @retval 无
	*/
/**
	* @brief  从Flash的指定地址读取总点数 (新)
	* @param  total_points: 用于存储读取到的总点数的指针
	* @retval 无
	*/
void read_total_points_from_flash(uint16 *total_points) {
	// 将32位地址分解为高、中、低三个8位地址
    uint8 addr_h = (uint8)((uint32)TOTAL_POINTS_ADDR >> 16);
	uint8 addr_m = (uint8)((uint32)TOTAL_POINTS_ADDR >> 8);
	uint8 addr_l = (uint8)((uint32)TOTAL_POINTS_ADDR);

	// 调用my_spi的读取函数
	read_spi(addr_h, addr_m, addr_l, sizeof(uint16), (uint8 *)total_points);
}

// /**
//   * @brief  从W25Q128 Flash的指定地址读取总点数 (旧)
//   * @param  total_points: 用于存储读取到的总点数的指针
//   * @retval 无
//   */
// void read_total_points_from_flash_old(uint16 *total_points) {
//     HW_SPI_Read_Nbytes(TOTAL_POINTS_ADDR, (uint8 *)total_points, sizeof(uint16));
// }

/**
	* @brief  记录一个导航点到Flash
	* @param  distance: 当前距离
	* @param  yaw: 当前偏航角
	* @retval 无
	*/
void navigation_record_point(int32 distance, float yaw) {
	save_navigation_point_to_flash(distance, yaw, total_points_saved);
	total_points_saved++;
}

/**
	* @brief  停止记录模式，保存最终的总点数到Flash
	* @param  无
	* @retval 无
	*/
void navigation_stop_record_mode(void) {
	save_total_points_to_flash(total_points_saved);
}

// --- 快速循迹 (回放模式) API 实现 ---

/**
	* @brief  开始回放模式，从Flash加载初始数据到RAM双缓冲区
	* @param  无
	* @retval 无
	*/
/**
	* @brief  开始回放模式，从Flash加载初始数据到RAM双缓冲区 (新)
	* @param  无
	* @retval 无
	*/
void navigation_start_playback_mode(void) {
	uint16 i;
	my_spi_init(); // 初始化SPI
	read_total_points_from_flash(&total_points_saved); // 读取总点数

	current_car_index = 0;
	flash_read_pointer = 0; // 初始时，Flash读取指针指向路径的起始

	// 首次填充RAM缓冲区 (MAX_POINTS个点)
	for (i = 0; i < MAX_POINTS && flash_read_pointer < total_points_saved; i++) {
		read_navigation_point_from_flash(&distance_memory[i], &yaw_memory[i], flash_read_pointer);
		flash_read_pointer++;
	}
}

// /**
//   * @brief  开始回放模式，从W25Q128 Flash加载初始数据到RAM双缓冲区 (旧)
//   * @param  无
//   * @retval 无
//   */
// void navigation_start_playback_mode_old(void) {
//     uint16 i;
//     HW_SPI_W25Q128_init(); // 初始化W25Q128
//     read_total_points_from_flash(&total_points_saved); // 读取总点数

//     current_car_index = 0;
//     flash_read_pointer = 0; // 初始时，Flash读取指针指向路径的起始

//     // 首次填充RAM缓冲区 (MAX_POINTS个点)
//     for (i = 0; i < MAX_POINTS && flash_read_pointer < total_points_saved; i++) {
//         read_navigation_point_from_flash(&distance_memory[i], &yaw_memory[i], flash_read_pointer);
//         flash_read_pointer++;
//     }
// }

/**
	* @brief  更新小车当前索引，并根据需要触发数据预加载
	*         此函数应在主循环中周期性调用，或者在小车前进一个点后调用
	* @param  无
	* @retval 无
	*/
void navigation_update_current_index(void) {
	uint16 i;
	current_car_index++; // 小车前进一个点

	// 检查是否需要加载下一个MAX_POINTS个点
	// 当当前索引达到MAX_POINTS的整数倍时，且Flash中还有数据可读
	if ((current_car_index > 0) && (current_car_index % MAX_POINTS == 0) && (flash_read_pointer < total_points_saved)) {
			// 从Flash读取下一个MAX_POINTS个点到RAM缓冲区
		for (i = 0; i < MAX_POINTS && flash_read_pointer < total_points_saved; i++) {
			read_navigation_point_from_flash(&distance_memory[i], &yaw_memory[i], flash_read_pointer);
			flash_read_pointer++;
		}
	}
		
	// 防止current_car_index超出total_points_saved
	if (current_car_index >= total_points_saved) {
		current_car_index = total_points_saved - 1; // 停留在最后一个点
		// 或者可以设置一个标志，表示路径结束
	}
}

/**
	* @brief  获取当前小车位置的导航点数据
	* @param  distance: 用于存储距离数据的指针
	* @param  yaw: 用于存储偏航角数据的指针
	* @retval 无
	*/
void navigation_get_current_point(int32 *distance, float *yaw) {
	uint16 ram_index = current_car_index % MAX_POINTS;
	*distance = distance_memory[ram_index];
	*yaw = yaw_memory[ram_index];
}

/**
	* @brief  获取前瞻导航点数据
	* @param  distance: 用于存储距离数据的指针
	* @param  yaw: 用于存储偏航角数据的指针
	* @param  lookahead_offset: 前瞻偏移量 (相对于当前点)
	* @retval 无
	*/
void navigation_get_lookahead_point(int32 *distance, float *yaw, uint16 lookahead_offset) {
	uint16 target_index = current_car_index + lookahead_offset;
	uint16 ram_index = target_index % MAX_POINTS;
	if (target_index >= total_points_saved) {
		// 如果前瞻点超出总点数，返回最后一个点的数据
		target_index = total_points_saved - 1;
	}
	*distance = distance_memory[ram_index];
	*yaw = yaw_memory[ram_index];
}

/**
	* @brief  导航记忆和读取测试例程
	*         通过P52引脚的电平变化，在示波器上观察写入和读取200个点所需的时间。
	* @param  无
	* @retval 无
	*/
void navigation_memory_test(void) {
	int32 test_distance;
	uint16 i;
	float test_yaw;
	uint8 *yaw_bytes;
	float temp_yaw;
	FloatUnion fu;

	my_spi_init(); // 初始化SPI
	// --- 测试写入200个点 ---
	P52 = 0; // 写入开始，P52拉低
	navigation_start_record_mode(); // 初始化Flash并擦除
	for (i = 0; i < 30; i++)
		navigation_record_point(i, (float)i / 10.0f); // 写入测试数据
	navigation_stop_record_mode(); // 保存总点数
	P52 = 1; // 写入结，P52拉高

	delay_ms(500); // 延时1秒，方便观察波形束
	// --- 测试读取200个点 ---
	
	P52 = 0; // 读取开始，P52拉低
	delay_ms(500);
	read_total_points_from_flash(&total_points_saved); // 读取总点数
	current_car_index = 0;
	flash_read_pointer = 0; // 初始时，Flash读取指针指向路径的起始
	// 首次填充RAM缓冲区 (MAX_POINTS个点)
	for (i = 0; i < 30 && flash_read_pointer < total_points_saved; i++) {
		read_navigation_point_from_flash(&distance_memory[i], &yaw_memory[i], flash_read_pointer);
		flash_read_pointer++;
	}
	// for (i = 0; i < 31; i++) {
	// 	navigation_get_current_point(&test_distance, &test_yaw); // 获取当前点数据
	// 	// 实际应用中，这里会使用这些数据进行导航控制
	// 	// 为了测试时间，我们只读取，不进行复杂计算
	// 	navigation_update_current_index(); // 更新索引，并触发预加载
	// }
	P52 = 1; // 读取结束，P52拉高
	printf("%d\r\n", total_points_saved);
	for (i = 0; i < 30; i++) {
		// 打印距离 (int32)
		printf("%d,", distance_memory[i]);
		// 使用联合来确保浮点数正确传递给printf，并强制转换为double
		
		fu.f = yaw_memory[i];
		printf("%.1f\r\n", (double)fu.f);
	}
}

// --- 原AT24C EEPROM相关函数 (已注释掉，不再使用) ---
/*
// /**
//   * @brief  保存指定段的导航数据到AT24C EEPROM
//   * @param  segment_index: 要保存的段的索引 (从0开始)
//   * @param  distances: 包含要保存的距离数据的数组指针
//   * @param  yaws: 包含要保存的偏航角数据的数组指针
//   * @param  num_points_in_segment: 当前段中实际要保存的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void save_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_in_segment) {
//     // 计算当前段在EEPROM中的起始地址
//     uint16 base_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
//     // uint16 i;
//     uint8 page_buffer[64]; // 64字节页缓冲区
//     uint16 bytes_to_write = num_points_in_segment * NAVIGATION_POINT_SIZE;
//     uint16 current_byte_idx = 0;

//     while (current_byte_idx < bytes_to_write) {
//         uint16 page_addr = base_address + current_byte_idx;
//         uint16 bytes_in_current_page = 0;
//         uint16 buffer_offset = 0;

//         // 计算当前页的起始地址在EEPROM中的偏移量
//         uint16 ee_page_offset = page_addr % 64;
				
//         // 填充页缓冲区
//         while (bytes_in_current_page < 64 && current_byte_idx < bytes_to_write) {
//             uint16 point_idx = current_byte_idx / NAVIGATION_POINT_SIZE;
//             uint16 byte_offset_in_point = current_byte_idx % NAVIGATION_POINT_SIZE;

//             if (byte_offset_in_point < sizeof(int32)) { // 距离数据
//                 page_buffer[buffer_offset] = ((uint8 *)&distances[point_idx])[byte_offset_in_point];
//             } else { // 偏航角数据
//                 page_buffer[buffer_offset] = ((uint8 *)&yaws[point_idx])[(byte_offset_in_point - sizeof(int32))];
//             }
//             current_byte_idx++;
//             bytes_in_current_page++;
//             buffer_offset++;
//         }
				
//         // 写入一整页或剩余数据
//         // 注意：AT24C_Write_Page 总是写入64字节，如果不足64字节，需要用AT24C_Write
//         // 这里为了简化，假设每次都写入完整的64字节页，或者处理最后一页不足64字节的情况
//         // 实际应根据ee_page_offset和bytes_in_current_page来决定如何调用
				
//         // 简单的处理方式：如果不是从页起始地址开始写，或者不足一页，使用AT24C_Write
//         // 否则使用AT24C_Write_Page
//         if (ee_page_offset != 0 || bytes_in_current_page < 64) {
//             // 如果不是从页起始地址开始写，或者不足一页，使用AT24C_Write
//             // AT24C_Write函数内部会处理页边界延时
//             AT24C_Write(page_addr, page_buffer, bytes_in_current_page);
//         } else {
//             // 从页起始地址开始写，且是完整一页，使用AT24C_Write_Page
//             AT24C_Write_Page(page_addr, page_buffer);
//             delay_ms(4); // 每次写入一页后延时，等待EEPROM写入完成
//         }
//     }
// }

// /**
//   * @brief  从AT24C EEPROM读取指定段的导航数据
//   * @param  segment_index: 要读取的段的索引 (从0开始)
//   * @param  distances: 用于存储读取到的距离数据的数组指针
//   * @param  yaws: 用于存储读取到的偏航角数据的数组指针
//   * @param  num_points_to_read: 当前段中实际要读取的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void read_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_to_read)
// {
//     // 计算当前段在EEPROM中的起始地址
//     uint16 base_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
//     uint16 i;
//     uint8 buffer[NAVIGATION_POINT_SIZE]; // 临时缓冲区，用于读取单个点的数据

//     for (i = 0; i < num_points_to_read; i++) {
//         // 使用AT24C_Read函数读取一个点的数据 (8字节)
//         AT24C_Read(base_address + i * NAVIGATION_POINT_SIZE, buffer, NAVIGATION_POINT_SIZE);
				
//         // 从缓冲区中解析int32距离数据
//         distances[i] = *(int32 *)&buffer[0];
//         // 从缓冲区中解析float偏航角数据
//         yaws[i] = *(float *)&buffer[sizeof(int32)];
//     }
// }

// /**
//   * @brief  保存总点数index到AT24C EEPROM的0x0000地址
//   * @param  无
//   * @retval 无
//   */
// void save_navigation_index(void)
// {
//     AT24C_Write(0x0000, (uint8 *)&index, sizeof(index));
// }

// /**
//   * @brief  从AT24C EEPROM的0x0000地址读取总点数index
//   * @param  无
//   * @retval 无
//   */
// void read_navigation_index(void)
// {
//     AT24C_Read(0x0000, (uint8 *)&index, sizeof(index));
// }

// void navigation_memory_test(void) {
//     uint8 i;
//     for (i = 0; i < 200; i++) {
//         distance_memory[i] = i;
//         yaw_memory[i] = (float)i;
//     }
//     P52 = 0;
//     save_navigation_segment(0, distance_memory, yaw_memory, 200);
//     P52 = 1;
//     // read_navigation_segment(0, distance_memory, yaw_memory, 200);
//     // P52 = 0;
// }

// /**
//   * @brief  保存指定段的导航数据到AT24C EEPROM
//   * @param  segment_index: 要保存的段的索引 (从0开始)
//   * @param  distances: 包含要保存的距离数据的数组指针
//   * @param  yaws: 包含要保存的偏航角数据的数组指针
//   * @param  num_points_in_segment: 当前段中实际要保存的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void save_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_in_segment) {
//     // 计算当前段在EEPROM中的起始地址
//     uint16 base_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
//     // uint16 i;
//     uint8 page_buffer[64]; // 64字节页缓冲区
//     uint16 bytes_to_write = num_points_in_segment * NAVIGATION_POINT_SIZE;
//     uint16 current_byte_idx = 0;

//     while (current_byte_idx < bytes_to_write) {
//         uint16 page_addr = base_address + current_byte_idx;
//         uint16 bytes_in_current_page = 0;
//         uint16 buffer_offset = 0;

//         // 计算当前页的起始地址在EEPROM中的偏移量
//         uint16 ee_page_offset = page_addr % 64;
				
//         // 填充页缓冲区
//         while (bytes_in_current_page < 64 && current_byte_idx < bytes_to_write) {
//             uint16 point_idx = current_byte_idx / NAVIGATION_POINT_SIZE;
//             uint16 byte_offset_in_point = current_byte_idx % NAVIGATION_POINT_SIZE;

//             if (byte_offset_in_point < sizeof(int32)) { // 距离数据
//                 page_buffer[buffer_offset] = ((uint8 *)&distances[point_idx])[byte_offset_in_point];
//             } else { // 偏航角数据
//                 page_buffer[buffer_offset] = ((uint8 *)&yaws[point_idx])[(byte_offset_in_point - sizeof(int32))];
//             }
//             current_byte_idx++;
//             bytes_in_current_page++;
//             buffer_offset++;
//         }
				
//         // 写入一整页或剩余数据
//         // 注意：AT24C_Write_Page 总是写入64字节，如果不足64字节，需要用AT24C_Write
//         // 这里为了简化，假设每次都写入完整的64字节页，或者处理最后一页不足64字节的情况
//         // 实际应根据ee_page_offset和bytes_in_current_page来决定如何调用
				
//         // 简单的处理方式：如果不是从页起始地址开始写，或者不足一页，使用AT24C_Write
//         // 否则使用AT24C_Write_Page
//         if (ee_page_offset != 0 || bytes_in_current_page < 64) {
//             // 如果不是从页起始地址开始写，或者不足一页，使用AT24C_Write
//             // AT24C_Write函数内部会处理页边界延时
//             AT24C_Write(page_addr, page_buffer, bytes_in_current_page);
//         } else {
//             // 从页起始地址开始写，且是完整一页，使用AT24C_Write_Page
//             AT24C_Write_Page(page_addr, page_buffer);
//             delay_ms(4); // 每次写入一页后延时，等待EEPROM写入完成
//         }
//     }
// }

// /**
//   * @brief  从AT24C EEPROM读取指定段的导航数据
//   * @param  segment_index: 要读取的段的索引 (从0开始)
//   * @param  distances: 用于存储读取到的距离数据的数组指针
//   * @param  yaws: 用于存储读取到的偏航角数据的数组指针
//   * @param  num_points_to_read: 当前段中实际要读取的点数 (最大为POINTS_PER_SEGMENT)
//   * @retval 无
//   */
// void read_navigation_segment(uint16 segment_index, int32 *distances, float *yaws, uint16 num_points_to_read)
// {
//     // 计算当前段在EEPROM中的起始地址
//     uint16 base_address = NAVIGATION_DATA_START_ADDR + segment_index * POINTS_PER_SEGMENT * NAVIGATION_POINT_SIZE;
//     uint16 i;
//     uint8 buffer[NAVIGATION_POINT_SIZE]; // 临时缓冲区，用于读取单个点的数据

//     for (i = 0; i < num_points_to_read; i++) {
//         // 使用AT24C_Read函数读取一个点的数据 (8字节)
//         AT24C_Read(base_address + i * NAVIGATION_POINT_SIZE, buffer, NAVIGATION_POINT_SIZE);
				
//         // 从缓冲区中解析int32距离数据
//         distances[i] = *(int32 *)&buffer[0];
//         // 从缓冲区中解析float偏航角数据
//         yaws[i] = *(float *)&buffer[sizeof(int32)];
//     }
// }

// /**
//   * @brief  保存总点数index到AT24C EEPROM的0x0000地址
//   * @param  无
//   * @retval 无
//   */
// void save_navigation_index(void)
// {
//     AT24C_Write(0x0000, (uint8 *)&index, sizeof(index));
// }

// /**
//   * @brief  从AT24C EEPROM的0x0000地址读取总点数index
//   * @param  无
//   * @retval 无
//   */
// void read_navigation_index(void)
// {
//     AT24C_Read(0x0000, (uint8 *)&index, sizeof(index));
// }

// void navigation_memory_test(void) {
//     uint8 i;
//     for (i = 0; i < 200; i++) {
//         distance_memory[i] = i;
//         yaw_memory[i] = (float)i;
//     }
//     P52 = 0;
//     save_navigation_segment(0, distance_memory, yaw_memory, 200);
//     P52 = 1;
//     // read_navigation_segment(0, distance_memory, yaw_memory, 200);
//     // P52 = 0;
// }
