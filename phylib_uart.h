#ifndef _PHY_UART_H
#define _PHY_UART_H

#define PHYLIB_BUF_SIZE 1000

int serial_read(char *buf, int max_len);
int serial_read_str(char *buf, int max_len);
int serial_write(const char *buf, int len);
int serial_write_str(const char *buf);
int serial_write_read_str(const char * tx_buf, char * rx_buf, int max_len);
int serial_open(const char *tty_dev, int speed, int waitTime);
void serial_close();
int serial_active();

#endif
