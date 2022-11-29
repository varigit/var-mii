CC ?= gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = main.o phy_adin1300.o phy_ar803x.o phylib.o phylib_uart.o
TARGET = var-mii
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS);

clean:
	rm -f $(OBJFILES) $(TARGET)

cleanall: clean
	rm -f var-mii-32 var-mii-64
