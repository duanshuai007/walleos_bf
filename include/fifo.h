#ifndef _INCLUDE_FIFO_H_ 
#define _INCLUDE_FIFO_H_

#ifdef _cplusplus
extern "C" {
#endif

typedef struct FIFO32 {
	int *buf;
	volatile int p, q, size, free, flags;
	struct TTask *task;
}FIFO32;

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TTask *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

#ifdef __cplusplus
}
#endif

#endif
