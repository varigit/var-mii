#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/mii.h>
#include <linux/sockios.h>

#define DEBUG 1
#define RET_BAD_ARGS -1
#define RET_IO_ERR -2
#define RET_UNKNOWN_PHY -3
#define RET_ERROR -3

#define AR803x_PHY_ID_1             0x4d
#define AR803x_PHY_DEBUG_ADDR_REG   0x1d
#define AR803x_PHY_DEBUG_DATA_REG   0x1e
#define AR803x_DEBUG_REG_0          0x00
#define AR803x_DEBUG_REG_31         0x1f

#define ADIN1300_PHY_ID_1           0x283
#define ADIN1300_EXT_REG_PTR        0x10
#define ADIN1300_EXT_REG_DATA       0x11
#define ADIN1300_GE_RGMII_CFG       0xff23

void usage() {
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
}

int mii_read_reg(const char * if_name, const int phy_addr, const int phy_reg, __u16 * value) {
    struct ifreq ifr;
    int ret = 0;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);

    struct mii_ioctl_data* mii = (struct mii_ioctl_data*)(&ifr.ifr_data);
    mii->phy_id  = phy_addr;
    mii->reg_num = phy_reg;
    mii->val_in  = 0;
    mii->val_out = 0;

    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd != -1)
    {
        if (ioctl(fd, SIOCGMIIREG, &ifr) != -1) {
            *value = mii->val_out;
            #if DEBUG
            printf("%s: %s phy@%d reg 0x%x = 0x%x\n", __func__, if_name, phy_addr, phy_reg, mii->val_out);
            #endif
        } else {
            printf("%s: Error: %s phy@%d reg 0x%x ioctl failed\n", __func__, if_name, phy_addr, phy_reg);
            ret = -2;
        }
        close(fd);
    } else {
            printf("%s: Error: %s phy@%d reg 0x%x socket failed\n", __func__, if_name, phy_addr, phy_reg);
            ret = -1;
    }

    return ret;
}

int mii_write_reg(const char * if_name, const int phy_addr, const int phy_reg, const __u16 phy_val) {
    struct ifreq ifr;
    int ret = 0;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);

    struct mii_ioctl_data* mii = (struct mii_ioctl_data*)(&ifr.ifr_data);
    mii->phy_id  = phy_addr;
    mii->reg_num = phy_reg;
    mii->val_in  = phy_val;
    mii->val_out = 0;

    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd != -1)
    {
        if (ioctl(fd, SIOCSMIIREG, &ifr) != -1) {
            #if DEBUG
            printf("%s: %s phy@%d reg 0x%x = 0x%x\n", __func__, if_name, phy_addr, phy_reg, mii->val_in);
            #endif
        } else {
            printf("%s: Error: %s phy@%d reg 0x%x ioctl failed\n", __func__, if_name, phy_addr, phy_reg);
            ret = -2;
        }
        close(fd);
    } else {
            printf("%s: Error: %s phy@%d reg 0x%x socket failed\n", __func__, if_name, phy_addr, phy_reg);
            ret = -1;
    }

    return ret;
}

int adin1300_read_extended(const char * if_name, const int phy_addr, const int phy_reg, __u16 * value) {
    mii_write_reg(if_name, phy_addr, ADIN1300_EXT_REG_PTR, phy_reg);
    return mii_read_reg(if_name, phy_addr, ADIN1300_EXT_REG_DATA, value);
}

int adin1300_write_extended(const char * if_name, const int phy_addr, const int phy_reg, __u16 value) {
    mii_write_reg(if_name, phy_addr, ADIN1300_EXT_REG_PTR, phy_reg);
    return mii_write_reg(if_name, phy_addr, ADIN1300_EXT_REG_DATA, value);
}

int ar803x_read_extended(const char * if_name, const int phy_addr, const int phy_reg, __u16 * value) {
    mii_write_reg(if_name, phy_addr, AR803x_PHY_DEBUG_ADDR_REG, phy_reg);
    return mii_read_reg(if_name, phy_addr, AR803x_PHY_DEBUG_DATA_REG, value);
}

int ar803x_write_extended(const char * if_name, const int phy_addr, const int phy_reg, __u16 value) {
    mii_write_reg(if_name, phy_addr, AR803x_PHY_DEBUG_ADDR_REG, phy_reg);
    return mii_write_reg(if_name, phy_addr, AR803x_PHY_DEBUG_DATA_REG, value);
}

int verify_adin1300(const char * if_name, const int phy_addr) {
    __u16 value;
    int ret = 0;

    adin1300_read_extended(if_name, phy_addr, 0xFF23, &value);
    if(value & (1 << 1)) {
        printf("%s: Error: GE_RGMII_TX_ID_EN is enabled\n");
        ret = -1;
    }
    if(value & (1 << 2)) {
        printf("%s: Error: GE_RGMII_RX_ID_EN is enabled\n");
        ret = -1;
    }

    if(ret)
        printf("%s: failed\n", __func__);
    else
        printf("%s: passed\n", __func__);

    return ret;
}

int verify_ar803x(const char * if_name, const int phy_addr) {
    __u16 value;
    int ret = 0;

    ar803x_read_extended(if_name, phy_addr, AR803x_DEBUG_REG_0, &value);
    if(value & (1 << 15)) {
        printf("%s: Error: SEL_CLK125M_DSP set, rx delay is enabled\n");
        ret = -1;
    }

    ar803x_read_extended(if_name, phy_addr, AR803x_DEBUG_REG_31, &value);
    if(!(value & (1 << 3))) {
        printf("%s: Error: SEL_1P5_1P8_POS_REG not set, voltage is 1.5v\n");
        ret = -1;
    }

    if(ret)
        printf("%s: failed\n", __func__);
    else
        printf("%s: passed\n", __func__);

    return ret;
}

int main(int argc, char *argv[])
{
    struct ifreq ifr;
    const char * if_name;
    int phy_addr, phy_reg = -1;
    __u16 phy_val = 0xffff;

    /* parse args */
    if(argc == 3) {
        if_name = argv[1];
        phy_addr = strtol(argv[2], NULL, 0);
    } else if(argc == 4) {
        if_name = argv[1];
        phy_addr = strtol(argv[2], NULL, 0);
        phy_reg = strtol(argv[3], NULL, 0);
    } else if(argc == 5) {
        if_name = argv[1];
        phy_addr = strtol(argv[2], NULL, 0);
        phy_reg = strtol(argv[3], NULL, 0);
        phy_val = strtol(argv[4], NULL, 0);
    } else {
        usage();
        return RET_BAD_ARGS;
    }

    /* If value arg passed, write this register */
    if(phy_val != 0xffff) {
        if(mii_write_reg(if_name, phy_addr, phy_reg, phy_val))
            return RET_IO_ERR;
    }
    /* If register arg passed, read this register and exit */
    if(phy_reg >= 0) {
        if(mii_read_reg(if_name, phy_addr, phy_reg, &phy_val))
            return RET_IO_ERR;
        printf("%x\n", phy_val);
        return 0;
    }

    /* Run Variscite PHY verification */
    if(mii_read_reg(if_name, phy_addr, MII_PHYSID1, &phy_val))
        return RET_IO_ERR;

    switch(phy_val) {
        case ADIN1300_PHY_ID_1:
            printf("PHY@%d: ADIN1300\n", phy_addr);
            verify_adin1300(if_name, phy_addr);
            break;
        case AR803x_PHY_ID_1:
            printf("PHY@%d: AR8033\n", phy_addr);
            verify_ar803x(if_name, phy_addr);
            break;
    }

    return 0;
}
