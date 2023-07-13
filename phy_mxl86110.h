#ifndef _PHY_MXL86110_H
#define _PHY_MXL86110_H

#include "phylib.h"

#define MXL86110_PHY_ID_1               0xC133
#define MXL86110_EXT_REG_PTR            0x1e
#define MXL86110_EXT_REG_DATA           0x1f

int mxl86110_verify_phy_mode(const phy_t * phy, const char * mode);
int mxl86110_verify_led_cfg(const phy_t * phy);
#endif
