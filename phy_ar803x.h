#ifndef _PHY_ATH_H
#define _PHY_ATH_H

#include "phylib.h"

#define AR803x_PHY_ID_1             0x4d

#define AR803x_PHY_DEBUG_ADDR_REG   0x1d
#define AR803x_PHY_DEBUG_DATA_REG   0x1e

#define AT803X_DEBUG_REG_0          0x00
#define AT803X_DEBUG_RX_CLK_DLY_EN  BIT(15)

#define AT803X_DEBUG_REG_5          0x05
#define AT803X_DEBUG_TX_CLK_DLY_EN  BIT(8)

#define AR803x_DEBUG_REG_31         0x1f
#define AT803X_VDDIO_1P8V           BIT(3)
#define AT803X_VDDIO_DONTCARE       0xff

int ar803x_read_extended(const phy_t * phy, const int phy_reg, __u16 * value);
int ar803x_write_extended(const phy_t * phy, const int phy_reg, __u16 value);
int ar803x_verify_phy_mode(const phy_t * phy, const char * mode);
int ar803x_verify_vddio(const phy_t * phy, const __u16 vddio);

#endif
