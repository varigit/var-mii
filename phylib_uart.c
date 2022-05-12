#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "phylib_uart.h"

static int fd = -1;

int serial_active()
{
	return fd > 0;
}

int serial_read(char *buf, int max_len)
{
	buf[0] = 0;
	return read(fd, buf, max_len);
}

int serial_read_str(char *buf, int max_len)
{
	int len = 0, total_len = 0;
	buf[0] = 0;
	do {
		len = serial_read(buf + total_len, max_len - total_len);
		total_len += len;
	} while (len > 0);

	// printf("rx[%03d]: '%s'\n", total_len, buf);

	return total_len;
}

int serial_write(const char *buf, int len)
{
	tcflush(fd, TCIOFLUSH);
	len = write(fd, buf, len);
	tcdrain(fd);
	return len;
}

int serial_write_str(const char *buf)
{
	return serial_write(buf, strlen(buf));
}

int serial_write_read_str(const char * tx_buf, char * rx_buf, int max_len) {
	int len;
	char rx_buf_tmp[PHYLIB_BUF_SIZE];
	char * p_start, * p_end;

	rx_buf[0] = 0;
	serial_write_str(tx_buf);
	len = serial_read_str(rx_buf_tmp, max_len);

	if (len <= 0)
		return len;

	/* Remove U-Boot echo of transmit string */
	p_start = strstr(rx_buf_tmp, tx_buf);
	if (p_start)
		p_start += strlen(tx_buf);
	else
		p_start = rx_buf_tmp;

	/* Remove trailing U-Boot> prompt */
	p_end = strstr(p_start, "\r\n");
	if (p_end)
		*p_end = 0;

	return snprintf(rx_buf, max_len, "%s", p_start);
}

int serial_open(const char *tty_dev, int speed, int waitTime)
{
	int isBlockingMode;
	struct termios tty;

	if (fd >= 0)
	{
		printf("%s:%d Error: serial port already initialized\n", __func__, __LINE__);
		return -1;
	}

	isBlockingMode = 0;
	if (waitTime < 0 || waitTime > 255)
		isBlockingMode = 1;

	fd = open(tty_dev, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror(tty_dev);
		exit(-1);
	}

	isBlockingMode = 0;
	if (waitTime < 0 || waitTime > 255)
		isBlockingMode = 1;

	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0)
	{
		printf("%s:%d Error: %s\n", __func__, __LINE__, strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	cfmakeraw(&tty);
	tty.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode
	tty.c_cflag &= ~CSTOPB;		 // 1 stop bit
	tty.c_cflag &= ~CRTSCTS;	 // Disable hardware flow control
	tty.c_lflag &= ~ECHO;		 // Disable echo

	tty.c_cc[VMIN] = (1 == isBlockingMode) ? 1 : 0;		// read doesn't block
	tty.c_cc[VTIME] = (1 == isBlockingMode) ? 0 : waitTime; // in unit of 100 milli-sec for set timeout value

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		printf("%s:%d Error: %s\n", __func__, __LINE__, strerror(errno));
		return -1;
	}

	return fd;
}

void serial_close()
{
	if (fd > 0)
		close(fd);
}
