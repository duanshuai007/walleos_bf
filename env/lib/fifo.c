/* FIFO */
#include <fifo.h>
#include <task.h>
#include <uspienv.h>
#include <s5p4418_serial_stdio.h>

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TTask *task)
/* FIFO缓冲区的初始化*/
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /*空*/
	fifo->flags = 0;
	fifo->p = 0; /*写入位置*/
	fifo->q = 0; /*读取位置*/
	fifo->task = task; /*有数据写入时需要唤醒的任务*/
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
/*向FIFO写入数据并累积起来*/
{
	if(fifo == NULL)
	{
		printf("fifo32_put:fifo == NULL\r\n");
		return -1;
	}

	if (fifo->free == 0) {

		fifo->flags |= FLAGS_OVERRUN;
        //write_uart("full fifo !!!\n");
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
    //u8 ucHIDButtons = (u8)((data>>16) & 0xff);
    //if (ucHIDButtons & 1)
    //{
    //    write_uart("left button..\n");
    //}
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {
		if (TaskGetState(fifo->task) == TaskStateBlocked) { /*如果任务处于休眠状态*/
			TaskSetState(fifo->task, TaskStateReady); /*将任务唤醒*/
            //if (!(ucHIDButtons & 1))
            //{
            //    write_uart("wakeup\n");
            //}
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
/*从FIFO取得一个数据*/
{
	int data;
	if (fifo->free == fifo->size) {
	/*当缓冲区为空的情况下返回-1*/
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
/*报告已经存储了多少数据*/
{
	return fifo->size - fifo->free;
}
