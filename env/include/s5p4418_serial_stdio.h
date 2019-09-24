#ifndef _ENV_INCLUDE_S5P4418_SERIAL_STDIO_H__
#define _ENV_INCLUDE_S5P4418_SERIAL_STDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

typedef struct serial_msg
{
    u8 *buffer;
    u32 head;
    u32 tile;
}tSerialMsg;

void initPrintLock(void);
void lock_printf(void);
void unlock_printf(void);

int printf(const char *fmt, ...);

void init_msg_queue(struct serial_msg *q);
int send_serial_msg(const char *fmt, ...);
void add_msg_queue(struct serial_msg *q, char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __S5P4418_SERIAL_STDIO_H__ */
