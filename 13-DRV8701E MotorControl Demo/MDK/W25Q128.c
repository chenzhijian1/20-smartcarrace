#include "W25Q128.h"
#include "headfile.h" // 包含通用头文件，假设其中包含了GPIO操作和延时函数

// 软件SPI发送和接收一个字节
// tx_data: 要发送的数据
// 返回值: 接收到的数据
static uint8 SPI_ReadWriteByte(uint8 tx_data)
{
    uint8 i, rx_data = 0;

    for (i = 0; i < 8; i++)
    {
        // 发送数据位
        if ((tx_data & 0x80) == 0x80)
        {
            SPI_MOSI = 1;
        }
        else
        {
            SPI_MOSI = 0;
        }
        tx_data <<= 1; // 准备下一位

        SPI_SCLK = 1; // SCLK拉高
        delay_us(1);  // 延时，确保建立时间（根据实际SPI时序调整）

        // 接收数据位
        rx_data <<= 1;
        if (SPI_MISO)
        {
            rx_data |= 0x01;
        }
        SPI_SCLK = 0; // SCLK拉低
        delay_us(1);  // 延时，确保保持时间（根据实际SPI时序调整）
    }
    return rx_data;
}

// SPI初始化函数
void W25Q128_SPI_init(void)
{
    // 配置引脚模式
    // STC32G系列单片机GPIO配置通常通过PxM0和PxM1寄存器
    // PxM0 = 0, PxM1 = 0 : 高阻输入 (默认)
    // PxM0 = 1, PxM1 = 0 : 推挽输出
    // PxM0 = 0, PxM1 = 1 : 开漏输出
    // PxM0 = 1, PxM1 = 1 : 准双向口

    // SCLK (P32) - 推挽输出
    P3M0 |= (1 << 2);
    P3M1 &= ~(1 << 2);

    // MOSI (P34) - 推挽输出
    P3M0 |= (1 << 4);
    P3M1 &= ~(1 << 4);

    // CS (P35) - 推挽输出
    P3M0 |= (1 << 5);
    P3M1 &= ~(1 << 5);

    // MISO (P33) - 高阻输入
    P3M0 &= ~(1 << 3);
    P3M1 &= ~(1 << 3);

    // 初始状态
    SPI_CS = 1;   // CS拉高，不选中
    SPI_SCLK = 0; // SCLK拉低
    SPI_MOSI = 0; // MOSI拉低
}

// 以下是其他函数的占位符，将在后续步骤中实现

// 检查Flash ID
void FlashCheckID(void)
{
    uint8 manufacturer_id, device_id_high, device_id_low;

    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_JEDEC_ID); // 发送JEDEC ID命令
    manufacturer_id = SPI_ReadWriteByte(0xFF); // 读取制造商ID
    device_id_high = SPI_ReadWriteByte(0xFF);  // 读取设备ID高字节
    device_id_low = SPI_ReadWriteByte(0xFF);   // 读取设备ID低字节
    SPI_CS = 1; // 取消选中

    // 可以在这里添加打印或断点来验证ID
    // 例如：printf("W25Q128 ID: %02X %02X %02X\r\n", manufacturer_id, device_id_high, device_id_low);
}

// 检查Flash忙状态
uint8 CheckFlashBusy(void)
{
    uint8 status;
    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_READ_STATUS_REG1); // 发送读取状态寄存器1命令
    status = SPI_ReadWriteByte(0xFF); // 读取状态寄存器1内容
    SPI_CS = 1; // 取消选中
    return (status & W25Q_BUSY_BIT); // 返回BUSY位状态
}

// Flash写使能
void FlashWriteEnable(void)
{
    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_WRITE_ENABLE); // 发送写使能命令
    SPI_CS = 1; // 取消选中
    delay_us(1); // 短暂延时，确保命令执行
}

// Flash整片擦除
void FlashChipErase(void)
{
    FlashWriteEnable(); // 写使能
    while(CheckFlashBusy()); // 等待忙状态结束

    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_CHIP_ERASE); // 发送整片擦除命令
    SPI_CS = 1; // 取消选中

    while(CheckFlashBusy()); // 等待忙状态结束
}

// Flash扇区擦除
void FlashSectorErase(uint32 addr)
{
    FlashWriteEnable(); // 写使能
    while(CheckFlashBusy()); // 等待忙状态结束

    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_SECTOR_ERASE); // 发送扇区擦除命令
    SPI_ReadWriteByte((addr >> 16) & 0xFF); // 发送24位地址高8位
    SPI_ReadWriteByte((addr >> 8) & 0xFF);  // 发送24位地址中8位
    SPI_ReadWriteByte(addr & 0xFF);         // 发送24位地址低8位
    SPI_CS = 1; // 取消选中

    while(CheckFlashBusy()); // 等待忙状态结束
}

// SPI读取N字节数据
void SPI_Read_Nbytes(uint32 addr, uint8 *buffer, uint16 size)
{
    uint16 i;
    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_READ_DATA); // 发送读取数据命令
    SPI_ReadWriteByte((addr >> 16) & 0xFF); // 发送24位地址高8位
    SPI_ReadWriteByte((addr >> 8) & 0xFF);  // 发送24位地址中8位
    SPI_ReadWriteByte(addr & 0xFF);         // 发送24位地址低8位

    for (i = 0; i < size; i++)
    {
        buffer[i] = SPI_ReadWriteByte(0xFF); // 读取数据
    }
    SPI_CS = 1; // 取消选中
}

// SPI读取并比较数据
uint8 SPI_Read_Compare(uint32 addr, uint8 *buffer, uint16 size)
{
    uint16 i;
    // 临时缓冲区，假设最大页大小为256字节，如果需要读取更多数据，需要动态分配或分多次读取
    // W25Q128的页大小是256字节，所以这里使用256字节的临时缓冲区是合理的
    uint8 temp_buffer[256]; 

    // 确保读取大小不超过临时缓冲区，并避免溢出
    uint16 bytes_to_read = (size > 256) ? 256 : size; 

    SPI_Read_Nbytes(addr, temp_buffer, bytes_to_read); // 读取数据到临时缓冲区

    for (i = 0; i < bytes_to_read; i++)
    {
        if (temp_buffer[i] != buffer[i])
        {
            return 0; // 不匹配
        }
    }
    return 1; // 完全匹配
}

// SPI写入N字节数据
void SPI_Write_Nbytes(uint32 addr, uint8 *buffer, uint8 size)
{
    uint8 i;
    FlashWriteEnable(); // 写使能
    while(CheckFlashBusy()); // 等待忙状态结束

    SPI_CS = 0; // 选中芯片
    SPI_ReadWriteByte(W25Q_PAGE_PROGRAM); // 发送页编程命令
    SPI_ReadWriteByte((addr >> 16) & 0xFF); // 发送24位地址高8位
    SPI_ReadWriteByte((addr >> 8) & 0xFF);  // 发送24位地址中8位
    SPI_ReadWriteByte(addr & 0xFF);         // 发送24位地址低8位

    for (i = 0; i < size; i++)
    {
        SPI_ReadWriteByte(buffer[i]); // 发送数据
    }
    SPI_CS = 1; // 取消选中

    while(CheckFlashBusy()); // 等待忙状态结束
}
