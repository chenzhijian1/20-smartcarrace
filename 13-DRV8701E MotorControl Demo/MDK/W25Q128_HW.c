#include "W25Q128_HW.h"
#include "headfile.h" // 包含通用头文件，假设其中包含了GPIO操作和延时函数
#include "zf_spi.h"   // 包含逐飞科技的硬件SPI驱动头文件

// 硬件SPI初始化函数
void HW_SPI_W25Q128_init(void)
{
    // 初始化硬件SPI模块，使用通道1，模式0，最高速率
    // SCK: P1.5, MOSI: P1.3, MISO: P1.4
    spi_init(SPI_CH1, SPI_CH1_SCLK_P15, SPI_CH1_MOSI_P13, SPI_CH1_MISO_P14, 0, MASTER, SPI_SYSclk_DIV_4); // 假设SPI_SYSclk_DIV_2是最高速率
    gpio_mode(P3_3, GPO_PP);
    P33 = 1;
    // 配置CS引脚为推挽输出，并拉高（不选中）
    gpio_mode(P4_7, GPO_PP);
    W25Q128_HW_SPI_CS = 1;
}

// 检查Flash ID (硬件SPI版本)
void HW_SPI_FlashCheckID(void)
{
    uint8 manufacturer_id, device_id_high, device_id_low;

    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_JEDEC_ID); // 发送JEDEC ID命令
    manufacturer_id = spi_mosi(0xFF); // 读取制造商ID
    device_id_high = spi_mosi(0xFF);  // 读取设备ID高字节
    device_id_low = spi_mosi(0xFF);   // 读取设备ID低字节
    W25Q128_HW_SPI_CS = 1; // 取消选中

    // 可以在这里添加打印或断点来验证ID
    // 例如：printf("W25Q128 HW ID: %02X %02X %02X\r\n", manufacturer_id, device_id_high, device_id_low);
}

// 检查Flash忙状态 (硬件SPI版本)
uint8 HW_SPI_CheckFlashBusy(void)
{
    uint8 status;
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_READ_STATUS_REG1); // 发送读取状态寄存器1命令
    status = spi_mosi(0xFF); // 读取状态寄存器1内容
    W25Q128_HW_SPI_CS = 1; // 取消选中
    return (status & W25Q_BUSY_BIT); // 返回BUSY位状态
}

// Flash写使能 (硬件SPI版本)
void HW_SPI_FlashWriteEnable(void)
{
    uint8 status;
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_WRITE_ENABLE); // 发送写使能命令
    W25Q128_HW_SPI_CS = 1; // 取消选中

    // 等待WEL位被设置 (Write Enable Latch)
    // 确保Flash已进入写使能状态
    while (1)
    {
        W25Q128_HW_SPI_CS = 0; // 选中芯片
        spi_mosi(W25Q_READ_STATUS_REG1); // 发送读取状态寄存器1命令
        status = spi_mosi(0xFF); // 读取状态寄存器1内容
        W25Q128_HW_SPI_CS = 1; // 取消选中

        if (status & W25Q_WEL_BIT) // 如果WEL位为1
        {
            break; // 退出循环
        }
    }
}

// Flash整片擦除 (硬件SPI版本)
void HW_SPI_FlashChipErase(void)
{
    HW_SPI_FlashWriteEnable(); // 写使能
    while(HW_SPI_CheckFlashBusy()); // 恢复：等待忙状态结束 (确保写使能完成)
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_CHIP_ERASE); // 发送整片擦除命令
    W25Q128_HW_SPI_CS = 1; // 取消选中
    while(HW_SPI_CheckFlashBusy()); // 恢复：等待忙状态结束 (确保擦除完成)
}

// Flash扇区擦除 (硬件SPI版本)
void HW_SPI_FlashSectorErase(uint32 addr)
{
    HW_SPI_FlashWriteEnable(); // 写使能
    while(HW_SPI_CheckFlashBusy()); // 等待忙状态结束
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_SECTOR_ERASE); // 发送扇区擦除命令
    spi_mosi((addr >> 16) & 0xFF); // 发送24位地址高8位
    spi_mosi((addr >> 8) & 0xFF);  // 发送24位地址中8位
    spi_mosi(addr & 0xFF);         // 发送24位地址低8位
    W25Q128_HW_SPI_CS = 1; // 取消选中
    P63 = 0;
    while(HW_SPI_CheckFlashBusy()); // 等待忙状态结束
}

// SPI读取N字节数据 (硬件SPI版本)
void HW_SPI_Read_Nbytes(uint32 addr, uint8 *buffer, uint16 size)
{
    uint16 i;
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_READ_DATA); // 发送读取数据命令
    spi_mosi((addr >> 16) & 0xFF); // 发送24位地址高8位
    spi_mosi((addr >> 8) & 0xFF);  // 发送24位地址中8位
    spi_mosi(addr & 0xFF);         // 发送24位地址低8位

    for (i = 0; i < size; i++)
    {
        buffer[i] = spi_mosi(0xFF); // 读取数据
    }
    W25Q128_HW_SPI_CS = 1; // 取消选中
}

// SPI读取并比较数据 (硬件SPI版本)
uint8 HW_SPI_Read_Compare(uint32 addr, uint8 *buffer, uint16 size)
{
    uint16 i;
    uint8 temp_buffer[256]; // 临时缓冲区，假设最大页大小为256字节

    uint16 bytes_to_read = (size > 256) ? 256 : size; 

    HW_SPI_Read_Nbytes(addr, temp_buffer, bytes_to_read); // 读取数据到临时缓冲区

    for (i = 0; i < bytes_to_read; i++)
    {
        if (temp_buffer[i] != buffer[i])
        {
            return 0; // 不匹配
        }
    }
    return 1; // 完全匹配
}

// SPI写入N字节数据 (硬件SPI版本)
void HW_SPI_Write_Nbytes(uint32 addr, uint8 *buffer, uint16 size)
{
    uint16 i; // 修正：将循环变量改为 uint16，避免潜在溢出
    HW_SPI_FlashWriteEnable(); // 写使能
    while(HW_SPI_CheckFlashBusy()); // 等待忙状态结束 (确保写使能完成)
    W25Q128_HW_SPI_CS = 0; // 选中芯片
    spi_mosi(W25Q_PAGE_PROGRAM); // 发送页编程命令
    spi_mosi((addr >> 16) & 0xFF); // 发送24位地址高8位
    spi_mosi((addr >> 8) & 0xFF);  // 发送24位地址中8位
    spi_mosi(addr & 0xFF);         // 发送24位地址低8位
    for (i = 0; i < size; i++)
    {
        spi_mosi(buffer[i]); // 发送数据
    }
    W25Q128_HW_SPI_CS = 1; // 取消选中
    while(HW_SPI_CheckFlashBusy()); // 等待忙状态结束 (确保页编程完成)
}
