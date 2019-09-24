/*
 * s5p4418-serial-stdio.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <s5p4418_serial_stdio.h>
#include <s5p4418_serial.h>
#include <synchronize.h>
#include <config.h>
#include <mutexes.h>
#include <smp_macros.h>
#include <alloc.h>

mutex_t printlock;

void initPrintLock(void)
{
    init_mutex(&printlock);
}

void lock_printf(void)
{
    lock_mutex(&printlock);
}

void unlock_printf(void)
{
    unlock_mutex(&printlock);
}

#ifdef SERIAL_FIFO

#define QUEUE_MAX_SIZE 1024

struct serial_msg s_msg;

void init_msg_queue(struct serial_msg *q)
{
    q->head = 0;
    q->tile = 0;
    q->buffer = malloc(sizeof(char) * QUEUE_MAX_SIZE);
}

void add_msg_queue(struct serial_msg *q, char *buf, int len)
{
    int i;
    for( i = 0; i < len; i++)
    {
        if(q->tile >= (QUEUE_MAX_SIZE - 1))
            q->tile = 0;
        q->buffer[q->tile] = *buf;
        q->tile++;
        buf++;
    }
}

int send_serial_msg(const char *fmt, ...)
{
    va_list ap;
    char buf[QUEUE_MAX_SIZE];
    int len;
    int rv;
    const int ch = 0;

    va_start(ap, fmt);
    rv = vsnprintf(buf, len + 1, fmt, ap);
    va_end(ap);

    rv = s5p4418_serial_write_string(ch, buf);
    return rv;
}

#endif  //SERIAL_FIFO

#undef LOCK

int printf(const char *fmt, ...)
{

#ifdef LOCK
    lock_mutex(&printlock);
#endif
    
    va_list ap;
    char buf[CONFIG_SYS_PBSIZE];

    //int len;
    int rv;
    const int ch = 0;

    va_start(ap, fmt);
    rv = vsnprintf(buf, CONFIG_SYS_PBSIZE, fmt, ap);
    va_end(ap);

    rv = (s5p4418_serial_write_string(ch, buf) < 0) ? 0 : rv;

#ifdef LOCK
    unlock_mutex(&printlock);
#endif

    return rv;
}

