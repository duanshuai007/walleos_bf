#include <input/motioneventfifo.h>
#include <types.h>
#include <fifo.h>
#include <string.h>

static FIFO32 sTFT5406BufferFifo;
static int sTFT5406BufferBuff[MAX_TFT5406BUFFER_FIFO_SIZE];

void initMotionEventFifo(void)
{
    memset(&sTFT5406BufferFifo, 0, sizeof(FIFO32));
	fifo32_init(&sTFT5406BufferFifo, MAX_TFT5406BUFFER_FIFO_SIZE, sTFT5406BufferBuff, NULL);	
}

int putMotionEventFifo(void *data)
{
	return fifo32_put(&sTFT5406BufferFifo, (int)data);
}

void *getMotionEventFifo(void)
{
	int result = fifo32_get(&sTFT5406BufferFifo);

	if (result == -1)
	{
		return NULL;
	}

	return (void *)result;
}


int getMotionEventStatus(void)
{
	return fifo32_status(&sTFT5406BufferFifo);
}

