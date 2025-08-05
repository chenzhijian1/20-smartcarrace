#ifndef __W25Q128_HW_H
#define __W25Q128_HW_H

#include "headfile.h" // 包含通用头文件，假设其中包含了基本类型定义

// W25Q128命令定义 (与软件SPI版本相同)
#define W25Q_WRITE_ENABLE       0x06
#define W25Q_VOLATILE_SR_WRITE  0x50
#define W25Q_WRITE_DISABLE      0x04
#define W25Q_READ_STATUS_REG1   0x05
#define W25Q_READ_STATUS_REG2   0x35
#define W25Q_READ_STATUS_REG3   0x15
#define W25Q_WRITE_STATUS_REG1  0x01
#define W25Q_WRITE_STATUS_REG2  0x31
#define W25Q_WRITE_STATUS_REG3  0x11
#define W25Q_READ_DATA          0x03
#define W25Q_FAST_READ          0x0B
#define W25Q_PAGE_PROGRAM       0x02
#define W25Q_SECTOR_ERASE       0x20
#define W25Q_BLOCK_ERASE_32K    0x52
#define W25Q_BLOCK_ERASE_64K    0xD8
#define W25Q_CHIP_ERASE         0xC7
#define W25Q_POWER_DOWN         0xB9
#define W25Q_RELEASE_POWER_DOWN 0xAB
#define W25Q_MANUFACTURER_ID    0x90
#define W25Q_JEDEC_ID           0x9F

// 状态寄存器位定义
#define W25Q_BUSY_BIT           0x01    // Status Register 1, Bit 0 (Write In Progress)
#define W25Q_WEL_BIT            0x02    // Status Register 1, Bit 1 (Write Enable Latch)

// 硬件SPI引脚定义 (使用通道一的默认引脚)
// 注意：CS引脚需要软件控制，这里只是一个示例，实际项目中可能需要根据具体硬件连接调整
#define W25Q128_HW_SPI_CS P47 // 假设使用P4.7作为硬件SPI的CS引脚，与gpio_mode保持一致

// 函数声明 (硬件SPI版本)
void HW_SPI_W25Q128_init(void);
void HW_SPI_FlashCheckID(void);
uint8 HW_SPI_CheckFlashBusy(void);
void HW_SPI_FlashWriteEnable(void);
void HW_SPI_FlashChipErase(void);
void HW_SPI_FlashSectorErase(uint32 addr);
void HW_SPI_Read_Nbytes(uint32 addr, uint8 *buffer, uint16 size);
uint8 HW_SPI_Read_Compare(uint32 addr, uint8 *buffer, uint16 size);
void HW_SPI_Write_Nbytes(uint32 addr, uint8 *buffer, uint16 size);

#endif // __W25Q128_HW_H
