# VAR-MII

## Description

var-mii is a Utility that provides the following features:

1) Reading and writing ethernet phy mii registers in Linux
2) Automated test to very Variscite ethernet PHY registers in Linux
3) Automated test to verify Variscite ethernet PHY registers in U-Boot

## Reading registers from Linux

Run the following commands on the target device:

Read Registers:

```
./var-mii <interface> <address> <register>
```
Write Registers:
```
./var-mii <interface> <address> <register> <value>
```
## Automated tests in Linux

To run an automated test, run the following command on the target device:

```
./var-mii
```

## Automated tests in U-Boot

To run an automated test in U-Boot:

1) Connect your host computer to the target devices serial console
2) Boot the target device and interrupt the boot process on U-Boot
3) Quit minicom (or whichever program you use to connect to the serial console)
3) Run on the host computer:
```
$ ./var-mii <console serial device>
Example:
$ ./var-mii /dev/ttyUSB0
```

**Note:** It is important to quit minicom so that it doesn't conflict with var-mii
