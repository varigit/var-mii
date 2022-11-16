#include <linux/types.h>
#include <string.h>
#include <stdio.h>
#include "phy_adin1300.h"
#include "phylib.h"

int adin1300_verify_phy_mode(const phy_t * phy, const char * mode) {
	__u16 value;
	int ret = 0;

	/* read rgmii regsiter */
	mii_read_reg_ext(phy, ADIN1300_GE_RGMII_CFG, &value);

	if (!(value & ADIN1300_GE_RGMII_EN)) {
		printf("%s:\tError: ADIN1300_GE_RGMII_EN is disabled\n", __func__);
		ret = -1;
	}

	if (strcmp(mode, "rgmii") == 0) {
		if ((value & ADIN1300_GE_RGMII_TXID_EN)) {
			printf("%s:\tError: GE_RGMII_TX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
		if ((value & ADIN1300_GE_RGMII_RXID_EN)) {
			printf("%s:\tError: GE_RGMII_RX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-id") == 0) {
		if (!(value & ADIN1300_GE_RGMII_TXID_EN)) {
			printf("%s:\tError: GE_RGMII_TX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
		if (!(value & ADIN1300_GE_RGMII_RXID_EN)) {
			printf("%s:\tError: GE_RGMII_RX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-txid") == 0) {
		if (!(value & ADIN1300_GE_RGMII_TXID_EN)) {
			printf("%s:\tError: GE_RGMII_TX_ID_EN is disabled\n", __func__);
			ret = -1;
		}
		if ((value & ADIN1300_GE_RGMII_RXID_EN)) {
			printf("%s:\tError: GE_RGMII_RX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
	} else if (strcmp(mode, "rgmii-rxid") == 0) {
		if ((value & ADIN1300_GE_RGMII_TXID_EN)) {
			printf("%s:\tError: GE_RGMII_TX_ID_EN is enabled\n", __func__);
			ret = -1;
		}
		if (!(value & ADIN1300_GE_RGMII_RXID_EN)) {
			printf("%s:\tError: GE_RGMII_RX_ID_EN is disabled\n", __func__);
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

int adin1300_verify_clk_rcvr_125(const phy_t * phy, const int clk_rcvr_125_en) {
	__u16 value;
	int ret;

	/* read clock config regsiter */
	mii_read_reg_ext(phy, ADIN1300_GE_CLK_CFG, &value);

	/* verify clk_rcvr_125_en bit */
	if (clk_rcvr_125_en == (value & ADIN1300_GE_CLK_RCVR_125_EN)) {
		printf("%s:\tPASS: Clock Config clk_rcvr_125_en = '0x%x' \n",
		       __func__, clk_rcvr_125_en);
		ret = 0;
	} else {
		printf("%s:\tFAIL: Clock Config clk_rcvr_125_en = '0x%lx', expected '0x%x' \n",
		       __func__, value & ADIN1300_GE_CLK_RCVR_125_EN, clk_rcvr_125_en);
		ret = -1;
	}

	return ret;
}
