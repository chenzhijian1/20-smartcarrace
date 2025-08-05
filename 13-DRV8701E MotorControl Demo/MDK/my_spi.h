#ifndef __MY_SPI_H__
#define __MY_SPI_H__

#include "headfile.h"

#define   SPI_WRITE        0x06
#define   SPI_PAGE_WRITE   0x02
#define   SPI_EASRSE       0x20
#define   SPI_EASRSE_ALL   0x60
#define   SPI_BUSY         0X05
#define   SPI_READ         0x03

#define   W25Q128_HW_SPI_CS  P47



// uint8 my_spi_init(void);
void my_spi_init(void);

uint8  easrse_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l );

uint8  easrse_all_spi(void);

uint8 read_sr1(void);

uint8 read_sr2(void);

void read_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 size,uint8* array);

void write_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 size,uint8* array);

void all_write_spi(uint8 addr_h,uint8 addr_m,uint8 addr_l ,uint32 page,uint8* array);

void test_read_jedec_id(void);

void flash_disable_write_protection(void);

void detailed_flash_debug_gpio(void);

void final_flash_debug(void);

void final_solution_debug(void);

void check_protection_mode(void);

void check_quad_enable_bit(void);

void oscilloscope_debug_loop(void);

#endif