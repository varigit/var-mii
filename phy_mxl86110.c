#include <linux/types.h>
#include <linux/kernel.h>
#include <string.h>
#include <stdio.h>
#include "phy_mxl86110.h"
#include "phylib.h"

#define GENMASK(msb, lsb) (((1UL << ((msb) - (lsb) + 1)) - 1) << (lsb))
#if defined(ffs)
#define FIELD_GET(mask, value) (((value) & (mask)) >> (ffs(mask) - 1))
#else
#define FIELD_GET(mask, value) (((value) & (mask)) >> (__builtin_ffs(mask) - 1))
#endif

/* RGMII register */
#define MXL8611X_EXT_RGMII_CFG1_REG1						0xA003
#define MXL8611X_EXT_RGMII_CFG1_NO_DELAY					0
#define MXL8611X_EXT_RGMII_CFG1_REG2						0xA004
#define MXL8611X_EXT_RGMII_CFG1_REG2_RGMII_SPEED			GENMASK(15, 14)

/* LED registers and defines */
#define MXL8611X_LED0_CFG_REG 0xA00C
#define MXL8611X_LED1_CFG_REG 0xA00D
#define MXL8611X_LED2_CFG_REG 0xA00E

int mxl86110_verify_phy_mode(const phy_t * phy, const char * mode) {
	__u16 value;
	int ret = 0;
	int rgmii_speed;

	if (strcmp(mode, "rgmii") == 0) {
		/* read rgmii cfg regsiter */
		mii_read_reg_ext(phy, MXL8611X_EXT_RGMII_CFG1_REG1, &value);
		if (value != MXL8611X_EXT_RGMII_CFG1_NO_DELAY) {
			printf("%s:\tError: MXL8611X_EXT_RGMII_CFG1_REG1 is not 0 (0x%x), mode is not %s\n", __func__, value, mode);
			ret = -1;
		}

		/* read rgmii status regsiter */
		mii_read_reg_ext(phy, MXL8611X_EXT_RGMII_CFG1_REG2, &value);
		rgmii_speed = FIELD_GET(MXL8611X_EXT_RGMII_CFG1_REG2_RGMII_SPEED, value);
		switch(rgmii_speed) {
			case 0:
				printf("%s:\tError: RGMII Speed is 10Mbps\n", __func__);
				ret = -1;
				break;
			case 1:
				printf("%s:\tError: RGMII Speed is 100Mbps\n", __func__);
				ret = -1;
				break;
			case 2:
				printf("%s:\tRGMII Speed is 1000Mbps\n", __func__);
				break;
			default:
				printf("%s:\tError: Unknown RGMII speed\n", __func__);
				ret = -1;
				break;
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

int mxl86110_verify_led_cfg(const phy_t * phy) {
	int ret = 0;
	__u16 led_cfg;

	/* read rgmii status regsiter */
	mii_read_reg_ext(phy, MXL8611X_LED0_CFG_REG, &led_cfg);
	if (led_cfg != 0x2600) {
		printf("%s:\tError: COM_EXT_LED0_CFG is 0x%x, it should be 0x%x\n", __func__, led_cfg, 0x2600);
		ret = -1;
	}

	/* read rgmii status regsiter */
	mii_read_reg_ext(phy, MXL8611X_LED1_CFG_REG, &led_cfg);
	if (led_cfg != 0x0070) {
		printf("%s:\tError: COM_EXT_LED1_CFG is 0x%x, it should be 0x%x\n", __func__, led_cfg, 0x0070);
		ret = -1;
	}

	if (!ret) {
		printf("%s:\tPASS: COM_EXT_LED0_CFG/COM_EXT_LED1_CFG\n", __func__);
	} else {
		printf("%s:\tFAIL: COM_EXT_LED0_CFG/COM_EXT_LED1_CFG\n", __func__);
	}

	return ret;
}
