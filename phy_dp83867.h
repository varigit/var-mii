#ifndef _PHY_DP83867_H
#define _PHY_DP83867_H

#include "phylib.h"

#define DP83867_PHY_ID_1               0x2000

int dp83867_read_reg_ext(const phy_t * phy, const int phy_reg, __u16 * value);
int dp83867_write_reg_ext(const phy_t * phy, const int phy_reg, const __u16 phy_val);
int dp83867_verify_io_impedance(const phy_t * phy);

#endif
