#ifndef _PHY_ADIN_H
#define _PHY_ADIN_H

#include <include/utils.h>
#include "phylib.h"

#define ADIN1300_PHY_ID_1           0x283
#define ADIN1300_EXT_REG_PTR        0x10
#define ADIN1300_EXT_REG_DATA       0x11
#define ADIN1300_GE_RGMII_CFG       0xff23

/* RGMII internal delay settings for rx and tx for ADIN1300 */
#define ADIN1300_RGMII_1_60_NS      0x0001
#define ADIN1300_RGMII_1_80_NS      0x0002
#define ADIN1300_RGMII_2_00_NS      0x0000
#define ADIN1300_RGMII_2_20_NS      0x0006
#define ADIN1300_RGMII_2_40_NS      0x0007
#define ADIN1300_GE_RGMII_RXID_EN   BIT(2)
#define ADIN1300_GE_RGMII_TXID_EN   BIT(1)
#define ADIN1300_GE_RGMII_EN        BIT(0)

int adin1300_read_extended(const phy_t * phy, const int phy_reg, __u16 * value);
int adin1300_write_extended(const phy_t * phy, const int phy_reg, __u16 value);
int adin1300_verify_phy_mode(const phy_t * phy, const char * mode);

#endif
