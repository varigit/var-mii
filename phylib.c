
#include <linux/types.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/mii.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "phylib.h"
#include "phylib_uart.h"

static int _mii_read_reg(const phy_t * phy, const int phy_reg, __u16 * value) {
	struct ifreq ifr;
	int ret = 0;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, phy->if_name);

	struct mii_ioctl_data* mii = (struct mii_ioctl_data*)(&ifr.ifr_data);
	mii->phy_id  = phy->addr;
	mii->reg_num = phy_reg;
	mii->val_in  = 0;
	mii->val_out = 0;

	const int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd != -1)
	{
		if (ioctl(fd, SIOCGMIIREG, &ifr) != -1) {
			*value = mii->val_out;
			#if DEBUG
			printf("%s: %s phy@%d reg 0x%x = 0x%x\n", __func__, if_name, addr, phy_reg, mii->val_out);
			#endif
		} else {
			printf("%s: Error: %s phy@%d reg 0x%x ioctl failed\n", __func__, phy->if_name, phy->addr, phy_reg);
			ret = -2;
		}
		close(fd);
	} else {
			printf("%s: Error: %s phy@%d reg 0x%x socket failed\n", __func__, phy->if_name, phy->addr, phy_reg);
			ret = -1;
	}

	return ret;
}

static int _mii_write_reg(const phy_t * phy, const int phy_reg, const __u16 phy_val) {
	struct ifreq ifr;
	int ret = 0;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, phy->if_name);

	struct mii_ioctl_data* mii = (struct mii_ioctl_data*)(&ifr.ifr_data);
	mii->phy_id  = phy->addr;
	mii->reg_num = phy_reg;
	mii->val_in  = phy_val;
	mii->val_out = 0;

	const int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd != -1)
	{
		if (ioctl(fd, SIOCSMIIREG, &ifr) != -1) {
			#if DEBUG
			printf("%s: %s phy@%d reg 0x%x = 0x%x\n", __func__, if_name, addr, phy_reg, mii->val_in);
			#endif
		} else {
			printf("%s: Error: %s phy@%d reg 0x%x ioctl failed\n", __func__, phy->if_name, phy->addr, phy_reg);
			ret = -2;
		}
		close(fd);
	} else {
			printf("%s: Error: %s phy@%d reg 0x%x socket failed\n", __func__, phy->if_name, phy->addr, phy_reg);
			ret = -1;
	}

	return ret;
}

static int _mii_read_reg_uboot(const phy_t * phy, const int phy_reg, __u16 * value) {
	char tx_buf[PHYLIB_BUF_SIZE], rx_buf[PHYLIB_BUF_SIZE];
	snprintf(tx_buf, PHYLIB_BUF_SIZE, "mii read %x %x\r\n", phy->addr, phy_reg);
	serial_write_read_str(tx_buf, rx_buf, PHYLIB_BUF_SIZE);

	*value = (__u16) strtol(rx_buf, NULL, 16);
	// printf("%s@%d: %d = 0x%x (%s)\n", __func__, phy->addr, phy_reg, *value, rx_buf);
	return 0;
}

static int _mii_write_reg_uboot(const phy_t * phy, const int phy_reg, const __u16 phy_val) {
	char tx_buf[PHYLIB_BUF_SIZE];

	snprintf(tx_buf, PHYLIB_BUF_SIZE, "mii write %x %x %x\r\n", phy->addr, phy_reg, phy_val);
	return serial_write_str(tx_buf);
}

int mii_read_reg(const phy_t * phy, const int phy_reg, __u16 * value) {
	if (serial_active())
		return _mii_read_reg_uboot(phy, phy_reg, value);
	else
		return _mii_read_reg(phy, phy_reg, value);
}

int mii_write_reg(const phy_t * phy, const int phy_reg, const __u16 phy_val) {
	if (serial_active())
		return _mii_write_reg_uboot(phy, phy_reg, phy_val);
	else
		return _mii_write_reg(phy, phy_reg, phy_val);

}
