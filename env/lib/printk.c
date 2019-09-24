#include <types.h>
#include <stdio.h>
#include <stdarg.h>
#include <config.h>
#include <string.h>
#include <timer.h>
#include <div64.h>
#include <limits.h>
#include <headsmp.h>
#include <irqflags.h>
#include <global_timer.h>
#include <s5p4418_serial.h>

#define CONFIG_LOG_BUF_SHIFT	17
#define __LOG_BUF_LEN   (1 << CONFIG_LOG_BUF_SHIFT)

#define LOG_BUF_MASK (log_buf_len-1)
#define LOG_BUF(idx) (log_buf[(idx) & LOG_BUF_MASK])

#define SYSLOG_PRI_MAX_LENGTH	5

#define CONFIG_DEFAULT_MESSAGE_LOGLEVEL		4
/* printk's without a loglevel use this.. */
#define DEFAULT_MESSAGE_LOGLEVEL CONFIG_DEFAULT_MESSAGE_LOGLEVEL

/* We show everything that is MORE important than this.. */
#define MINIMUM_CONSOLE_LOGLEVEL 1 /* Minimum loglevel we let people use */
#define DEFAULT_CONSOLE_LOGLEVEL 7 /* anything MORE serious than KERN_DEBUG */

int console_printk[4] = {
	DEFAULT_CONSOLE_LOGLEVEL,   /* console_loglevel */
	DEFAULT_MESSAGE_LOGLEVEL,   /* default_message_loglevel */
	MINIMUM_CONSOLE_LOGLEVEL,   /* minimum_console_loglevel */
	DEFAULT_CONSOLE_LOGLEVEL,   /* default_console_loglevel */
};

#define default_message_loglevel console_printk[1]

static raw_spinlock_t logbuf_lock;
/*
    * The indices into log_buf are not constrained to log_buf_len - they
	 * must be masked before subscripting
	  */
static unsigned log_start;  /* Index into log_buf: next char to be read by syslog() */
static unsigned con_start;  /* Index into log_buf: next char to be sent to consoles */
static unsigned log_end;    /* Index into log_buf: most-recently-written-char + 1 */

static char __log_buf[__LOG_BUF_LEN];
static char *log_buf = __log_buf;
static int log_buf_len = __LOG_BUF_LEN;
static unsigned logged_chars; /* Number of chars produced since last read+clear operation */

static int printk_cpu = UINT_MAX;
static int console_locked; //console_suspended;
static int new_text_line = 1;
static bool_t printk_time = 1;

static int vprintk(const char *fmt, va_list args);

struct semaphore console_sem;
struct current *current;

#define lockdep_recursing(tsk)  ((tsk)->lockdep_recursion)

void console_unlock(void);

static inline void logbuf_lock_init(void)
{
	logbuf_lock.raw_lock.lock = 0;
}

static inline void sem_init(void)
{
	console_sem.count = 1;
}

static inline void lockdep_init(void)
{
	current->lockdep_recursion = 0;
}

void printk_init(void)
{
	sem_init();
	lockdep_init();
	logbuf_lock_init();
}

void lockdep_off(void)
{
	current->lockdep_recursion++;
}

void lockdep_on(void)
{
	current->lockdep_recursion--;
}

int printk(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);
	
	return r;
}

void up(struct semaphore *sem)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&sem->lock, flags);
	sem->count = 1;
	raw_spin_unlock_irqrestore(&sem->lock, flags);
}

int down_trylock(struct semaphore *sem)
{
	unsigned long flags;
	int count;

	raw_spin_lock_irqsave(&sem->lock, flags);
	count = sem->count - 1;
	if (count >= 0) 
		sem->count = count;
	raw_spin_unlock_irqrestore(&sem->lock, flags);

	return (count < 0); 
}

int console_trylock(void)
{
	if (down_trylock(&console_sem))
		return 0;
	console_locked = 1;
	return 1;
}

//#define have_callable_console(x)	...	

//static inline int can_use_console(unsigned int cpu)
//{
//	//return cpu_online(cpu) || have_callable_console();
//	//printf("can_use_console(%d) = %d\r\n", cpu, have_callable_console(cpu));
//	return have_callable_console(cpu);
//}

static int console_trylock_for_printk(unsigned int cpu) 
{
    UNUSED(cpu);
	int retval = 0;

	if (console_trylock()) {
		retval = 1; 
		/*   
		 * If we can't use the console, we need to release
		 * the console semaphore by hand to avoid flushing
		 * the buffer. We need to hold the console semaphore
		 * in order to do this test safely.
		 */
	//	if (!can_use_console(cpu)) {
	//		console_locked = 0; 
	//		retval = 0; 
	//	}    
	}    
	printk_cpu = UINT_MAX;
	//raw_spin_unlock(&logbuf_lock);
	return retval;
}


static const char recursion_bug_msg [] = 
        "BUG: recent printk recursion!\n";
static int recursion_bug;
static char printk_buf[1024];

#define smp_processor_id()	0

static void emit_log_char(char c)
{
	LOG_BUF(log_end) = c; 
	log_end++;
	if (log_end - log_start > (unsigned)log_buf_len)
		log_start = log_end - log_buf_len;
	if (log_end - con_start > (unsigned)log_buf_len)
		con_start = log_end - log_buf_len;
	if (logged_chars < (unsigned)log_buf_len)
		logged_chars++;
}

static size_t log_prefix(const char *p, unsigned int *level, char *special)
{
	unsigned int lev = 0; 
	char sp = '\0';
	size_t len; 

	if (p[0] != '<' || !p[1])
		return 0;
	if (p[2] == '>') {
		/* usual single digit level number or special char */
		switch (p[1]) {
			case '0' ... '7': 
				lev = p[1] - '0'; 
				break;
			case 'c': /* KERN_CONT */
			case 'd': /* KERN_DEFAULT */
				sp = p[1];
				break;
			default:
				return 0;
		}    
		len = 3; 
	} else {
		/* multi digit including the level and facility number */
		char *endp = NULL;

		lev = (simple_strtoul(&p[1], &endp, 10) & 7);
		if (endp == NULL || endp[0] != '>') 
			return 0;
		len = (endp + 1) - p; 
	}    

	/* do not accept special char if not asked for */
	if (sp && !special)
		return 0;

	if (special) {
		*special = sp;
		/* return special char, do not touch level */
		if (sp) 
			return len; 
	}    

	if (level)
		*level = lev; 
	return len; 
}

static int vprintk(const char *fmt, va_list args)
{
	int cpu = get_cpuid();
	int printed_len = 0;
	int current_log_level = default_message_loglevel;
	unsigned long flags;
	int this_cpu;
	char *p;
	size_t plen;
	char special;

	local_irq_save(flags);
	//this_cpu = smp_processor_id();
	this_cpu = cpu;

	if(printk_cpu == this_cpu)
	{		//如果当前cpu正在打印，则退出printk
		/*   
		 * If a crash is occurring during printk() on this CPU,
		 * then try to get the crash message out but make sure
		 * we can't deadlock. Otherwise just return to avoid the
		 * recursion and return - but flag the recursion so that
		 * it can be printed at the next appropriate moment:
		 */
		//if (!oops_in_progress && !lockdep_recursing(current)) {
		//	recursion_bug = 1; 
		if(!lockdep_recursing(current))
		{
			recursion_bug = 1;
			goto out_restore_irqs;
		}    
		//zap_locks();
	}  

	lockdep_off();
	raw_spin_lock(&logbuf_lock);
	printk_cpu = this_cpu;

	if (recursion_bug) {
		recursion_bug = 0; 
		strcpy(printk_buf, recursion_bug_msg);
		printed_len = strlen(recursion_bug_msg);
	} 

	/* Emit the output into the temporary buffer */
	printed_len += vscnprintf(printk_buf + printed_len,
			sizeof(printk_buf) - printed_len, fmt, args);

	p = printk_buf;

	/* Read log level and handle special printk prefix */
	plen = log_prefix(p, (unsigned int *)&current_log_level, &special);
	if (plen) {
		p += plen;

		switch (special) {
			case 'c': /* Strip <c> KERN_CONT, continue line */
				plen = 0;
				break;
			case 'd': /* Strip <d> KERN_DEFAULT, start new line */
				plen = 0;
			default:
				if (!new_text_line) {
					emit_log_char('\n');
					new_text_line = 1;
				}
		}
	}

	/*
	 * Copy the output into log_buf. If the caller didn't provide
	 * the appropriate log prefix, we insert them here
	 */
	for (; *p; p++) {
		if (new_text_line) {
			new_text_line = 0;

			if (plen) {
				/* Copy original log prefix */
				unsigned int i;

				for (i = 0; i < plen; i++)
					emit_log_char(printk_buf[i]);
				printed_len += plen;
			} else {
				/* Add log prefix */
				emit_log_char('<');
				emit_log_char(current_log_level + '0');
				emit_log_char('>');
				printed_len += 3;
			}

			if (printk_time) {
				/* Add the current time stamp */
				char tbuf[50], *tp;
				unsigned tlen;
				//unsigned long long t;
				u64 t;
				unsigned long nanosec_rem;

				//t = cpu_clock(printk_cpu);
				t = cpu_clock();
				nanosec_rem = do_div(t, 1000000000);
				tlen = sprintf(tbuf, "[%5lu.%06lu] ",
						(unsigned long) t,
						nanosec_rem / 1000);

				for (tp = tbuf; tp < tbuf + tlen; tp++)
					emit_log_char(*tp);
				printed_len += tlen;
			}

			if (!*p)
				break;
		}

		emit_log_char(*p);
		if (*p == '\n')
			new_text_line = 1;
	}

	if (console_trylock_for_printk(this_cpu))
	{
		console_unlock();
	}
	lockdep_on();
out_restore_irqs:
	raw_spin_unlock(&logbuf_lock);	//add by duanshuai
	local_irq_restore(flags);

	return printed_len;
}

static void __call_console_drivers(unsigned start, unsigned end)
{
	char *p = &LOG_BUF(start);
	s5p4418_serial_write_buf(0, p, end - start);
}

static void _call_console_drivers(unsigned start, unsigned end)
{
	if(start != end) {
		if ((start & LOG_BUF_MASK) > (end & LOG_BUF_MASK)) {
			/* wrapped write */
			__call_console_drivers(start & LOG_BUF_MASK,
					log_buf_len);
			__call_console_drivers(0, end & LOG_BUF_MASK);
		} else {
			__call_console_drivers(start, end);
		}
	}
}

static void call_console_drivers(unsigned start, unsigned end) 
{
	unsigned cur_index, start_print;
	static int msg_level = -1;

	cur_index = start;
	start_print = start;
	while (cur_index != end) {
		if (msg_level < 0 && ((end - cur_index) > 2)) {
			/*   
			 * prepare buf_prefix, as a contiguous array,
			 * to be processed by log_prefix function
			 */
			char buf_prefix[SYSLOG_PRI_MAX_LENGTH+1];
			unsigned i;
			for (i = 0; i < ((end - cur_index)) && (i < SYSLOG_PRI_MAX_LENGTH); i++) {
				buf_prefix[i] = LOG_BUF(cur_index + i);
			}    
			buf_prefix[i] = '\0'; /* force '\0' as last string character */

			/* strip log prefix */
			cur_index += log_prefix((const char *)&buf_prefix, (unsigned int *)&msg_level, NULL);
			start_print = cur_index;
		}    
		while (cur_index != end) {
			char c = LOG_BUF(cur_index);

			cur_index++;
			if (c == '\n') {
				if (msg_level < 0) { 
					/*   
					 * printk() has already given us loglevel tags in
					 * the buffer.  This code is here in case the
					 * log buffer has wrapped right round and scribbled
					 * on those tags
					 */
					msg_level = default_message_loglevel;
				}    
				_call_console_drivers(start_print, cur_index);
				msg_level = -1;
				start_print = cur_index;
				break;
			}    
		}    
	}    
	_call_console_drivers(start_print, end);
}

void console_unlock(void)
{
	//unsigned long flags;
	unsigned _con_start, _log_end;
	//unsigned wake_klogd = 0; 
    unsigned retry = 0;

again:
	for ( ; ; ) {
		//raw_spin_lock_irqsave(&logbuf_lock, flags);
		if (con_start == log_end)
			break;          /* Nothing to print */
		_con_start = con_start;
		_log_end = log_end;
		con_start = log_end;        /* Flush */
		//raw_spin_unlock(&logbuf_lock);
		call_console_drivers(_con_start, _log_end);
		//local_irq_restore(flags);
	}    
	console_locked = 0; 

	/* Release the exclusive_console once it is used */
	//raw_spin_unlock(&logbuf_lock);
	up(&console_sem);
	/*   
	 * Someone could have filled up the buffer again, so re-check if there's
	 * something to flush. In case we cannot trylock the console_sem again,
	 * there's a new owner and the console_unlock() from them will do the
	 * flush, no worries.
	 */
	//raw_spin_lock(&logbuf_lock);
	if (con_start != log_end)
		retry = 1; 
	//raw_spin_unlock_irqrestore(&logbuf_lock, flags);

	if (retry && console_trylock())
		goto again;
}

