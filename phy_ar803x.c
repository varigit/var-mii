#include <linux/types.h>
#include <string.h>
#include <stdio.h>
#include "phy_ar803x.h"
#include "phylib.h"

int ar803x_read_extended(const phy_t * phy, const int phy_reg, __u16 * value) {
	mii_write_reg(phy, AR803x_PHY_DEBUG_ADDR_REG, phy_reg);
	return mii_read_reg(phy, AR803x_PHY_DEBUG_DATA_REG, value);
}

int ar803x_write_extended(const phy_t * phy, const int phy_reg, __u16 value) {
	mii_write_reg(phy, AR803x_PHY_DEBUG_ADDR_REG, phy_reg);
	return mii_write_reg(phy, AR803x_PHY_DEBUG_DATA_REG, value);
}

int ar803x_verify_phy_mode(const phy_t * phy, const char * mode) {
	__u16 value_r0, value_r5;
	int ret = 0;

	/* read rgmii regsiters */
	ar803x_read_extended(phy, AT803X_DEBUG_REG_0, &value_r0);
	ar803x_read_extended(phy, AT803X_DEBUG_REG_5, &value_r5);

	if (strcmp(mode, "rgmii") == 0) {
		if ((value_r5 & AT803X_DEBUG_TX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_TX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
		if ((value_r0 & AT803X_DEBUG_RX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_RX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-id") == 0) {
		if (!(value_r5 & AT803X_DEBUG_TX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_TX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
		if (!(value_r0 & AT803X_DEBUG_RX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_RX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-txid") == 0) {
		if (!(value_r5 & AT803X_DEBUG_TX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_TX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
		if ((value_r0 & AT803X_DEBUG_RX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_RX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-rxid") == 0) {
		if ((value_r5 & AT803X_DEBUG_TX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_TX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
		if (!(value_r0 & AT803X_DEBUG_RX_CLK_DLY_EN)) {
			printf("%s:\t\tError: GE_RGMII_RX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
	} else {
		printf("%s:\t\tError: unknown phy mode '%s'\n", __func__, mode);
		ret = -1;
	}

	if (!ret) {
		printf("%s:\t\tPASS: RGMII Mode = '%s' \n", __func__, mode);
	} else {
		printf("%s:\t\tFAIL: RGMII Mode != '%s' \n", __func__, mode);
	}

	return ret;
}

int ar803x_verify_vddio(const phy_t * phy, const __u16 vddio) {
	__u16 value;
	int ret = 0;

	// Don't care, return
	if (vddio == AT803X_VDDIO_DONTCARE)
		return 0;

	/* read vddio regsiter */
	ar803x_read_extended(phy, AR803x_DEBUG_REG_31, &value);

	if (vddio != (value & AT803X_VDDIO_1P8V)) {
		printf("%s:\t\tFAIL: AT803X_VDDIO_1P8V != %d\n", __func__, vddio == 0 ? 0 : 1);
		ret = -1;
	} else {
		printf("%s:\t\tPASS: AT803X_VDDIO_1P8V = %d\n", __func__, vddio == 0 ? 0 : 1);
	}

	return ret;
}
