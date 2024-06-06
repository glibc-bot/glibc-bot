/*
 * TimerAndIO.h
 *
 *  Created on: 2022閿熸枻鎷�2閿熸枻鎷�26閿熸枻鎷�
 *      Author: Yue
 */

#ifndef DRIVER_TIMERANDIO_H_
#define DRIVER_TIMERANDIO_H_

#include <stdint.h>

#define MIO_PL_KO1		(480+11)
#define MIO_PL_KO2		(480+12)
#define MIO_PL_KO3		(480+13)
#define MIO_PL_KO4		(480+14)
#define MIO_PL_BUSEN	(480+15)

#define GPIO_EXPORT_PATH    "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH  "/sys/class/gpio/unexport"

/*
 * 名称	     ----------------------   基地址
 * KI  [4:1] ----------------------   0x4270_0000
 * {KO[9:5], KOL[4:1]} ------------   0x4280_0000
 * KO_DY  							  0x42b0_0000
 * */

enum reg_cfg {
	MULTIPLE_IO,  // 一个寄存器控制多个IO
	SINGLE_IO     // 一个寄存器控制一个IO
};

enum ko{
	ENUM_KOL,
	ENUM_KO_DY,
};

enum KO_DY {
	PL_BAT_K1,
	PL_HJDH_K2,
	PL_SJL_K3,
	PL_DYT_K4,
	PL_AL_K5,
	PL_YX_K6,
	PL_RK_K7,
	PL_GD_K8,
	RK_POWER_EN,
	RK_IN_RSTN,
	SSD_PERST,
	RK_PCIE_AWAKE,
};


enum KOL {
	KOL1 = 0,
	KOL2,
	KOL3,
	KOL4,
	KO5,
	KO6,
	KO7,
	KO8,
	KO9,
	KO1 = MIO_PL_KO1, 	// MIO11
	KO2,
	KO3,
	KO4,
	PL_BUSEN,  	// MIO15
};

enum KI {
	KI1 = 1,
	KI2,
	KI3,
	KI4,
};

enum ko_mode {
	LEVEL=0,
	PULSE
};

enum init_value {
    LOW = 0,  // IO初始值为低
    HIGH = 1  // IO初始值为高
};

void gpio_fpga_mmap();

void export_gpio(int gpio_number);

void set_gpio_direction(int gpio_number, const char *direction);

void write_gpio_value(int gpio_number, int value);

void unexport_gpio(int gpio_number);

// //////////////////////
void gpio_mode_init(enum reg_cfg reg_cfg, enum ko ko);

void gpio_kol_init(enum KOL kol, enum ko_mode mode, enum init_value value);

void gpio_ko_dy_init(enum KOL kol, enum ko_mode mode, enum init_value value);

uint8_t get_ki_state(enum KI ki);

void gpio_ki_init(enum KI ki, int debounce_time_10us);

void set_kol(enum KOL kol, uint8_t value);

void set_busen2(uint8_t value);

void set_ko_dy(enum KO_DY, uint8_t value);


#endif /* DRIVER_TIMERANDIO_H_ */
