LOCAL_PATH := $(call my-dir)

PRIVATE_LOCAL_CFLAGS := -Wall

#
# var-mii
#

include $(CLEAR_VARS)
LOCAL_MODULE := var-mii
LOCAL_SRC_FILES := main.c \
		phy_adin1300.c \
		phy_ar803x.c \
		phylib.c \
		phylib_uart.c \
		phy_mxl86110.c

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(PRIVATE_LOCAL_CFLAGS)

include $(BUILD_EXECUTABLE)
