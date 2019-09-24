#ifndef _ENV_INCLUDE_S5P4418_SERIAL_H_
#define _ENV_INCLUDE_S5P4418_SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

enum uart_base{
    S5P4418_UART0_BASE = 0xC00A1000,
    S5P4418_UART1_BASE = 0xC00A0000,
    S5P4418_UART2_BASE = 0xC00A2000,
    S5P4418_UART3_BASE = 0xC00A3000,
};

enum uart_reg{
    UART_DATA   = 0x00,
    UART_RSR    = 0x04,
    UART_FR     = 0x18,
    UART_ILPR   = 0x20,
    UART_IBRD   = 0x24,
    UART_FBRD   = 0x28,
    UART_LCRH   = 0x2c,
    UART_CR     = 0x30,
    UART_IFLS   = 0x34,
    UART_IMSC   = 0x38,
    UART_RIS    = 0x3c,
    UART_MIS    = 0x40,
    UART_ICR    = 0x44,
    UART_DMACR  = 0x48,
};

enum uart_cr{
    UART_CTSEN  = 1 << 15,
    UART_RTSEN  = 1 << 14,
    UART_OUT2   = 1 << 13,
    UART_OUT1   = 1 << 12,
    UART_RTS    = 1 << 11,
    UART_DTR    = 1 << 10,
    UART_RXE    = 1 << 9,
    UART_TXE    = 1 << 8,
    UART_LBE    = 1 << 7,
    UART_SIRLP  = 1 << 2,
    UART_SIREN  = 1 << 1,
    UART_EN     = 1 << 0,
};

enum uart_rsr{
    UART_RSR_FE = 0x01,
    UART_RSR_PE = 0x02,
    UART_RSR_BE = 0x04,
    UART_RSR_OE = 0x08,
};

enum uart_fr{
    UART_FR_BUSY    = 0x08,
    UART_FR_RXFE    = 0x10,
    UART_FR_TXFF    = 0x20,
    UART_FR_RXFF    = 0x40,
    UART_FR_TXFE    = 0x80,
    UART_FR_RI      = 0x100,
};
    
enum serial_reg_offset{
    PEN_OFFSET  = 1,
    EPS_OFFSET  = 2,
    STP2_OFFSET = 3,
    FEN_OFFSET  = 4,
    WLEN_OFFSET = 5,
};

enum serial_no{
    S5P4418_SERIAL0 = 0,
    S5P4418_SERIAL1,
    S5P4418_SERIAL2,
    S5P4418_SERIAL3,
};

enum baud_rate_t {
    B50     = 0,
    B75,
    B110,
    B134,
    B200,
    B300,
    B600,
    B1200,
    B1800,
    B2400,
    B4800,
    B9600,
    B19200,
    B38400,
    B57600,
    B76800,
    B115200,
    B230400,
    B380400,
    B460800,
    B921600
};

enum data_bits_t {
    DATA_BITS_5     = 0,
    DATA_BITS_6,
    DATA_BITS_7,
    DATA_BITS_8,
};

enum parity_bits_t {
    PARITY_NONE     = 0,
    PARITY_ODD,
    PARITY_EVEN,
};

enum stop_bits_t {
    STOP_BITS_1     = 0,
    STOP_BITS_2,
    STOP_BITS_1_5,
};

void s5p4418_serial_initial(void);
boolean s5p4418_serial_setup(int ch, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop);
ssize_t s5p4418_serial_read(int ch, u8_t * buf, size_t count);
ssize_t s5p4418_serial_write(int ch, u8_t * buf, size_t count);
ssize_t s5p4418_serial_write_string(int ch, const char * buf);

ssize_t s5p4418_serial_write_buf(int ch, const char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __S5P4418_SERIAL_H__ */

