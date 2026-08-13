#include <linux/types.h>
#include <string.h>
#include <stdio.h>
#include "phy_dp83867.h"
#include "phylib.h"


#define REG_REGCR               0x0d        /* Register Control Register */
#define REG_ADDAR               0x0e        /* Address or Data Register */

#define DP83867_DEVAD           0x1f        /* Vendor-defined address for DP83867 */

/* define in REG_REGCR how an access to REG_ADDAR is to be executed */
#define ADDAR_ADDR              0x0         /* set address */
#define ADDAR_DATA_NO_INC       0x4000      /* read / write data, don't inc address */
#define ADDAR_DATA_INC_WRRD     0x8000      /* read / write data, inc address on both read and write */
#define ADDAR_DATA_INC_WRONLY   0xC000      /* read / write data, inc address only when writing */

#define REG_IOMUX_CFG           0x0170
#define IOMUX_CFG_IMP_MASK      0x1f
#define IOMUX_CFG_MIN_IMP       0x1f        /* all bits set -> minimum impedance */
#define IOMUX_CFG_MAX_IMP       0x00        /* no  bit set  -> maximum impedance */

/* RGMII register */
#define DP83867_RGMIICTL                0x0032
#define DP83867_RGMII_EN                BIT(7)
#define DP83867_RGMII_TX_CLK_DELAY_EN   BIT(1)
#define DP83867_RGMII_RX_CLK_DELAY_EN   BIT(0)

/* For more details, please see DP83867 datasheet at https://www.ti.com/lit/ds/symlink/dp83867ir.pdf */


int dp83867_read_reg_ext(const phy_t * phy, const int phy_reg, __u16 * value) {
	/* Access to MDIO Extended Register Set as defined in the IEEE 802.3ah for
	 * accessing clause 45 extended register set via REGCR.
	 * Read, No Post Increment */

	mii_write_reg(phy, REG_REGCR, ADDAR_ADDR | DP83867_DEVAD);
	mii_write_reg(phy, REG_ADDAR, phy_reg);
	mii_write_reg(phy, REG_REGCR, ADDAR_DATA_NO_INC | DP83867_DEVAD);
	return mii_read_reg(phy, REG_ADDAR, value);
}

int dp83867_write_reg_ext(const phy_t * phy, const int phy_reg, const __u16 phy_val) {
	/* Access to MDIO Extended Register Set as defined in IEEE 802.3ah for
	 * accessing the clause 45 extended register set via REGCR.
	 * Write, No Post Increment */

	mii_write_reg(phy, REG_REGCR, ADDAR_ADDR | DP83867_DEVAD);
	mii_write_reg(phy, REG_ADDAR, phy_reg);
	mii_write_reg(phy, REG_REGCR, ADDAR_DATA_NO_INC | DP83867_DEVAD);
	return mii_write_reg(phy, REG_ADDAR, phy_val);
}

int dp83867_verify_phy_mode(const phy_t * phy, const char * mode) {
	__u16 value;
	int ret;

	/* read RGMIICTL register */
	ret = dp83867_read_reg_ext(phy, DP83867_RGMIICTL, &value);
	if (ret) {
		printf("%s:\tError: failed to read DP83867_RGMIICTL (%d)\n",
				__func__, ret);
		return ret;
	}

	ret = 0;

	if (!(value & DP83867_RGMII_EN)) {
		printf("%s:\tError: DP83867_RGMII_EN is disabled\n", __func__);
		ret = -1;
	}

	if (strcmp(mode, "rgmii") == 0) {
		if ((value & DP83867_RGMII_TX_CLK_DELAY_EN)) {
			printf("%s:\tError: DP83867_RGMII_TX_CLK_DELAY_EN is enabled\n", __func__);
			ret = -1;
		}
		if ((value & DP83867_RGMII_RX_CLK_DELAY_EN)) {
			printf("%s:\tError: DP83867_RGMII_RX_CLK_DELAY_EN is enabled\n", __func__);
			ret = -1;
		}
	} else {
		printf("%s:\tError: unknown phy mode '%s'\n", __func__, mode);
		ret = -1;
	}

	if (!ret) {
		printf("%s:\tPASS: RGMII Mode = '%s' \n", __func__, mode);
	} else {
		printf("%s:\tFAIL: RGMII Mode != '%s' \n", __func__, mode);
	}

	return ret;
}

int dp83867_verify_io_impedance(const phy_t * phy) {
	__u16 value;
	int ret = 0;

	value = dp83867_read_reg_ext(phy, REG_IOMUX_CFG, &value);
	dp83867_read_reg_ext(phy, REG_IOMUX_CFG, &value);
	value = value & IOMUX_CFG_IMP_MASK;
	if (value != IOMUX_CFG_MIN_IMP) {
		printf("%s:\tError: Impedance Control for MAC I/O not at minimum val, but at %x\n",
				__func__, value);
		ret = -1;
	} else {
		printf("%s:\tPASS: Impedance Control is at minimum value\n", __func__);
	}

	return ret;
}

