#ifndef __SYS_RTXC_H__
#define __SYS_RTXC_H__

#define CLOCKTICKS_PER_MS 10    //����ʱ�ӽ���

void init_lwip_timer(void);  //��ʼ��LWIP��ʱ��
u8_t timer_expired(u32_t *last_time,u32_t tmr_interval);	//��ʱ����ʱ�ж�

#endif /* __SYS_RTXC_H__ */

