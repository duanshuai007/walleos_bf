#ifndef _ENV_INCLUDE_IRQFLAGS_H_
#define _ENV_INCLUDE_IRQFLAGS_H_

#define PSR_I_BIT       0x00000080

#define barrier()		__asm__ __volatile__("": : :"memory")
#define smp_mb()		barrier()

#define ALT_SMP(smp, up)                    \
	        "9998:  " smp "\n"                  \
    "   .pushsection \".alt.smp.init\", \"a\"\n"    \
    "   .long   9998b\n"                \
    "   " up "\n"                   \
    "   .popsection\n"
#define WFE(cond)		ALT_SMP("wfe" cond, "nop")
#define SEV				ALT_SMP("sev", "nop")

typedef struct {
	volatile unsigned int lock;
} arch_spinlock_t;

typedef struct raw_spinlock {
	arch_spinlock_t raw_lock;
} raw_spinlock_t;

struct current{
	int lockdep_recursion;
};

struct semaphore{
	unsigned int count;
	raw_spinlock_t lock;
};

static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags;

	asm volatile(
			"   mrs %0, cpsr    @ arch_local_irq_save\n"
			"   cpsid   i"
			: "=r" (flags) : : "memory", "cc");
	return flags;
}

static inline void arch_local_irq_enable(void)
{
	asm volatile(
			"   cpsie i         @ arch_local_irq_enable"
			:   
			:   
			: "memory", "cc");
}

static inline void arch_local_irq_disable(void)
{
	asm volatile(
			"   cpsid i         @ arch_local_irq_disable"
			:   
			:   
			: "memory", "cc");
}

/*
 * Save the current interrupt enable state.
 */
static inline unsigned long arch_local_save_flags(void)
{
	unsigned long flags;
	asm volatile(
			"   mrs %0, cpsr    @ local_save_flags"
			: "=r" (flags) : : "memory", "cc");
	return flags;
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	asm volatile(
			"   msr cpsr_c, %0  @ local_irq_restore"
			:
			: "r" (flags)
			: "memory", "cc");
}

static inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return flags & PSR_I_BIT;
}

#define trace_hardirqs_on()        do { } while (0)
#define trace_hardirqs_off()       do { } while (0)

#define raw_local_irq_disable()     arch_local_irq_disable()
#define raw_local_irq_enable()      arch_local_irq_enable()
#define raw_local_irq_save(flags)           \
	do {                        \
		flags = arch_local_irq_save();      \
	} while (0)

#define raw_local_irq_restore(flags)            \
	do {                        \
		arch_local_irq_restore(flags);      \
	} while (0)
#define raw_local_save_flags(flags)         \
	do {                        \
		flags = arch_local_save_flags();    \
	} while (0)
#define raw_irqs_disabled_flags(flags)          \
	({                      \
	 arch_irqs_disabled_flags(flags);    \
	 })
#define raw_irqs_disabled()     (arch_irqs_disabled())
#define raw_safe_halt()         arch_safe_halt()

#define local_irq_save(flags)               \
	do {                        \
		raw_local_irq_save(flags);      \
		trace_hardirqs_off();           \
	} while (0)

#define local_irq_restore(flags)            \
	do {                        \
		if (raw_irqs_disabled_flags(flags)) {   \
			raw_local_irq_restore(flags);   \
			trace_hardirqs_off();       \
		} else {                \
			trace_hardirqs_on();        \
			raw_local_irq_restore(flags);   \
		}                   \
	} while (0)


static inline void dsb_sev(void)
{
	__asm__ __volatile__ (
			"dsb\n"
			SEV
			);
}

static inline void arch_spin_lock(arch_spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
			"1: ldrex   %0, [%1]\n"
			"   teq %0, #0\n"
			WFE("ne")
			"   strexeq %0, %2, [%1]\n"
			"   teqeq   %0, #0\n"
			"   bne 1b"
			: "=&r" (tmp)
			: "r" (&lock->lock), "r" (1)
			: "cc");

	smp_mb();
}

static inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
			"   ldrex   %0, [%1]\n"
			"   teq %0, #0\n"
			"   strexeq %0, %2, [%1]"
			: "=&r" (tmp)
			: "r" (&lock->lock), "r" (1)
			: "cc");

	if (tmp == 0) {
		smp_mb();
		return 1;
	} else {
		return 0;
	}
}

static inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	smp_mb();

	__asm__ __volatile__(
			"   str %1, [%0]\n"
			:
			: "r" (&lock->lock), "r" (0)
			: "cc");

	dsb_sev();
}

#define arch_spin_lock_flags(lock, flags) arch_spin_lock(lock)

static inline void do_raw_spin_lock_flags(raw_spinlock_t *lock, unsigned long *flags)
{
	//__acquire(lock);
    (void)flags;
	arch_spin_lock_flags(&lock->raw_lock, *flags);
}

static inline void do_raw_spin_lock(raw_spinlock_t *lock)
{
	//__acquire(lock);
	arch_spin_lock(&lock->raw_lock);
}

static inline unsigned long __raw_spin_lock_irqsave(raw_spinlock_t *lock)
{
	unsigned long flags;

	local_irq_save(flags);
	//preempt_disable();
	//spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
	/*
	 * On lockdep we dont want the hand-coded irq-enable of
	 * do_raw_spin_lock_flags() code, because lockdep assumes
	 * that interrupts are not re-enabled during lock-acquire:
	 */
	//#ifdef CONFIG_LOCKDEP
	//  LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
	//#else
	do_raw_spin_lock_flags(lock, &flags);
	//#endif
	return flags;
}

#define raw_spin_lock_irqsave(lock, flags)          \
	do {                        \
		flags = __raw_spin_lock_irqsave(lock);   \
	} while (0)

static inline void do_raw_spin_unlock(raw_spinlock_t *lock)
{
	arch_spin_unlock(&lock->raw_lock);
	//__release(lock);
}

static inline void __raw_spin_unlock_irqrestore(raw_spinlock_t *lock,
		unsigned long flags)
{
	//spin_release(&lock->dep_map, 1, _RET_IP_);
	do_raw_spin_unlock(lock);
	local_irq_restore(flags);
	//preempt_enable();
}

#define raw_spin_unlock_irqrestore(lock, flags)     \
	do {                            \
		__raw_spin_unlock_irqrestore(lock, flags);   \
	} while (0)

#define raw_spin_unlock(lock)       do_raw_spin_unlock(lock)
#define raw_spin_lock(lock)         do_raw_spin_lock(lock)

#endif
