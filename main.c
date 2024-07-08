#include <linux/types.h>
#include <linux/mii.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include "phylib.h"
#include "phylib_uart.h"
#include "phy_adin1300.h"
#include "phy_ar803x.h"
#include "phy_ksz9031.h"
#include "phy_mxl86110.h"

#define RET_IO_ERR -2
#define RET_UNKNOWN_PHY -3
#define RET_ERROR -3


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

machine_phyconfig_t machine_config_am62 = {
	.phy_count = 2,
	.phy_configs = {
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8mp = {
	.phy_count = 2,
	.phy_configs = {
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 1, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 4, .id = MXL86110_PHY_ID_1, .mode = "rgmii" }},
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
		{ .phy = { .if_name = "eth0", .addr = 0, .id = MXL86110_PHY_ID_1, .mode = "rgmii" }},
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

machine_phyconfig_t machine_config_imx93 = {
	.phy_count = 2,
	.phy_configs = {
		/* symphony */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = MXL86110_PHY_ID_1, .mode = "rgmii" }}, /* both imx93-var-som and imx93-var-dart */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 5, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx95 = {
	.phy_count = 2,
	.phy_configs = {
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = MXL86110_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth1", .addr = 1, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx8mq = {
	.phy_count = 1,
	.phy_configs = {
		/* dt8mcustomboard */
		{ .phy = { .if_name = "eth0", .addr = 0, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 0, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_DONTCARE },
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
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
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
		{ .phy = { .if_name = "eth1", .addr = 5, .id = ADIN1300_PHY_ID_1, .mode = "rgmii" }},
		{ .phy = { .if_name = "eth0", .addr = 4, .id = AR803x_PHY_ID_1,   .mode = "rgmii" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		{ .phy = { .if_name = "eth1", .addr = 5, .id = AR803x_PHY_ID_1,   .mode = "rgmii-rxid" }, .ar803_vddio = AT803X_VDDIO_1P8V },
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

machine_phyconfig_t machine_config_imx6ul = {
	.phy_count = 2,
	.phy_configs = {
		/* concerto */
		{ .phy = { .if_name = "eth0", .addr = 1, .id = KSZ9031_PHY_ID_1, .mode = "rmii-ref" }},
		{ .phy = { .if_name = "eth1", .addr = 3, .id = KSZ9031_PHY_ID_1, .mode = "rmii-ref" }},
		/* last entry */
		{ .phy = { .if_name = NULL }},
	},
};

static void usage(const char * err) {
	if (err)
		printf("Error: %s\n", err);

	printf("Usage:\n");
	printf("  var-mii supports multiple functions:\n");
	printf("  1) Run an automated test on the target, while the target is running Linux:\n");
	printf("    # ./var-mii\n");
	printf("  2) Run an automated test from a host, while the target is running U-Boot:\n");
	printf("    $ ./var-mii -d <console tty device>\n");
	printf("  3) Read/Write mii registers on the target, while the target is running Linux:\n");
	printf("    # ./var-mii -i <interface> -a <address> -r <register> -v <value> -e\n");
	printf("          interface: eth0, eth1, etc.\n");
	printf("          address: Phy Address\n");
	printf("          register: Optional argument to skip test and instead read a register\n");
	printf("          value: Optional argument to skip test and instead write a register\n\n");
	printf("          -e: Optional argument to indicate 'register / -r' is an extended register\n\n");
	printf("    Examples:\n\n");
	printf("          SOM:\t\t\t./var-mii -i eth0 -a 0x04 -r 0x2\n");
	printf("          Symphony:\t\t./var-mii -i eth1 -a 0x05 -r 0x2\n");
	printf("          DART:\t\t\t./var-mii -i eth0 -a 0x0  -r 0x2\n");
	printf("          DT8MCusbomboard:\t./var-mii -i eth1 -a 0x1  -r 0x2\n");
	printf("          SPEAR:\t\t./var-mii -i eth0 -a 0x0  -r 0x2\n");
	printf("          SP8Cusbomboard:\t./var-mii -i eth1 -a 0x1  -r 0x2\n");
	exit(1);
}

#define MACHINE_LEN 100
static char * get_soc_machine() {
	static char soc_machine[MACHINE_LEN] = "Unknown";
	char * ret;

	if (strcmp(soc_machine, "Unknown"))
		return soc_machine;

	if (serial_active()) {
		serial_write_read_str("echo $board_name", soc_machine, MACHINE_LEN);
	} else {
		FILE *f;

		f=fopen("/sys/devices/soc0/machine","r");
		if (!f) {
			printf("%s: Error: Could not open /sys/devices/soc0/machine\n", __func__);
			return soc_machine;
		}
		ret = fgets(soc_machine, MACHINE_LEN, f);

		if (!ret) {
			printf("%s: failed to read /sys/devices/soc0/machine\n", __func__);
		}

		fclose(f);
	}

	printf("%s:\t\t%s\n", __func__, soc_machine);

	return soc_machine;
}

static char * get_soc() {
	static char soc_name[MACHINE_LEN] = "Unknown";
	char * ret;

	if (strcmp(soc_name, "Unknown"))
		return soc_name;

	if (serial_active()) {
		serial_write_read_str("echo $soc_type", soc_name, MACHINE_LEN);
	} else {
		FILE *f;

		f=fopen("/sys/devices/soc0/soc_id","r");
		if (!f) {
			return soc_name;
		}
		ret = fgets(soc_name, MACHINE_LEN, f);

		if (!ret) {
			printf("%s: failed to read /sys/devices/soc0/soc_id\n", __func__);
		}

		fclose(f);
	}

	printf("%s:\t\t\t%s\n", __func__, soc_name);

	return soc_name;
}

static char * soc_arg = NULL;
static machine_phyconfig_t * get_machine_phyconfig() {
	char * machine = get_soc_machine();
	char * soc = get_soc();
	machine_phyconfig_t * machine_phyconfig = NULL;

	if(soc_arg)
		soc = soc_arg;

	/* Select phy configuration based on SoC name */
	if (strstr(soc, "i.MX8MQ") || strstr(soc, "imx8mq"))
		machine_phyconfig = &machine_config_imx8mq;
	else if (strstr(soc, "i.MX8MM") || strstr(soc, "imx8mm"))
		machine_phyconfig = &machine_config_imx8mm;
	else if (strstr(soc, "i.MX8MP") || strstr(soc, "imx8mp"))
		machine_phyconfig = &machine_config_imx8mp;
	else if (strstr(soc, "i.MX8MN") || strstr(soc, "imx8mn"))
		machine_phyconfig = &machine_config_imx8mn;
	else if (strstr(soc, "i.MX8QM") || strstr(soc, "imx8qm"))
		machine_phyconfig = &machine_config_imx8qm;
	else if (strstr(soc, "i.MX8QXP") || strstr(soc, "imx8qxp"))
		machine_phyconfig = &machine_config_imx8qx;
	else if (strstr(soc, "i.MX7D"))
		machine_phyconfig = &machine_config_imx7;
	else if (strstr(soc, "i.MX6UL") && !strstr(soc, "i.MX6ULZ")) {
		/* VAR-SOM-6UL on SymphonyBoard don't support on-board phy */
		if (strstr(machine, "Symphony") || strstr(machine, "symphony")) {
			machine_config_imx6ul.phy_count = 1;
			machine_config_imx6ul.phy_configs[1].phy.if_name = NULL;
		}
		machine_phyconfig = &machine_config_imx6ul;
	} else if (strstr(soc, "i.MX6") && !strstr(soc, "i.MX6ULZ"))
		machine_phyconfig = &machine_config_imx6dl;
	else if (strstr(soc, "i.MX93") || strstr(soc, "imx93"))
		machine_phyconfig = &machine_config_imx93;
	else if (strstr(soc, "i.MX95") || strstr(soc, "imx95"))
		machine_phyconfig = &machine_config_imx95;

	if (machine_phyconfig != NULL)
		return machine_phyconfig;

	/* Select phy configuration based on machine name
	   Todo: Migrate to use soc_id, which is more consistant then machine name */
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
	} else if (strstr(machine, "VAR-SOM-AM62")) {
		machine_phyconfig = &machine_config_am62;
	} else {
		printf("%s:\t\tFAIL: Could not find config for machine '%s'\n", __func__, machine);
	}

	return machine_phyconfig;
}

static int var_probe_phy_ids() {
	int i = 0;
	machine_phyconfig_t * machine_config = get_machine_phyconfig();

	if (machine_config == NULL)
		return RET_UNKNOWN_PHY;

	for (i = 0; machine_config->phy_configs[i].phy.if_name != NULL; i++) {
		__u16 phy_val = 0xffff;
		phyconfig_t * phy_config = &machine_config->phy_configs[i];

		/* Read the phy id */
		if (mii_read_reg(&phy_config->phy, MII_PHYSID1, &phy_val))
			return RET_IO_ERR;

		/* Save the phy id */
		phy_config->phy.id_actual = (uint32_t) phy_val;
	}

	return 0;
}

static int var_init_phy_extended_registers() {
	int i = 0;
	machine_phyconfig_t * machine_config = get_machine_phyconfig();

	if (machine_config == NULL)
		return RET_UNKNOWN_PHY;

	for (i = 0; machine_config->phy_configs[i].phy.if_name != NULL; i++) {
		phyconfig_t * phy_config = &machine_config->phy_configs[i];

		switch(phy_config->phy.id) {
			case ADIN1300_PHY_ID_1:
				phy_config->phy.reg_extended_ptr = ADIN1300_EXT_REG_PTR;
				phy_config->phy.reg_extended_data = ADIN1300_EXT_REG_DATA;
				break;
			case AR803x_PHY_ID_1:
				phy_config->phy.reg_extended_ptr = AR803x_PHY_DEBUG_ADDR_REG;
				phy_config->phy.reg_extended_data = AR803x_PHY_DEBUG_DATA_REG;
				break;
			case MXL86110_PHY_ID_1:
				phy_config->phy.reg_extended_ptr = MXL86110_EXT_REG_PTR;
				phy_config->phy.reg_extended_data = MXL86110_EXT_REG_DATA;
				break;
			default:
				phy_config->phy.reg_extended_ptr = -1;
				phy_config->phy.reg_extended_data = -1;
				printf("default\n");
		}
	}

	return 0;
}

static int var_init_phys() {
	if (var_init_phy_extended_registers()) {
		printf("Failed to intialize extended registers\n");
		return RET_ERROR;
	}

	if (var_probe_phy_ids()) {
		printf("Failed to probe phy ids\n");
		return RET_ERROR;
	}

	return 0;
}

/* return phy config given a if_name and addr */
static phyconfig_t * get_phy_config(const char * if_name, uint8_t addr) {
	int i = 0;
	machine_phyconfig_t * machine_config = get_machine_phyconfig();

	if (machine_config == NULL)
		return NULL;

	for (i = 0; machine_config->phy_configs[i].phy.if_name != NULL; i++) {
		phyconfig_t * phy_config = &machine_config->phy_configs[i];

		/* Check if phy id matches */
		if (phy_config->phy.id_actual != phy_config->phy.id)
			continue;

		/* Check if phy address matches */
		if (phy_config->phy.addr != addr)
			continue;

		/* Check if phy if_name matches */
		if (strcmp(phy_config->phy.if_name, if_name))
			continue;

		return phy_config;
	}

	return NULL;
}

static int var_verify_phys() {
	int i = 0;
	uint8_t verified_phys = 0;
	machine_phyconfig_t * machine_config = get_machine_phyconfig();

	if (machine_config == NULL) {
		return RET_ERROR;
	}

	for (i = 0; machine_config->phy_configs[i].phy.if_name != NULL; i++) {
		const phyconfig_t phy_config = machine_config->phy_configs[i];

		/* Check if phy id matches */
		if (phy_config.phy.id_actual != phy_config.phy.id) {
			DEBUG_PRINT("%s:not found: %s 0x%x@0x%x (0x%x)\n", __func__, phy_config.phy.if_name,
			       phy_config.phy.id, phy_config.phy.addr, phy_config.phy.id_actual);
			continue;
		}

		switch (phy_config.phy.id_actual) {
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
				printf("%s:\t\tPHY@%d: KSZ9031/KSZ8081\n", __func__, phy_config.phy.addr);
				/* For now, don't do any verification for KSZ9031/KSZ8081, just acknowledge it was found */
				verified_phys++;
				break;
			case MXL86110_PHY_ID_1:
				printf("%s:\t\tPHY@%d: MXL86110\n", __func__, phy_config.phy.addr);
				if (mxl86110_verify_phy_mode(&phy_config.phy, phy_config.phy.mode)) {
					continue;
				}
				if (mxl86110_verify_led_cfg(&phy_config.phy)) {
					continue;
				}
				verified_phys++;
				break;
			default:
				printf("%s:\t\tPHY@%d: Unknown ID: 0x%x\n", __func__, phy_config.phy.addr, phy_config.phy.id_actual);
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
	int phy_reg_is_extended = 0;
	__u16 phy_val = 0xffff;
	int c, index;

	memset(&phy, 0, sizeof(phy_t));
	phy.addr = 0xff;

	opterr = 0;

	while ((c = getopt (argc, argv, "hei:a:r:v:d:s:")) != -1) {
		switch (c){
		case 'h': /* help */
			usage(NULL);
			break;
		case 'i': /* interface */
			phy.if_name = optarg;
			break;
		case 'a': /* address */
			phy.addr = strtol(optarg, NULL, 16);
			break;
		case 'e': /* extended */
			phy_reg_is_extended = 1;
			break;
		case 'r': /* register */
			phy_reg = strtol(optarg, NULL, 16);
			break;
		case 'v': /* value */
			phy_val = strtol(optarg, NULL, 16);
			break;
		case 'd': /* U-Boot or Linux automated test */
			if (!strstr(optarg, "/dev/tty")) {
				usage("Invalid argument for -d");
			} else {
				serial_open(optarg, B115200, 1);
			}
			break;
		case 's': /* soc override */
			soc_arg = optarg;
			break;
		case '?':
			switch(optopt) {
			case 'i':
			case 'a':
			case 'r':
			case 'v':
			case 'd':
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				break;
			default:
				if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option or character `\\x%x'.\n", optopt);
			}
		default:
			usage(NULL);
		}
	}

	for (index = optind; index < argc; index++) {
		printf ("Error: Non-option argument %s\n", argv[index]);
		usage(NULL);
	}

	if (phy.if_name) {
		if (phy.addr == 0xff)
			usage("Missing phy address");
		if (phy.id == 0xffffffff)
			usage("Missing phy id");
		if (phy_reg == -1)
			usage("Missing phy register to read/write");
	}

	if (var_init_phys()) {
		printf("Failed to int phys\n");
		return RET_ERROR;
	}

	/*
	User is requesting a manual read/write of an extended register
	Try to identify the phy and get the extended registers from the phy
	configuration
	*/
	if (phy_reg_is_extended && (phy_val != 0xffff || phy_reg >= 0)) {
		phyconfig_t * phy_config = get_phy_config(phy.if_name, phy.addr);

		if (phy_config == NULL) {
			printf("Error: Could not get phy configuration to determine extended registers\n");
			return RET_ERROR;
		}

		memcpy(&phy, &phy_config->phy, sizeof(phy_t));
	}

	/* If value arg passed, write this register */
	if (phy_val != 0xffff) {
		if (!phy_reg_is_extended) {
			if (mii_write_reg(&phy, phy_reg, phy_val))
				return RET_IO_ERR;
		} else {
			if (mii_write_reg_ext(&phy, phy_reg, phy_val))
				return RET_IO_ERR;
		}
	}
	/* If register arg passed, read this register and exit */
	if (phy_reg >= 0) {
		if (!phy_reg_is_extended) {
			if (mii_read_reg(&phy, phy_reg, &phy_val))
				return RET_IO_ERR;
		} else {
			if (mii_read_reg_ext(&phy, phy_reg, &phy_val))
				return RET_IO_ERR;
		}
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

	if (serial_active())
		serial_close();

	return 0;
}
