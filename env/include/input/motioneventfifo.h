#ifndef _ENV_INCLUDE_INPUT_TFT5406BUFFERFIFO_H_
#define _ENV_INCLUDE_INPUT_TFT5406BUFFERFIFO_H_

#define MAX_TFT5406BUFFER_FIFO_SIZE 1024

void initMotionEventFifo(void);
int putMotionEventFifo(void *data);
void *getMotionEventFifo(void);
int getMotionEventStatus(void);

#endif
