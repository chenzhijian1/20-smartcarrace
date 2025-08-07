#include "headfile.h"
#include "my_spi.h"


//  uint8 my_spi_init(void)
// {
// 	gpio_init(IO_P40,GPO,0,GPO_PUSH_PULL);

//     spi_init(SPI_1,SPI_MODE0,9600,SPI1_CH3_SCLK_P43,SPI1_CH3_MOSI_P41,SPI1_CH3_MISO_P42,IO_P40);

//     gpio_set_level(IO_P40,0);

//     spi_write_8bit(SPI_1,0x90);

// 	spi_write_8bit(SPI_1,0x00);

// 	spi_write_8bit(SPI_1,0x00);

// 	spi_write_8bit(SPI_1,0x00);

// 	id[0]=spi_read_8bit(SPI_1);

// 	id[1]=spi_read_8bit(SPI_1);
 
// 	id[2]=spi_read_8bit(SPI_1);
	
// 	gpio_set_level(IO_P40,1);
	
// 	if((id[0]==239)&&(id[1]==23)&&(id[2]==239))
		
// 	return 1;
	
	
// 	return 0;
// }

/**
 * @brief ???SPI??????SPI Flash??????
 *        ??SPI???????????????
 * @param None
 * @return None
 */
void my_spi_init(void)
{
    // ?????SPI???????1???0?????
    // SCK: P1.5, MOSI: P1.3, MISO: P1.4
    spi_init(SPI_CH1, SPI_CH1_SCLK_P15, SPI_CH1_MOSI_P13, SPI_CH1_MISO_P14, 0, MASTER, SPI_SYSclk_DIV_2); // ??SPI_SYSclk_DIV_2?????
    gpio_mode(P3_3, GPO_PP);
    P33 = 1;

	gpio_mode(P3_7, GPO_PP);
	P37 = 1;
	// ??CS????????????????
    gpio_mode(P4_7, GPO_PP);
    W25Q128_HW_SPI_CS = 1;
}

/**
 * @brief ??SPI Flash??????
 *        ??????????????????????????????????????
 * @param addr_h ?????
 * @param addr_m ?????
 * @param addr_l ?????
 * @return uint8 ??????????????0?????
 */
uint8  easrse_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l )
{
	// 1. 发送写使能命令 (0x06)
	P47 = 0; // CS拉低
	spi_mosi(SPI_WRITE);
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保写使能生效

	// 2. 发送扇区擦除命令 (0x20) 和地址
	P47 = 0; // CS拉低
	spi_mosi(SPI_EASRSE);
	spi_mosi(addr_h);
	spi_mosi(addr_m);
	spi_mosi(addr_l);
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保命令被Flash接收

	// 3. 等待擦除操作完成 (轮询忙碌位)
	while(read_sr1() & 0x01); // 循环读取状态寄存器1，直到BUSY位(Bit 0)为0
	P63 = 0; // 调试引脚，可保留或移除
	return 0; // 擦除成功返回0
}

/**
 * @brief ??SPI Flash??????
 *        ???????????????????????????????????
 * @param None
 * @return uint8 ??????????????0?????
 */
uint8  easrse_all_spi(void)
{
	// 1. 发送写使能命令 (0x06)
	P47 = 0; // CS拉低
	spi_mosi(SPI_WRITE);
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保写使能生效

	// 2. 发送全片擦除命令 (0x60)
	P47 = 0; // CS拉低
	spi_mosi(SPI_EASRSE_ALL);
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保命令被Flash接收

	// 3. 等待擦除操作完成 (轮询忙碌位)
	while(read_sr1() & 0x01); // 循环读取状态寄存器1，直到BUSY位(Bit 0)为0
	// P63 = 0; // 调试引脚，可保留或移除
	return 0; // 擦除成功返回0
}
/**
 * @brief ??SPI Flash??????1 (SR1) ???
 *        SR1????????????????????
 * @param None
 * @return uint8 ?????1??
 */
uint8 read_sr1(void)
{
	uint8 status_reg_value;
	P47 = 0; // CS拉低
	spi_mosi(SPI_BUSY); // 发送读状态寄存器1命令 (0x05)
	status_reg_value = spi_mosi(0xFF); // 读取状态寄存器值
	P47 = 1; // CS拉高

	// 调试输出：通过P32引脚指示BUSY位状态
	// 确保P32已配置为GPO_PP
	// gpio_mode(P3_2, GPO_PP); // 假设已在my_spi_init或main中配置
	// if (status_reg_value & 0x01) { // 如果BUSY位为1 (忙碌)
	// 	P32 = 0; // 拉低P32
	// } else { // 如果BUSY位为0 (空闲)
	// 	P32 = 1; // 拉高P32
	// }
	
	return status_reg_value;
}
/**
 * @brief ??SPI Flash??????2 (SR2) ???
 *        SR2?????????????????OTP????
 * @param None
 * @return uint8 ?????2??
 */
uint8 read_sr2(void)
{
	uint8 temp=0;
	
	
	//  gpio_set_level(IO_P40,0);
	P47 = 0;
	 spi_mosi(0x35); // ??0x35???????????2
	 
	 temp=spi_mosi(0xFF); // ??????????
	 
	//  gpio_set_level(IO_P40,1);
	 P47 = 1;
	 /*
	
	 gpio_set_level(IO_P40,0);
	
	 spi_write_8bit(SPI_1,SPI_WRITE);
	
	 gpio_set_level(IO_P40,1);
	 
	 gpio_set_level(IO_P40,0);
	
	 spi_write_8bit(SPI_1,0x31);
	 
	 spi_write_8bit(SPI_1,0x42);
	 
	 gpio_set_level(IO_P40,1);
	 
	 gpio_set_level(IO_P40,0);
	
	 spi_write_8bit(SPI_1,0x35);
	 
	 temp = spi_read_8bit(SPI_1);
	 
	 gpio_set_level(IO_P40,1);*/
	
	 return temp;
	
}


/**
 * @brief ?SPI Flash????????????
 *        ??????????????????????????????????
 * @param addr_h ?????
 * @param addr_m ?????
 * @param addr_l ?????
 * @param size ???????????256???????
 * @param array ?????????????
 * @return None
 */
void write_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 size,uint8* array)
{
	 uint32 point;
	
	// 1. 等待芯片空闲 (确保前一个操作已完成)
	while(read_sr1() & 0x01);
	 
	// 2. 发送写使能命令 (0x06)
	P47 = 0; // CS拉低
	spi_mosi(SPI_WRITE);
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保写使能生效
	
	// 3. 发送页编程命令 (0x02)、地址和数据
	P47 = 0; // CS拉低
	spi_mosi(SPI_PAGE_WRITE);
	spi_mosi(addr_h);
	spi_mosi(addr_m);
	spi_mosi(addr_l);
	
	for(point=0;point<size;point++)
	{
		spi_mosi(array[point]);
	}
	
	P47 = 1; // CS拉高
	// delay_us(10); // 短暂延时，确保命令被Flash接收

	// 4. 等待写入操作完成 (轮询忙碌位)
	while(read_sr1() & 0x01);
}

/**
 * @brief ?SPI Flash?????????????
 *        ?????????????????? `write_spi` ????????
 * @param addr_h ???????
 * @param addr_m ???????
 * @param addr_l ???????
 * @param page ??????
 * @param array ?????????????
 * @return None
 */
void  all_write_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 page,uint8* array)
{
	uint32 addr;
	uint32 point=0;
	
	addr=(addr_h<<16)+(addr_m<<8)+(addr_l<<0);
	
	for(;(uint32)addr<(page*256);addr+=256) //255??
	{
		write_spi((uint8)(addr>>16),(uint8)(addr>>8),(uint8)(addr),256,&array[point]);
		point+=256;
	}

    while(read_sr1() & 0x01);
}


/**
 * @brief ?SPI Flash??????????
 *        ??????????????????????????????
 * @param addr_h ???????
 * @param addr_m ???????
 * @param addr_l ???????
 * @param array_size ???????
 * @param array ??????????????
 * @return None
 */
void read_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 array_size,uint8* array)
{
	 uint32 point;
	
	//  gpio_set_level(IO_P40,0);
	P47 = 0;
	 spi_mosi(SPI_READ);
	
	 spi_mosi(addr_h);
	
	 spi_mosi(addr_m);
	
	 spi_mosi(addr_l);
	
	 for(point=0;point<array_size;point++)
	{
		array[point]=spi_mosi(0xFF);
	}
	 
	//  gpio_set_level(IO_P40,1);
	P47 = 1;
}

void test_read_jedec_id(void) {
    uint8 manufacturer_id, device_id_high, device_id_low;

    P47 = 0; // ??CS?????

    spi_mosi(0x9F); // ???? JEDEC ID ???

    // ????3????????3????ID??
    manufacturer_id = spi_mosi(0xFF);
    device_id_high  = spi_mosi(0xFF);
    device_id_low   = spi_mosi(0xFF);

    P47 = 1; // ??CS?????

	if (manufacturer_id == 0xEF)  P32 = 0;
	if (device_id_high == 0x40)   P36 = 0;
	if (device_id_low == 0x18)    P50 = 0;
	delay_ms(1);
	// --- ?????????????????? ---
    // manufacturer_id ???? 0xEF
    // device_id_high  ???? 0x40
    // device_id_low   ???? 0x18 (?? W25Q128JV)
}

void flash_disable_write_protection(void) {
    // 1. 发送“写使能”命令 (0x06)
	uint8 temp = 1;
    P47 = 0;
    spi_mosi(0x06);
    P47 = 1;

    // 2. 发送“写状态寄存器-1”命令 (0x01)，并将保护位清零
    P47 = 0;
    spi_mosi(0x01); // 写状态寄存器-1 命令
    spi_mosi(0x00); // 将所有保护位 (BP0-BP4, SRP) 都清零
    P47 = 1;

    // 等待状态寄存器写入完成
    
    do {
        P47 = 0;
        spi_mosi(0x05); // 读状态寄存器-1 命令
        temp = spi_mosi(0xFF);
        P47 = 1;
    } while (temp & 0x01); // 等待忙碌位变为0
}

void detailed_flash_debug_gpio(void) {
    uint8 status_reg1;

    // --- 步骤 0: 初始化 ---
    my_spi_init();
    // 确保调试引脚是输出模式
    gpio_mode(P2_4, GPO_PP);
    gpio_mode(P4_4, GPO_PP);
    gpio_mode(P4_6, GPO_PP);
    
    // --- 步骤 1: 发送“写使能”命令 ---
    P47 = 0;
    spi_mosi(0x06); // 写使能命令
    P47 = 1;
    delay_ms(1);

    // --- 步骤 2: 读取状态，检查 WEL 位是否被置位 ---
    P47 = 0;
    spi_mosi(0x05); // 读状态寄存器-1
    status_reg1 = spi_mosi(0xFF);
    P47 = 1;

    // 检查 WEL 位 (状态寄存器第1位) 是否为 1
    // 如果 status_reg1 的值是 0x02，说明“写使能”成功
    if (status_reg1 == 0x02) {
        P24 = 0; // 如果“写使能”成功，则拉低 P24
    }

    // --- 步骤 3: 发送“写状态寄存器”命令 ---
    P47 = 0;
    spi_mosi(0x01); // 写状态寄存器-1 命令
    spi_mosi(0x00); // 尝试将所有保护位清零
    P47 = 1;
    delay_ms(1);

    // --- 步骤 4: 再次读取状态，检查是否不再忙碌 ---
    P47 = 0;
    spi_mosi(0x05); // 读状态寄存器-1
    status_reg1 = spi_mosi(0xFF);
    P47 = 1;

    // 检查 BUSY 位 (状态寄存器第0位) 是否为 0
    // 如果 status_reg1 的值是 0x00，说明“写状态寄存器”操作成功且已完成
    if (status_reg1 == 0x00) {
        P44 = 0; // 如果操作成功且已完成，则拉低 P44
    }

    // --- 步骤 5: 再次发送“写使能”并尝试擦除扇区 ---
    // 这一步是为了确认在解除保护后，擦除操作是否能正常启动
    P47 = 0;
    spi_mosi(0x06); // 写使能
    P47 = 1;
    delay_ms(1);

    P47 = 0;
    spi_mosi(0x20); // 扇区擦除命令 (Sector Erase)
    spi_mosi(0x00); // 地址高位
    spi_mosi(0x00); // 地址中位
    spi_mosi(0x00); // 地址低位
    P47 = 1;
    delay_ms(1);

    // --- 步骤 6: 检查擦除操作是否已启动 (芯片是否变忙) ---
    P47 = 0;
    spi_mosi(0x05); // 读状态寄存器-1
    status_reg1 = spi_mosi(0xFF);
    P47 = 1;

    // 检查 BUSY 位 (第0位) 和 WEL 位 (第1位)
    // 擦除操作开始后，BUSY 位应为 1，WEL 位应被自动清零。
    // 所以 status_reg1 的值应该是 0x01。
    if (status_reg1 == 0x01) {
        P46 = 0; // 如果擦除操作成功启动，则拉低 P46
    }

    // 让程序停在这里，方便您测量引脚电平
    while(1);
}

void final_flash_debug(void) {
    uint8 status_reg1;
	uint16 timeout = 1000; // 设置一个超时计数

    my_spi_init();
    gpio_mode(P2_4, GPO_PP);
    gpio_mode(P4_4, GPO_PP);
    gpio_mode(P4_6, GPO_PP);

    // --- 步骤 1: 写使能 ---
    P47 = 0;
    spi_mosi(0x06);
    P47 = 1;
    delay_us(1); // <--- 增加微秒延时

    // --- 步骤 2: 检查 WEL 位 ---
    P47 = 0;
    spi_mosi(0x05);
    status_reg1 = spi_mosi(0xFF);
    P47 = 1;
    delay_us(1); // <--- 增加微秒延时
    if (status_reg1 == 0x02) {
        P24 = 0;
    }

    // --- 步骤 3: 写状态寄存器 ---
    P47 = 0;
    spi_mosi(0x01);
    spi_mosi(0x00);
    P47 = 1;
    delay_us(1); // <--- 增加微秒延时

    // --- 步骤 4: 循环等待操作完成 ---
    // 我们给它一点时间来完成，比如10毫秒
    
    do {
        P47 = 0;
        spi_mosi(0x05);
        status_reg1 = spi_mosi(0xFF);
        P47 = 1;
        delay_us(10); // 每次查询之间延时10微秒
        timeout--;
    } while ((status_reg1 & 0x01) && (timeout > 0)); // 等待忙碌位变为0，或者超时

    // --- 步骤 5: 检查最终状态 ---
    if ((status_reg1 & 0x01) == 0) { // 如果不忙了
        P44 = 0;
    }

    // --- 步骤 6: 尝试擦除 ---
    P47 = 0;
    spi_mosi(0x06);
    P47 = 1;
    delay_us(1);

    P47 = 0;
    spi_mosi(0x20);
    spi_mosi(0x00);
    spi_mosi(0x00);
    spi_mosi(0x00);
    P47 = 1;
    delay_us(1);

    timeout = 1000;
    do {
        P47 = 0;
        spi_mosi(0x05);
        status_reg1 = spi_mosi(0xFF);
        P47 = 1;
        delay_us(10);
        timeout--;
    } while ((status_reg1 & 0x01) == 0 && (timeout > 0)); // 等待它变忙

    if (status_reg1 & 0x01) { // 如果变忙了
        P46 = 0;
    }

    while(1);
}

void final_solution_debug(void) {
    uint8 status_reg;

    my_spi_init();
    gpio_mode(P2_4, GPO_PP);
    gpio_mode(P4_4, GPO_PP);

    // --- 步骤 1: 发送“写使能” (0x06) ---
    P47 = 0;        // CS 拉低
    delay_us(1);    // 关键延时: 确保 CS 稳定
    spi_mosi(0x06); // 发送命令
    delay_us(1);    // 关键延时
    P47 = 1;        // CS 拉高
    delay_us(1);    // 关键延时: 确保命令执行间隔

    // --- 步骤 2: 验证“写使能”是否成功 ---
    P47 = 0;
    delay_us(1);
    spi_mosi(0x05); // 读状态寄存器-1
    status_reg = spi_mosi(0xFF);
    delay_us(1);
    P47 = 1;
    delay_us(1);
    if (status_reg == 0x02) {
        P24 = 0; // 如果写使能成功，拉低 P24
    }

    // --- 步骤 3: 发送“写状态寄存器” (0x01) 并写入 0x00 ---
    P47 = 0;
    delay_us(1);
    spi_mosi(0x01); // 写状态寄存器-1 命令
    spi_mosi(0x00); // 写入 0x00 来解除保护
    delay_us(1);
    P47 = 1;
    delay_ms(5); // **关键延时**: 写状态寄存器需要一个短暂的内部写入时间 (tW)，通常是几毫秒

    // --- 步骤 4: 检查操作是否完成 ---
    // 此时芯片应该已经完成了写操作，并且变为空闲状态
    P47 = 0;
    delay_us(1);
    spi_mosi(0x05); // 再次读取状态寄存器-1
    status_reg = spi_mosi(0xFF);
    delay_us(1);
    P47 = 1;
    
    if (status_reg == 0x00) {
        P44 = 0; // 如果成功解除保护且芯片空闲，拉低 P44
    }

    while(1); // 停在这里
}

void check_protection_mode(void) {
    uint8 status_reg1, status_reg2;

    my_spi_init();
    // 确保调试引脚是输出模式
    gpio_mode(P2_4, GPO_PP); // 用 P24 指示 SRP0 的状态
    gpio_mode(P4_4, GPO_PP); // 用 P44 指示 SRP1 的状态

    // --- 步骤 1: 读取状态寄存器-1 ---
    P47 = 0;
    spi_mosi(0x05); // 读 SR1 命令
    status_reg1 = spi_mosi(0xFF);
    P47 = 1;
    delay_us(1);

    // --- 步骤 2: 读取状态寄存器-2 ---
    P47 = 0;
    spi_mosi(0x35); // 读 SR2 命令
    status_reg2 = spi_mosi(0xFF);
    P47 = 1;

    // --- 步骤 3: 检查 SRP0 位 (SR1 的第 7 位) ---
    // 如果 SRP0 位为 0 (正常)，则拉低 P24
    if ((status_reg1 & 0x80) == 0) {
        P24 = 0;
    }

    // --- 步骤 4: 检查 SRP1 位 (SR2 的第 0 位) ---
    // 如果 SRP1 位为 0 (正常)，则拉低 P44
    if ((status_reg2 & 0x01) == 0) {
        P44 = 0;
    }

    // 让程序停在这里，方便您测量引脚电平
    while(1);
}

void check_quad_enable_bit(void) {
    uint8 status_reg2;

    my_spi_init();
    // 我们使用 P24 来指示 QE 位的状态
    gpio_mode(P2_4, GPO_PP);

    // --- 步骤 1: 读取状态寄存器-2 ---
    P47 = 0;
    spi_mosi(0x35); // 发送读取状态寄存器-2的命令
    status_reg2 = spi_mosi(0xFF); // 读取返回的值
    P47 = 1;

    // --- 步骤 2: 检查 QE 位 (SR2 的第 1 位) ---
    // 如果 QE 位为 1, 那么 status_reg2 & 0x02 的结果不为 0
    if (status_reg2 & 0x02) {
        P24 = 0; // 如果 QE=1，则拉低 P24
    }

    // 让程序停在这里，方便您测量引脚电平
    while(1);
}

// 将此函数添加到您的 my_spi.c 或 main.c 中
// 再次确认您的 oscilloscope_debug_loop 函数是否是这样的：
void oscilloscope_debug_loop(void) {
    uint8 status_reg_value;

    my_spi_init(); // 确保SPI已初始化 (使用模式0)

    while(1) {
        // 1. 发送“写使能” (0x06)
        P47 = 0;
        delay_us(1);
        spi_mosi(0x06);
        delay_us(1);
        P47 = 1;
        delay_ms(5);

        // 2. 发送“写状态寄存器” (0x01) 并写入 0x00
        P47 = 0;
        delay_us(1);
        spi_mosi(0x01);
        spi_mosi(0x00); // 写入 0x00 来解除保护
        delay_us(1);
        P47 = 1;
        delay_ms(15);

        // 3. 读取状态寄存器 (0x05) 来验证结果
        P47 = 0;
        delay_us(1);
        spi_mosi(0x05); // 读状态寄存器-1 命令
        status_reg_value = spi_mosi(0xFF); // 读取结果
        delay_us(1);
        P47 = 1;
        delay_ms(5);
    }
}
