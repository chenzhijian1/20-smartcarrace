#ifndef __W25Q128_H
#define __W25Q128_H

#include "headfile.h" // 假设这个头文件包含了STC32G的GPIO定义和基本类型

// SPI引脚定义
#define SPI_SCLK P32
#define SPI_MISO P33
#define SPI_MOSI P34
#define SPI_CS P35

// W25Q128命令定义
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
#define W25Q_BUSY_BIT           0x01    // Status Register 1, Bit 0

// 函数声明
void W25Q128_SPI_init(void);
void FlashCheckID(void);
uint8 CheckFlashBusy(void);
void FlashWriteEnable(void);
void FlashChipErase(void);
void FlashSectorErase(uint32 addr);
void SPI_Read_Nbytes(uint32 addr, uint8 *buintffer, uint16 size);
uint8 SPI_Read_Compare(uint32 addr, uint8 *buintffer, uint16 size);
void SPI_Write_Nbytes(uint32 addr, uint8 *buintffer, uint8 size);

#endif // __W25Q128_H
