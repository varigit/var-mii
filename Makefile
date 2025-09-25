CC ?= gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = main.o phy_adin1300.o phy_ar803x.o phy_mxl86110.o phylib.o phylib_uart.o
OBJFILES = main.o phy_adin1300.o phy_ar803x.o phy_mxl86110.o phylib.o phylib_uart.o phy_dp83867.o
TARGET = var-mii
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS);

clean:
	rm -f $(OBJFILES) $(TARGET)

cleanall: clean
	rm -f var-mii-32 var-mii-64
