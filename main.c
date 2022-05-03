#include <linux/types.h>
#include <linux/mii.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "phylib.h"
#include "phy_adin1300.h"
#include "phy_ar803x.h"
#include "phy_ksz9031.h"

#define RET_BAD_ARGS -1
#define RET_IO_ERR -2
#define RET_UNKNOWN_PHY -3
#define RET_ERROR -3


#define MACHINE_LEN 100
static char soc_machine[MACHINE_LEN];

#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

typedef struct {
	phy_t phy;
	uint8_t adin1300_rx_delay;          /* relevant only for rgmii-id, rgmii-rxid */
	uint8_t adin1300_tx_delay;          /* relevant only for rgmii-id, rgmii-txid */
	uint8_t clk_rcvr_125_en;            /* default = 0 = disabled */
	uint8_t ar803_vddio;                /* 1v5 or 1v8, default = 0 = 1v5 */
} phyconfig_t;

typedef struct {
	uint8_t phy_count;              /* number of phys expected */
	phyconfig_t phy_configs[];
} machine_phyconfig_t;

machine_phyconfig_t machine_config_imx8mp = {
	.phy_count = 2,
	.phy_configs = {
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 1, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 5, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8mm = {
	.phy_count = 1,
	.phy_configs = {
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8mn = {
	.phy_count = 1,
	.phy_configs = {
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx6dl = {
	.phy_count = 1,
	.phy_configs = {
		{ .phy = { .if_name = "eth0", .addr = 7, .id = ADIN1300_PHY_ID_1, .mode = "rgmii-id" }, .clk_rcvr_125_en = ADIN1300_GE_CLK_RCVR_125_EN },
		{ .phy = { .if_name = "eth0", .addr = 7, .id = KSZ9031_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx7 = {
	.phy_count = 2,
	.phy_configs = {
		/* mx7Customboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii-rxid" }},
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii-rxid" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii-id" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 1, .id = AR803x_PHY_ID_1,   .mode = "rgmii-id" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8qx = {
	.phy_count = 2,
	.phy_configs = {
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii-rxid" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 5, .id = AR803x_PHY_ID_1,   .mode = "rgmii-rxid" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8qm = {
	.phy_count = 2,
	.phy_configs = {
		/* sp8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 1, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii-rxid" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 5, .id = AR803x_PHY_ID_1,   .mode = "rgmii-rxid" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

static void usage() {
	printf("Usage:\n\n./var-mii <interface> <address> <register> <value>\n\n");
	printf("  interface: eth0, eth1, etc.\n");
	printf("  address: Phy Address\n");
	printf("  register: Optional argument to skip test and instead read a register\n");
	printf("  value: Optional argument to skip test and instead write a register\n");
	printf("\nExamples:\n\n");
	printf("SOM:\t\t\t./var-mii eth0 0x04 0x2\n");
	printf("Symphony:\t\t./var-mii eth1 0x05 0x2\n");
	printf("DART:\t\t\t./var-mii eth0 0x0 0x2\n");
	printf("DT8MCusbomboard:\t./var-mii eth1 0x1 0x2\n");
	printf("SPEAR:\t\t\t./var-mii eth0 0x0 0x2\n");
	printf("SP8Cusbomboard:\t./var-mii eth1 0x1 0x2\n");
}

static char * get_soc_machine() {
	FILE *f;
	char * ret;

	f=fopen("/sys/devices/soc0/machine","r");
	ret = fgets(soc_machine, MACHINE_LEN, f);

	if (!ret) {
		printf("%s: failed to read /sys/devices/soc0/machine\n", __func__);
	return ret;
	}

	fclose(f);

	printf("%s:\t\t%s\n", __func__, soc_machine);

	return ret;
}

static machine_phyconfig_t * get_machine_phyconfig() {
	char * machine = get_soc_machine();
	machine_phyconfig_t * machine_phyconfig = NULL;

	if (strstr(machine, "DART-MX8M-PLUS") || strstr(machine, "VAR-SOM-MX8M-PLUS")) {
		machine_phyconfig = &machine_config_imx8mp;
	} else if (strstr(machine, "DART-MX8M-MINI") || strstr(machine, "VAR-SOM-MX8M-MINI")) {
		machine_phyconfig = &machine_config_imx8mm;
	/* Old releases such as sumo have the machine name as VAR-SOM-MX8MN */
	} else if (strstr(machine, "VAR-SOM-MX8M-NANO") || strstr(machine, "VAR-SOM-MX8MN")) {
		machine_phyconfig = &machine_config_imx8mn;
	} else if (strstr(machine, "Variscite i.MX6 DL/Solo") ||
	          (strstr(machine, "Variscite i.MX6 DualLite/Solo VAR-SOM-DUAL/VAR-SOM-SOLO"))) {
		machine_phyconfig = &machine_config_imx6dl;
	} else if (strstr(machine, "Variscite i.MX7")) {
		machine_phyconfig = &machine_config_imx7;
	} else if (strstr(machine, "VAR-SOM-MX8X")) {
		machine_phyconfig = &machine_config_imx8qx;
	} else if (strstr(machine, "SPEAR-MX8") || strstr(machine, "VAR-SOM-MX8")) {
		machine_phyconfig = &machine_config_imx8qm;
	} else {
		printf("%s:\t\tFAIL: Could not find config for machine '%s'\n", __func__, machine);
	}

	return machine_phyconfig;
}

static int var_verify_phys() {
	int i = 0;
	uint8_t verified_phys = 0;
	machine_phyconfig_t * machine_config = get_machine_phyconfig();

	if (machine_config == NULL) {
		return RET_ERROR;
	}

	for (i = 0; machine_config->phy_configs[i].phy.if_name != NULL; i++) {
		__u16 phy_val = 0xffff;
		const phyconfig_t phy_config = machine_config->phy_configs[i];

		/* Check if phy id matches */
		if (mii_read_reg(&phy_config.phy, MII_PHYSID1, &phy_val))
			return RET_IO_ERR;

		if (phy_val != phy_config.phy.id) {
			DEBUG_PRINT("%s:not found: %s 0x%x@0x%x (0x%x)\n", __func__, phy_config.phy.if_name,
			       phy_config.phy.id, phy_config.phy.addr, phy_val);
			continue;
		}

		switch (phy_val) {
			case ADIN1300_PHY_ID_1:
				printf("%s:\t\tPHY@%d: ADIN1300\n", __func__, phy_config.phy.addr);
				if (adin1300_verify_phy_mode(&phy_config.phy, phy_config.phy.mode)) {
					continue;
				}
				if (adin1300_verify_clk_rcvr_125(&phy_config.phy, phy_config.clk_rcvr_125_en)) {
					continue;
				}
				verified_phys++;
				break;
			case AR803x_PHY_ID_1:
				printf("%s:\t\tPHY@%d: AR8033\n", __func__, phy_config.phy.addr);
				if (ar803x_verify_phy_mode(&phy_config.phy, phy_config.phy.mode)) {
					continue;
				}
				if (ar803x_verify_vddio(&phy_config.phy, phy_config.ar803_vddio)) {
					continue;
				}
				verified_phys++;
				break;
			case KSZ9031_PHY_ID_1:
				printf("%s:\t\tPHY@%d: KSZ9031\n", __func__, phy_config.phy.addr);
				/* For now, don't do any verification for KSZ9031, just acknowledge it was found */
				verified_phys++;
				break;
			default:
				printf("%s:\t\tPHY@%d: Unknown ID: 0x%x\n", __func__, phy_config.phy.addr, phy_val);
				return RET_UNKNOWN_PHY;
				break;
		}
	}

	if (verified_phys == machine_config->phy_count) {
		printf("%s:\t\tPASS: verified %d PHYs\n", __func__, machine_config->phy_count);
	} else {
		printf("%s:\t\tFAIL: verified %d / %d PHYs\n", __func__, verified_phys, machine_config->phy_count);
	}

	return !(verified_phys == machine_config->phy_count);
}

int main(int argc, char *argv[])
{
	phy_t phy;
	int phy_reg = -1;
	__u16 phy_val = 0xffff;

	/* parse args */
	if (argc == 3) {
		phy.if_name = argv[1];
		phy.addr = strtol(argv[2], NULL, 0);
	} else if (argc == 4) {
		phy.if_name = argv[1];
		phy.addr = strtol(argv[2], NULL, 0);
		phy_reg = strtol(argv[3], NULL, 0);
	} else if (argc == 5) {
		phy.if_name = argv[1];
		phy.addr = strtol(argv[2], NULL, 0);
		phy_reg = strtol(argv[3], NULL, 0);
		phy_val = strtol(argv[4], NULL, 0);
	} else if (argc == 2) {
		usage();
		return RET_BAD_ARGS;
	}

	/* If value arg passed, write this register */
	if (phy_val != 0xffff) {
		if (mii_write_reg(&phy, phy_reg, phy_val))
			return RET_IO_ERR;
	}
	/* If register arg passed, read this register and exit */
	if (phy_reg >= 0) {
		if (mii_read_reg(&phy, phy_reg, &phy_val))
			return RET_IO_ERR;
		printf("%x\n", phy_val);
		return 0;
	}

	/* Run Variscite PHY verification */
	if (var_verify_phys()) {
		printf("Results: Fail\n");
		return RET_ERROR;
	} else {
		printf("Results: Success\n");
	}

	return 0;
}
