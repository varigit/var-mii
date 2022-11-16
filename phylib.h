
#ifndef _PHYLIB_H
#define _PHYLIB_H

#include <linux/types.h>
#include <stdint.h>

#define BIT(nr)                 (1UL << (nr))

typedef struct {
	const char * if_name;
	uint8_t addr;
	uint32_t id; /* the target id used to match the correct phy_config*/
	uint32_t id_actual; /* the actual id read from mdio */
	const char * mode;
	int reg_extended_ptr;
	int reg_extended_data;
} phy_t;

int mii_read_reg(const phy_t * phy, const int phy_reg, __u16 * value);
int mii_write_reg(const phy_t * phy, const int phy_reg, const __u16 phy_val);
int mii_read_reg_ext(const phy_t * phy, const int phy_reg, __u16 * value);
int mii_write_reg_ext(const phy_t * phy, const int phy_reg, const __u16 phy_val);
/* initialize u-boot uart */
int phylib_uboot_uart_open(const char * uboot_tty);
void phylib_uboot_uart_close();
#endif
