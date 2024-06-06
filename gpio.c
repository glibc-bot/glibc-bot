#include "gpio.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>


// PS MIO GPIO操作的函数
void export_gpio(int gpio_number) {
    int fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Error opening GPIO export file");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "%d", gpio_number);

    close(fd);
}

void set_gpio_direction(int gpio_number, const char *direction) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio_number);

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("Error opening GPIO direction file");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "%s", direction);

    close(fd);
}

void write_gpio_value(int gpio_number, int value) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_number);

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("Error opening GPIO value file");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "%d", value);

    close(fd);
}

void unexport_gpio(int gpio_number) {
    int fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Error opening GPIO unexport file");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "%d", gpio_number);

    close(fd);
}

// PL GPIO操作函数
unsigned char *ko_dy_base_addr;
unsigned char *kol_base_addr;
unsigned char *ki_base_addr;
volatile void *fpga_phy_addr;

void gpio_fpga_mmap()
{
    // GP地址映射
    int mem_fd;

    if (-1 == (mem_fd = open("/dev/mem", O_RDWR | O_SYNC)))
    {
        perror("fpga_mem_map::open"), exit(-1);
    }

    ki_base_addr = mmap(NULL, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x42700000);

    kol_base_addr = mmap(NULL, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x42800000);

    ko_dy_base_addr = mmap(NULL, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x42b00000);
}

void gpio_mode_init(enum reg_cfg reg_cfg, enum ko ko)
{
	if (ENUM_KOL == ko) {
		*(uint32_t*)(kol_base_addr+0x10) = reg_cfg;

		// 导出gpio
		export_gpio(KO1);
		export_gpio(KO2);
		export_gpio(KO3);
		export_gpio(KO4);
		export_gpio(PL_BUSEN);

		set_gpio_direction(KO1, "out");
		set_gpio_direction(KO2, "out");
		set_gpio_direction(KO3, "out");
		set_gpio_direction(KO4, "out");
		set_gpio_direction(PL_BUSEN, "out");

		write_gpio_value(PL_BUSEN, 0);
	}
	else
		*(uint32_t*)(ko_dy_base_addr+0x10) = reg_cfg;
}

void gpio_kol_init(enum KOL kol, enum ko_mode mode, enum init_value value)
{
	// 模式设置
	*(uint32_t*)(kol_base_addr + 0x8000 + kol*0x10*4 + 0x00) = mode;
	*(uint32_t*)(kol_base_addr + 0x8000 + kol*0x10*4 + 0x04) = value;
	//*(uint32_t*)(kol_base_addr + 0x8000 + kol*0x10*4 + 0x08) = ;
}

void gpio_ki_init(enum KI ki, int debounce_time_10us)
{
	*(uint32_t*)(ki_base_addr + 0x10*ki*4) = debounce_time_10us;
}

uint8_t get_ki_state(enum KI ki)
{
	return ((*(uint32_t*)(ki_base_addr)) & (1 << ki)) >> ki;
}

void set_kol(enum KOL kol, uint8_t value)
{
	if (kol >= KOL1 && kol <= KO9)
	{
		*(uint32_t*)(kol_base_addr + kol*0x10*4 + 0x8000 + 0x0c) = value ? 0x55 : 0xaa;
	}
	else
	{
		write_gpio_value(kol, value);
	}
}

void set_ko_dy(enum KO_DY ko_dy, uint8_t value)
{
	*(uint32_t*)(ko_dy_base_addr + ko_dy*0x10*4 + 0x8000 + 0x0c) = value ? 0x55 : 0xaa;
}

void set_busen2(uint8_t value)
{
	*(uint32_t*)(kol_base_addr + 0x08) = value ? 0x55 : 0xaa;
}
