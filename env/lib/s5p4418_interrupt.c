#include <alloc.h>
#include <s5p4418_interrupt.h>
#include <s5p4418_gpio.h>
#include <io.h>
#include <list.h>
#include <string.h>
#include <stdlib.h>
#include <sizes.h>
#include <io.h>
#include <i2c/i2c_util.h>
#include <input/touchscreen.h>
#include <uspios.h>
#include <s5p4418_serial_stdio.h>
#include <types.h>
#include <system.h>
#include <smp.h>
#include <s5p4418_tick_delay.h>
#include <synchronize.h>
#include <stdio.h>
#include <timer.h>
#include <smp_macros.h>
#include <headsmp.h>
#include <global_timer.h>
#include <config.h>

static const unsigned int S5P4418_VIC0_BASE = 0xC0002000;
static const unsigned int S5P4418_VIC1_BASE = 0xC0003000;

struct irq_handler_t s5p4418_irq_handler[IRQ_LINES];
struct irq_handler_t s5p4418_irq_handler_gpioa[GPIO_PINS_NUMBER];
struct irq_handler_t s5p4418_irq_handler_gpiob[GPIO_PINS_NUMBER];
struct irq_handler_t s5p4418_irq_handler_gpioc[GPIO_PINS_NUMBER];
struct irq_handler_t s5p4418_irq_handler_gpiod[GPIO_PINS_NUMBER];
struct irq_handler_t s5p4418_irq_handler_gpioe[GPIO_PINS_NUMBER];

struct irq_list_t
{
    struct irq_t * irq;
    struct list_head entry;
};

struct irq_list_t __irq_list = {
    .entry = {
        .next   = &(__irq_list.entry),
        .prev   = &(__irq_list.entry),
    },
};

static inline u32 __GET_IRQ_BITS(u8 no)
{
    return no << 1;
}
static void null_interrupt_function(void * data)
{
    UNUSED(data);
}

static struct irq_t * irq_search(const char * name)
{
    struct irq_list_t * pos, * n;

    if(!name)
        return NULL;

    list_for_each_entry_safe(pos, n, &(__irq_list.entry), entry)
    {
        if(strcmp(pos->irq->name, name) == 0)
            return pos->irq;
    }

    return NULL;
}

boolean irq_register(struct irq_t * irq)
{
    struct irq_list_t * il;

    if(!irq || !irq->name)
        return FALSE;

    if(irq_search(irq->name))
    {
        return FALSE;
    }

    il = malloc(sizeof(struct irq_list_t));
    if(!il)
        return FALSE;

    irq->handler->func = null_interrupt_function;
    irq->handler->data = NULL;
    if(irq->set_type)
        irq->set_type(irq, IRQ_TYPE_NONE);
    if(irq->disable)
        irq->disable(irq);
    il->irq = irq;
    list_add_tail(&il->entry, &(__irq_list.entry));

    return TRUE;
}

boolean irq_unregister(struct irq_t * irq)
{
    struct irq_list_t * pos, * n;

    if(!irq || !irq->name)
        return FALSE;

    list_for_each_entry_safe(pos, n, &(__irq_list.entry), entry)
    {
        if(pos->irq == irq)
        {
            irq->handler->func = null_interrupt_function;
            irq->handler->data = NULL;
            if(pos->irq->set_type)
                pos->irq->set_type(irq, IRQ_TYPE_NONE);
            if(pos->irq->disable)
                pos->irq->disable(pos->irq);

            list_del(&(pos->entry));
            free(pos);
            return TRUE;
        }
    }

    return FALSE;
}

boolean request_irq(const char * name, enum irq_to_cpu to_cpu, TIRQHandler * func, enum irq_type_t type, void * data)
{
    struct irq_t * irq;

    if(!name || !func)
        return FALSE;

    irq = irq_search(name);
    if(!irq)
        return FALSE;

    if(irq->handler->func != null_interrupt_function)
        return FALSE;

    irq->handler->func = func;
    irq->handler->data = data;
	//by duanshuai add
	irq->cpuid	= to_cpu;
	//
    if(irq->set_type)
        irq->set_type(irq, type);
    if(irq->enable)
        irq->enable(irq);

    return TRUE;
}

boolean free_irq(const char * name)
{
    struct irq_t * irq;

    if(!name)
        return FALSE;

    irq = irq_search(name);
    if(!irq)
        return FALSE;

    if(irq->handler->func == null_interrupt_function)
        return FALSE;

    irq->handler->func = null_interrupt_function;
    irq->handler->data = NULL;
    if(irq->set_type)
        irq->set_type(irq, IRQ_TYPE_NONE);
    if(irq->disable)
        irq->disable(irq);

    return TRUE;
}

void enable_irq(const char * name)
{
    if(!name)
        return;

    struct irq_t * irq = irq_search(name);

    if(irq && irq->enable)
        irq->enable(irq);
}

void disable_irq(const char * name)
{
    if(!name)
        return;

    struct irq_t * irq = irq_search(name);

    if(irq && irq->disable)
        irq->disable(irq);
}

inline __attribute__((always_inline)) unsigned long __ffs(unsigned long word)
{
    return __builtin_ctzl(word);
}

static void s5p4418_irq_handler_func_gpioa(void * data)
{
    UNUSED(data);
    u32 det;
    u32 offset;

    det = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DET));
    if(det != 0)
    {
        offset = __ffs(det);
        (s5p4418_irq_handler_gpioa[offset].func)(s5p4418_irq_handler_gpioa[offset].data);
        write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DET), (CLEAR_INTERRUPT << offset));
    }
}

static void s5p4418_irq_handler_func_gpiob(void * data)
{
    UNUSED(data);
    u32 det;
    u32 offset;

    det = read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DET));
    if(det != 0)
    {
        offset = __ffs(det);
        (s5p4418_irq_handler_gpiob[offset].func)(s5p4418_irq_handler_gpiob[offset].data);
        write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DET), (CLEAR_INTERRUPT << offset));
    }
}

static void s5p4418_irq_handler_func_gpioc(void * data)
{
    UNUSED(data);
    u32 det;
    u32 offset;

    det = read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DET));
    if(det != 0)
    {
        offset = __ffs(det);
        (s5p4418_irq_handler_gpioc[offset].func)(s5p4418_irq_handler_gpioc[offset].data);
        write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DET), (CLEAR_INTERRUPT << offset));
    }
}

static void s5p4418_irq_handler_func_gpiod(void * data)
{
    UNUSED(data);
    u32 det;
    u32 offset;

    det = read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DET));
    if(det != 0)
    {
        offset = __ffs(det);
        (s5p4418_irq_handler_gpiod[offset].func)(s5p4418_irq_handler_gpiod[offset].data);
        write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DET), (CLEAR_INTERRUPT << offset));
    }
}

static void s5p4418_irq_handler_func_gpioe(void * data)
{
    UNUSED(data);
    u32 det;
    u32 offset;

    det = read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DET));
    if(det != 0)
    {
        offset = __ffs(det);
        (s5p4418_irq_handler_gpioe[offset].func)(s5p4418_irq_handler_gpioe[offset].data);
        write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DET), (CLEAR_INTERRUPT << offset));
    }
}

static inline void  s5p4418_vic_irq_set(int no, const unsigned int addr, int enable)
{
    unsigned int val;
    no %= IRQ_NOMBER_VIC0;
    val = read32(phys_to_virt(addr + VIC_INTENABLE));
    if(enable)
        val |= (BIT_ENABLE << no);
    else
        val &= ~(BIT_ENABLE << no);

    write32(phys_to_virt(addr + VIC_INTENABLE), val);
}

static inline void s5p4418_gpio_irq_set(int no, const unsigned int addr, int enable)
{
    unsigned int val;
    no %= IRQ_NOMBER_VIC0;
    val = read32(phys_to_virt(addr + GPIO_INTENB));
    if(enable)
        val |= (BIT_ENABLE << no);
    else
        val &= ~(BIT_ENABLE << no);
    write32(phys_to_virt(addr + GPIO_INTENB), val);
}

static void s5p4418_irq_enable(struct irq_t * irq)
{
    if(!irq)
        return;

    int no = irq->no;

    /* VIC0 */
    if(no < IRQ_NOMBER_VIC0)
    {
        s5p4418_vic_irq_set(no, S5P4418_VIC0_BASE, ENABLE);
    }
    /* VIC1 */
    else if(no < IRQ_NOMBER_VIC1)
    {
        s5p4418_vic_irq_set(no, S5P4418_VIC1_BASE, ENABLE);
    }
    /* GPIOA */
    else if(no < IRQ_NOMBER_GPIOA)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOA_BASE, ENABLE);
        //no = no - IRQ_NOMBER_VIC1;
        //write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB)) | (BIT_ENABLE << no)));
    }
    /* GPIOB */
    else if(no < IRQ_NOMBER_GPIOB)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOB_BASE, ENABLE);
        //no = no - IRQ_NOMBER_GPIOA;
        //write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB)) | (BIT_ENABLE << no)));
    }
    /* GPIOC */
    else if(no < IRQ_NOMBER_GPIOC)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOC_BASE, ENABLE);
        //no = no - IRQ_NOMBER_GPIOB;
        //write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB)) | (BIT_ENABLE << no)));
    }
    /* GPIOD */
    else if(no < IRQ_NOMBER_GPIOD)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOD_BASE, ENABLE);
        //no = no - IRQ_NOMBER_GPIOC;
        //write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB)) | (BIT_ENABLE << no)));
    }
    /* GPIOE */
    else if(no < IRQ_NOMBER_GPIOE)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOE_BASE, ENABLE);
        //no = no - IRQ_NOMBER_GPIOD;
        //write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB)) | (BIT_ENABLE << no)));
    }
}

static void s5p4418_irq_disable(struct irq_t * irq)
{
    if(!irq)
        return;

    int no = irq->no;

    /* VIC0 */
    if(no < IRQ_NOMBER_VIC0)
    {
        s5p4418_vic_irq_set(no, S5P4418_VIC0_BASE, DISABLE);
        //write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR)) | (BIT_ENABLE << no)));
    }
    /* VIC1 */
    else if(no < IRQ_NOMBER_VIC1)
    {
        s5p4418_vic_irq_set(no, S5P4418_VIC1_BASE, DISABLE);
        //no = no - IRQ_NOMBER_VIC0;
        //write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR)) | (BIT_ENABLE << no)));
    }
    /* GPIOA */
    else if(no < IRQ_NOMBER_GPIOA)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOA_BASE, DISABLE);
        //no = no - IRQ_NOMBER_VIC1;
        //write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB)) & ~(BIT_ENABLE << no)));
    }
    /* GPIOB */
    else if(no < IRQ_NOMBER_GPIOB)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOB_BASE, DISABLE);
        //no = no - IRQ_NOMBER_GPIOA;
        //write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB)) & ~(BIT_ENABLE << no)));
    }
    /* GPIOC */
    else if(no < IRQ_NOMBER_GPIOC)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOC_BASE, DISABLE);
        //no = no - IRQ_NOMBER_GPIOB;
        //write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB)) & ~(BIT_ENABLE << no)));
    }
    /* GPIOD */
    else if(no < IRQ_NOMBER_GPIOD)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOD_BASE, DISABLE);
        //no = no - IRQ_NOMBER_GPIOC;
        //write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB)) & ~(BIT_ENABLE << no)));
    }
    /* GPIOE */
    else if(no < IRQ_NOMBER_GPIOE)
    {
        s5p4418_gpio_irq_set(no, S5P4418_GPIOA_BASE, DISABLE);
        //no = no - IRQ_NOMBER_GPIOD;
        //write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB)) & ~(BIT_ENABLE << no)));
    }
}

static inline void s5p4418_gpio_irq_set_type(int no, const unsigned int addr, u32 cfg)
{
    unsigned int val;

    no %= IRQ_NOMBER_VIC0;

    if(cfg != GPIO_DET_MODE_RESERVED)
    {
        if(no < GPIO_LOW_SIXTEEN)
        {
            val = read32(phys_to_virt(addr + GPIO_DETMODE0));
            val &= ~(GPIO_DET_MODE_MASK << __GET_IRQ_BITS(no));
            val |= (cfg & GPIO_DET_MODE_MASK) << __GET_IRQ_BITS(no);
            write32(addr + GPIO_DETMODE0, val);
        }
        else if(no < GPIO_HIGH_SIXTEEN)
        {
            val = read32(phys_to_virt(addr + GPIO_DETMODE1));
            val &= ~(GPIO_DET_MODE_MASK << __GET_IRQ_BITS(no - GPIO_LOW_SIXTEEN));
            val |= (cfg & GPIO_DET_MODE_MASK) << __GET_IRQ_BITS(no - GPIO_LOW_SIXTEEN);
            write32(addr + GPIO_DETMODE1, val);
        }

        val = read32(phys_to_virt(addr + GPIO_DETMODEEX));
        if(cfg & GPIO_DET_MODE_NO_MASK)
            val |= BIT_ENABLE << no;
        else
            val &= ~(BIT_ENABLE << no);
        write32(addr + GPIO_DETMODEEX, val);

        val = read32(addr + GPIO_DETENB);
        val |= BIT_ENABLE << no;
        write32(addr + GPIO_DETENB, val);
    }
    else
    {
        val = read32(addr + GPIO_DETENB);
        val &= ~(BIT_ENABLE << no);
        write32(addr + GPIO_DETENB, val);
    }
}

static void s5p4418_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
    if(!irq)
        return;

    int no = irq->no;
    //u32 val;
    u32 cfg;

    switch(type)
    {
    case IRQ_TYPE_NONE:
        cfg = GPIO_DET_MODE_RESERVED;
        break;
    case IRQ_TYPE_LEVEL_LOW:
        cfg = GPIO_DET_MODE_LOW_LEVEL;
        break;
    case IRQ_TYPE_LEVEL_HIGH:
        cfg = GPIO_DET_MODE_HIGH_LEVEL;
        break;
    case IRQ_TYPE_EDGE_FALLING:
        cfg = GPIO_DET_MODE_FALLING_EDGE;
        break;
    case IRQ_TYPE_EDGE_RISING:
        cfg = GPIO_DET_MODE_RISING_EDGE;
        break;
    case IRQ_TYPE_EDGE_BOTH:
        cfg = GPIO_DET_MODE_BOTH_EDGE;
        break;
    default:
        return;
    }

    /* VIC0 */
    if(no < IRQ_NOMBER_VIC0)
    {
    }
    /* VIC1 */
    else if(no < IRQ_NOMBER_VIC1)
    {
        no = no - IRQ_NOMBER_VIC0;
    }
    /* GPIOA */
    else if(no < IRQ_NOMBER_GPIOA)
    {
        s5p4418_gpio_irq_set_type(no, S5P4418_GPIOA_BASE, cfg);
        //no = no - IRQ_NOMBER_VIC1;
        //if(cfg != GPIO_DET_MODE_RESERVED)
        //{
        //    if(no < GPIO_LOW_SIXTEEN)
        //    {
        //        val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODE0));
        //        val &= ~(GPIO_DET_MODE_MASK << __GET_IRQ_BITS(no));
        //        val |= (cfg & GPIO_DET_MODE_MASK) << __GET_IRQ_BITS(no);
        //        write32(S5P4418_GPIOA_BASE + GPIO_DETMODE0, val);
        //    }
        //    else if(no < GPIO_HIGH_SIXTEEN)
        //    {
        //        val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODE1));
        //        val &= ~(GPIO_DET_MODE_MASK << __GET_IRQ_BITS(no - GPIO_LOW_SIXTEEN));
        //        val |= (cfg & GPIO_DET_MODE_MASK) << __GET_IRQ_BITS(no - GPIO_LOW_SIXTEEN);
        //        write32(S5P4418_GPIOA_BASE + GPIO_DETMODE1, val);
        //    }

        //    val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODEEX));
        //    if(cfg & GPIO_DET_MODE_NO_MASK)
        //        val |= BIT_ENABLE << no;
        //    else
        //        val &= ~(BIT_ENABLE << no);
        //    write32(S5P4418_GPIOA_BASE + GPIO_DETMODEEX, val);

        //    val = read32(S5P4418_GPIOA_BASE + GPIO_DETENB);
        //    val |= BIT_ENABLE << no;
        //    write32(S5P4418_GPIOA_BASE + GPIO_DETENB, val);
        //}
        //else
        //{
        //    val = read32(S5P4418_GPIOA_BASE + GPIO_DETENB);
        //    val &= ~(BIT_ENABLE << no);
        //    write32(S5P4418_GPIOA_BASE + GPIO_DETENB, val);
        //}
    }
    /* GPIOB */
    else if(no < IRQ_NOMBER_GPIOB)
    {
        s5p4418_gpio_irq_set_type(no, S5P4418_GPIOB_BASE, cfg); 
    }
    /* GPIOC */
    else if(no < IRQ_NOMBER_GPIOC)
    {
        s5p4418_gpio_irq_set_type(no, S5P4418_GPIOC_BASE, cfg);
    }
    /* GPIOD */
    else if(no < IRQ_NOMBER_GPIOD)
    {
        s5p4418_gpio_irq_set_type(no, S5P4418_GPIOD_BASE, cfg);
    }
    /* GPIOE */
    else if(no < IRQ_NOMBER_GPIOE)
    {
        s5p4418_gpio_irq_set_type(no, S5P4418_GPIOE_BASE, cfg);;
    }
}

static struct irq_t s5p4418_irqs[] = {
    /* VIC0 */
    {
        .name       = "MCUSTOP",
        .no         = 0,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DMA0",
        .no         = 1,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DMA1",
        .no         = 2,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "CLKPWR0",
        .no         = 3,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "CLKPWR1",
        .no         = 4,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "CLKPWR2",
        .no         = 5,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "UART1",
        .no         = 6,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "UART0",
        .no         = 7,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "UART2",
        .no         = 8,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "UART3",
        .no         = 9,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "UART4",
        .no         = 10,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
		.name		= "NULL",
		.no			= 11,
	}, {
        .name       = "SSP0",
        .no         = 12,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SSP1",
        .no         = 13,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SSP2",
        .no         = 14,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "I2C0",
        .no         = 15,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "I2C1",
        .no         = 16,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "I2C2",
        .no         = 17,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DEINTERLACE",
        .no         = 18,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SCALER",
        .no         = 19,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "AC97",
        .no         = 20,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SPDIFRX",
        .no         = 21,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SPDIFTX",
        .no         = 22,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "TIMER0",
        .no         = 23,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "TIMER1",
        .no         = 24,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "TIMER2",
        .no         = 25,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "TIMER3",
        .no         = 26,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PWM0",
        .no         = 27,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PWM1",
        .no         = 28,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PWM2",
        .no         = 29,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PWM3",
        .no         = 30,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "WDT",
        .no         = 31,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },

    /* VIC1 */
    {
        .name       = "MPEGTSI",
        .no         = 32,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[32],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DISPLAYTOP0",
        .no         = 33,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[33],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DISPLAYTOP1",
        .no         = 34,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[34],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DISPLAYTOP2",
        .no         = 35,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[35],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "DISPLAYTOP3",
        .no         = 36,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[36],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "VIP0",
        .no         = 37,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[37],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "VIP1",
        .no         = 38,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[38],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "MIPI",
        .no         = 39,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[39],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "3DGPU",
        .no         = 40,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[40],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "ADC",
        .no         = 41,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[41],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PPM",
        .no         = 42,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[42],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SDMMC0",
        .no         = 43,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[43],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SDMMC1",
        .no         = 44,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[44],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "SDMMC2",
        .no         = 45,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[45],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "CODA9600",
        .no         = 46,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[46],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "CODA9601",
        .no         = 47,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[47],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GMAC",
        .no         = 48,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[48],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "USB20OTG",
        .no         = 49,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[49],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "USB20HOST",
        .no         = 50,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[50],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
		.name		= "NULL",
		.no			= 51,
	},  {
		.name		= "NULL",
		.no			= 52,
	}, {
		.name		= "NULL",
		.no			= 53,
	}, {
		.name		= "NULL",
		.no			= 54,
	}, {
		.name		= "NULL",
		.no			= 55,
	}, {
		.name		= "NULL",
		.no			= 56,
	}, {
		.name		= "NULL",
		.no			= 57,
	},{
        .name       = "CRYPTO",
        .no         = 58,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[58],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "PDM",
        .no         = 59,
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler[59],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
		.name		= "NULL",
		.no			= 60,
	}, {
		.name		= "NULL",
		.no			= 61,
	}, {
		.name		= "NULL",
		.no			= 62,
	}, {
		.name		= "NULL",
		.no			= 63,
	},

    /* GPIOA */
    {
        .name       = "GPIOA0",
        .no         = GPIOA_IRQ_NO(0),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA1",
        .no         = GPIOA_IRQ_NO(1),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA2",
        .no         = GPIOA_IRQ_NO(2),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA3",
        .no         = GPIOA_IRQ_NO(3),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA4",
        .no         = GPIOA_IRQ_NO(4),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA5",
        .no         = GPIOA_IRQ_NO(5),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA6",
        .no         = GPIOA_IRQ_NO(6),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA7",
        .no         = GPIOA_IRQ_NO(7),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA8",
        .no         = GPIOA_IRQ_NO(8),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA9",
        .no         = GPIOA_IRQ_NO(9),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA10",
        .no         = GPIOA_IRQ_NO(10),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA11",
        .no         = GPIOA_IRQ_NO(11),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[11],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA12",
        .no         = GPIOA_IRQ_NO(12),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA13",
        .no         = GPIOA_IRQ_NO(13),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA14",
        .no         = GPIOA_IRQ_NO(14),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA15",
        .no         = GPIOA_IRQ_NO(15),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA16",
        .no         = GPIOA_IRQ_NO(16),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA17",
        .no         = GPIOA_IRQ_NO(17),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA18",
        .no         = GPIOA_IRQ_NO(18),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA19",
        .no         = GPIOA_IRQ_NO(19),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA20",
        .no         = GPIOA_IRQ_NO(20),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA21",
        .no         = GPIOA_IRQ_NO(21),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA22",
        .no         = GPIOA_IRQ_NO(22),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA23",
        .no         = GPIOA_IRQ_NO(23),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA24",
        .no         = GPIOA_IRQ_NO(24),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA25",
        .no         = GPIOA_IRQ_NO(25),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA26",
        .no         = GPIOA_IRQ_NO(26),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA27",
        .no         = GPIOA_IRQ_NO(27),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA28",
        .no         = GPIOA_IRQ_NO(28),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA29",
        .no         = GPIOA_IRQ_NO(29),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA30",
        .no         = GPIOA_IRQ_NO(30),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOA31",
        .no         = GPIOA_IRQ_NO(31),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioa[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },

    /* GPIOB */
    {
        .name       = "GPIOB0",
        .no         = GPIOB_IRQ_NO(0),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB1",
        .no         = GPIOB_IRQ_NO(1),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB2",
        .no         = GPIOB_IRQ_NO(2),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB3",
        .no         = GPIOB_IRQ_NO(3),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB4",
        .no         = GPIOB_IRQ_NO(4),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB5",
        .no         = GPIOB_IRQ_NO(5),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB6",
        .no         = GPIOB_IRQ_NO(6),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB7",
        .no         = GPIOB_IRQ_NO(7),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB8",
        .no         = GPIOB_IRQ_NO(8),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB9",
        .no         = GPIOB_IRQ_NO(9),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB10",
        .no         = GPIOB_IRQ_NO( 10),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB11",
        .no         = GPIOB_IRQ_NO( 11),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[11],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB12",
        .no         = GPIOB_IRQ_NO( 12),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB13",
        .no         = GPIOB_IRQ_NO( 13),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB14",
        .no         = GPIOB_IRQ_NO( 14),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB15",
        .no         = GPIOB_IRQ_NO( 15),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB16",
        .no         = GPIOB_IRQ_NO( 16),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB17",
        .no         = GPIOB_IRQ_NO( 17),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB18",
        .no         = GPIOB_IRQ_NO( 18),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB19",
        .no         = GPIOB_IRQ_NO( 19),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB20",
        .no         = GPIOB_IRQ_NO( 20),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB21",
        .no         = GPIOB_IRQ_NO( 21),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB22",
        .no         = GPIOB_IRQ_NO( 22),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB23",
        .no         = GPIOB_IRQ_NO( 23),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB24",
        .no         = GPIOB_IRQ_NO( 24),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB25",
        .no         = GPIOB_IRQ_NO( 25),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB26",
        .no         = GPIOB_IRQ_NO( 26),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB27",
        .no         = GPIOB_IRQ_NO( 27),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB28",
        .no         = GPIOB_IRQ_NO( 28),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB29",
        .no         = GPIOB_IRQ_NO( 29),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB30",
        .no         = GPIOB_IRQ_NO( 30),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOB31",
        .no         = GPIOB_IRQ_NO( 31),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiob[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },

    /* GPIOC */
    {
        .name       = "GPIOC0",
        .no         = GPIOC_IRQ_NO(0),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC1",
        .no         = GPIOC_IRQ_NO(1),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC2",
        .no         = GPIOC_IRQ_NO(2),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC3",
        .no         = GPIOC_IRQ_NO(3),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC4",
        .no         = GPIOC_IRQ_NO(4),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC5",
        .no         = GPIOC_IRQ_NO(5),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC6",
        .no         = GPIOC_IRQ_NO(6),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC7",
        .no         = GPIOC_IRQ_NO(7),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC8",
        .no         = GPIOC_IRQ_NO(8),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC9",
        .no         = GPIOC_IRQ_NO(9),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC10",
        .no         = GPIOC_IRQ_NO(10),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC11",
        .no         = GPIOC_IRQ_NO(11),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[11],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC12",
        .no         = GPIOC_IRQ_NO(12),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC13",
        .no         = GPIOC_IRQ_NO(13),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC14",
        .no         = GPIOC_IRQ_NO(14),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC15",
        .no         = GPIOC_IRQ_NO(15),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC16",
        .no         = GPIOC_IRQ_NO(16),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC17",
        .no         = GPIOC_IRQ_NO(17),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC18",
        .no         = GPIOC_IRQ_NO(18),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC19",
        .no         = GPIOC_IRQ_NO(19),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC20",
        .no         = GPIOC_IRQ_NO(20),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC21",
        .no         = GPIOC_IRQ_NO(21),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC22",
        .no         = GPIOC_IRQ_NO(22),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC23",
        .no         = GPIOC_IRQ_NO(23),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC24",
        .no         = GPIOC_IRQ_NO(24),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC25",
        .no         = GPIOC_IRQ_NO(25),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC26",
        .no         = GPIOC_IRQ_NO(26),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC27",
        .no         = GPIOC_IRQ_NO(27),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC28",
        .no         = GPIOC_IRQ_NO(28),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC29",
        .no         = GPIOC_IRQ_NO(29),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC30",
        .no         = GPIOC_IRQ_NO(30),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOC31",
        .no         = GPIOC_IRQ_NO(31),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioc[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },

    /* GPIOD */
    {
        .name       = "GPIOD0",
        .no         = GPIOD_IRQ_NO(0),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD1",
        .no         = GPIOD_IRQ_NO(1),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD2",
        .no         = GPIOD_IRQ_NO(2),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD3",
        .no         = GPIOD_IRQ_NO(3),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD4",
        .no         = GPIOD_IRQ_NO(4),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD5",
        .no         = GPIOD_IRQ_NO(5),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD6",
        .no         = GPIOD_IRQ_NO(6),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD7",
        .no         = GPIOD_IRQ_NO(7),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD8",
        .no         = GPIOD_IRQ_NO(8),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD9",
        .no         = GPIOD_IRQ_NO(9),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD10",
        .no         = GPIOD_IRQ_NO(10),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD11",
        .no         = GPIOD_IRQ_NO(11),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[11],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD12",
        .no         = GPIOD_IRQ_NO(12),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD13",
        .no         = GPIOD_IRQ_NO(13),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD14",
        .no         = GPIOD_IRQ_NO(14),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD15",
        .no         = GPIOD_IRQ_NO(15),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD16",
        .no         = GPIOD_IRQ_NO(16),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD17",
        .no         = GPIOD_IRQ_NO(17),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD18",
        .no         = GPIOD_IRQ_NO(18),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD19",
        .no         = GPIOD_IRQ_NO(19),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD20",
        .no         = GPIOD_IRQ_NO(20),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD21",
        .no         = GPIOD_IRQ_NO(21),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD22",
        .no         = GPIOD_IRQ_NO(22),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD23",
        .no         = GPIOD_IRQ_NO(23),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD24",
        .no         = GPIOD_IRQ_NO(24),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD25",
        .no         = GPIOD_IRQ_NO(25),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD26",
        .no         = GPIOD_IRQ_NO(26),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD27",
        .no         = GPIOD_IRQ_NO(27),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD28",
        .no         = GPIOD_IRQ_NO(28),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD29",
        .no         = GPIOD_IRQ_NO(29),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD30",
        .no         = GPIOD_IRQ_NO(30),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOD31",
        .no         = GPIOD_IRQ_NO(31),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpiod[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },

    /* GPIOE */
    {
        .name       = "GPIOE0",
        .no         = GPIOE_IRQ_NO(0),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[0],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE1",
        .no         = GPIOE_IRQ_NO(1),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[1],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE2",
        .no         = GPIOE_IRQ_NO(2),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[2],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE3",
        .no         = GPIOE_IRQ_NO(3),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[3],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE4",
        .no         = GPIOE_IRQ_NO(4),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[4],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE5",
        .no         = GPIOE_IRQ_NO(5),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[5],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE6",
        .no         = GPIOE_IRQ_NO(6),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[6],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE7",
        .no         = GPIOE_IRQ_NO(7),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[7],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE8",
        .no         = GPIOE_IRQ_NO(8),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[8],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE9",
        .no         = GPIOE_IRQ_NO(9),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[9],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE10",
        .no         = GPIOE_IRQ_NO(10),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[10],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE11",
        .no         = GPIOE_IRQ_NO(11),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[11],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE12",
        .no         = GPIOE_IRQ_NO(12),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[12],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE13",
        .no         = GPIOE_IRQ_NO(13),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[13],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE14",
        .no         = GPIOE_IRQ_NO(14),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[14],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE15",
        .no         = GPIOE_IRQ_NO(15),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[15],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE16",
        .no         = GPIOE_IRQ_NO(16),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[16],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE17",
        .no         = GPIOE_IRQ_NO(17),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[17],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE18",
        .no         = GPIOE_IRQ_NO(18),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[18],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE19",
        .no         = GPIOE_IRQ_NO(19),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[19],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE20",
        .no         = GPIOE_IRQ_NO(20),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[20],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE21",
        .no         = GPIOE_IRQ_NO(21),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[21],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE22",
        .no         = GPIOE_IRQ_NO(22),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[22],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE23",
        .no         = GPIOE_IRQ_NO(23),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[23],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE24",
        .no         = GPIOE_IRQ_NO(24),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[24],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE25",
        .no         = GPIOE_IRQ_NO(25),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[25],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE26",
        .no         = GPIOE_IRQ_NO(26),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[26],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE27",
        .no         = GPIOE_IRQ_NO(27),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[27],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE28",
        .no         = GPIOE_IRQ_NO(28),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[28],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE29",
        .no         = GPIOE_IRQ_NO(29),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[29],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE30",
        .no         = GPIOE_IRQ_NO(30),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[30],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    }, {
        .name       = "GPIOE31",
        .no         = GPIOE_IRQ_NO(31),
		.cpuid		= DEFAULT_CPU,
        .handler    = &s5p4418_irq_handler_gpioe[31],
        .enable     = s5p4418_irq_enable,
        .disable    = s5p4418_irq_disable,
        .set_type   = s5p4418_irq_set_type,
    },
};

static u32 read_p15_c1(void)
{
    u32 value;

    __asm__ __volatile__(
        "mrc p15, 0, %0, c1, c0, 0"
        : "=r" (value)
        :
        : "memory");

    return value;
}

static void write_p15_c1(u32 value)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c1, c0, 0"
        :
        : "r" (value)
        : "memory");

    read_p15_c1();
}

void vic_enable(void)
{
    u32 reg;

    reg = read_p15_c1();
    write_p15_c1(reg | CR_VE);
}

void irq_enable(void)
{
    u32 tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "bic %0, %0, #(1<<7)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

void fiq_enable(void)
{
    u32 tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "bic %0, %0, #(1<<6)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

static void s5p4418_irq_init(void)
{
    unsigned int i;
    const u32 ALL_GPIO_INTERRUPT_ENABLE = (0x1f << (53 - 32));

    /* VIC to core, pass through GIC */
#ifdef SMP
    //do nothing
#else
    write32(phys_to_virt(0xf0000100), 0);
#endif
    /* Select irq mode */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTSELECT), ALL_ZERO);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTSELECT), ALL_ZERO);

    /* Disable all interrupts */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE), ALL_ZERO);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), ALL_ZERO);

    /* Clear all interrupts */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR), INTER_ALL_CLEAR);
    DEBUG( INT_ON, "VIC_INTENCLEAR0 : 0x%x\n", read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR)));
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR), INTER_ALL_CLEAR);
    DEBUG( INT_ON, "VIC_INTENCLEAR1 : 0x%x\n", read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR)));//VIC_INTENCLEAR不能设>置

    /* Clear all irq status */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_IRQSTATUS), ALL_ZERO);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_IRQSTATUS), ALL_ZERO);

    /* Clear all fiq status */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_FIQSTATUS), ALL_ZERO);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_FIQSTATUS), ALL_ZERO);

    /* Clear all software interrupts */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), INTER_ALL_CLEAR);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), INTER_ALL_CLEAR);
    DEBUG( INT_ON, "SOFTINTCLEAR0 : 0x%x\n", read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR)));
    DEBUG( INT_ON, "SOFTINTCLEAR1 : 0x%x\n", read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR)));//VIC_SOFTINTCLEAR不能>设置

    /* Set vic address to zero */
    write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_ADDRESS), ALL_ZERO);
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_ADDRESS), ALL_ZERO);
#if 1
    for(i = 0; i < 32; i++)
    {
        write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
        write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
    }
#endif
    /*
     * Install interrupt chains
     */
    s5p4418_irq_handler[GPIOA_IRQ_NO].func = s5p4418_irq_handler_func_gpioa;
    s5p4418_irq_handler[GPIOB_IRQ_NO].func = s5p4418_irq_handler_func_gpiob;
    s5p4418_irq_handler[GPIOC_IRQ_NO].func = s5p4418_irq_handler_func_gpioc;
    s5p4418_irq_handler[GPIOD_IRQ_NO].func = s5p4418_irq_handler_func_gpiod;
    s5p4418_irq_handler[GPIOE_IRQ_NO].func = s5p4418_irq_handler_func_gpioe;

    /*
     * Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE vic interrupt.
     */
    write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE)) | ALL_GPIO_INTERRUPT_ENABLE));

    for(i = 0; i < ARRAY_SIZE(s5p4418_irqs); i++)
    {
        irq_register(&s5p4418_irqs[i]);
    }

    /* Enable vector interrupt controller */
    vic_enable();

    /* Enable irq and fiq */
    irq_enable();
    fiq_enable();
}

static int s_irq_count = 0;
static void gpiob9_interrupt_func(void * data)
{
    UNUSED(data);
#ifdef KEY_INT_ON
	u8 cpuid = get_cpuid();
#endif
	DEBUG( KEY_INT_ON,  "cpu[%d]:gpiob9 interrupt begin\r\n", cpuid);

    s_irq_count ++;
    DEBUG( KEY_INT_ON, "====s_irq_count = %d\r\n", s_irq_count);
    if(gpio_get_value(S5P4418_GPIOB(9)) == 0)
        DEBUG( KEY_INT_ON, "LED_STATUS_ON\r\n");
    else 
        DEBUG( KEY_INT_ON, "LED_STATUS_OFF\r\n");
}

//#define TOUCH_INT_ON TRUE
static void gpiob29_interrupt_func(void * data)
{
    UNUSED(data);
#ifdef TOUCH_INT_ON
    u32 cpuid = get_cpuid();
#endif
	DEBUG( TOUCH_INT_ON,  "cpu[%d]:gpiob29 interrupt begin\r\n", cpuid);
//	printk("gpiob29 interrupt begin\r\n");

    struct irq_t *irq;
    I2C_CMD  MasterCmd;
    MasterCmd.Channel        = 1;
    MasterCmd.TransferSize   = 33;
    MasterCmd.MSMode = 2;
    MasterCmd.SlaveAddress   = 0x70;
    MasterCmd.SubAddress     = 0x00;
    MasterCmd.ClockPrescaler = 6;
    MasterCmd.ClockDivider   = 256;

//    u8 *pBuf = (u8 *)malloc(sizeof(TFT5406Buffer));
    TouchScreenDevice *s_pThis = TouchScreenDeviceGet();  
    if(I2C_MasterRead(&MasterCmd, (u8 *)s_pThis->m_pFT5406Buffer) != -1)
    {

		irq = irq_search("GPIOB29");
		irq->disable(irq);
        s_pThis->m_pFT5406Buffer->DeviceMode &= 0x70;
        s_pThis->m_pFT5406Buffer->NumPoints &= 0x07;        
        TouchUpdateMotionEvent(s_pThis);
		irq->enable(irq);

        // event->touch_point = buf[0x02] & 0x07;

        // TouchScreenDevice *s_pThis = TouchScreenDeviceGet();
        // memset(s_pThis->m_pFT5406Buffer->Point,0,sizeof(s_pThis->m_pFT5406Buffer->Point)*TOUCH_SCREEN_MAX_POINTS);
        // s_pThis->m_pFT5406Buffer->DeviceMode =  buf[0x00] & 0x70;
        // s_pThis->m_pFT5406Buffer->GestureID  =  buf[0x01];
        // s_pThis->m_pFT5406Buffer->NumPoints  =  event->touch_point;

        // switch(event->touch_point)
        // {
        //     case 5:
        //         s_pThis->m_pFT5406Buffer->Point[4].xh = buf[0x1b];
        //         s_pThis->m_pFT5406Buffer->Point[4].xl = buf[0x1c];
        //         s_pThis->m_pFT5406Buffer->Point[4].yh = buf[0x1d];
        //         s_pThis->m_pFT5406Buffer->Point[4].yl = buf[0x1e];
        //     case 4:
        //         s_pThis->m_pFT5406Buffer->Point[3].xh = buf[0x15];
        //         s_pThis->m_pFT5406Buffer->Point[3].xl = buf[0x16];
        //         s_pThis->m_pFT5406Buffer->Point[3].yh = buf[0x17];
        //         s_pThis->m_pFT5406Buffer->Point[3].yl = buf[0x18];
        //     case 3:
        //         s_pThis->m_pFT5406Buffer->Point[2].xh = buf[0x0f];
        //         s_pThis->m_pFT5406Buffer->Point[2].xl = buf[0x10];
        //         s_pThis->m_pFT5406Buffer->Point[2].yh = buf[0x11];
        //         s_pThis->m_pFT5406Buffer->Point[2].yl = buf[0x12];
        //     case 2:
        //         s_pThis->m_pFT5406Buffer->Point[1].xh = buf[0x09];
        //         s_pThis->m_pFT5406Buffer->Point[1].xl = buf[0x0a];
        //         s_pThis->m_pFT5406Buffer->Point[1].yh = buf[0x0b];
        //         s_pThis->m_pFT5406Buffer->Point[1].yl = buf[0x0c];
        //     case 1:
        //         s_pThis->m_pFT5406Buffer->Point[0].xh = buf[0x03];
        //         s_pThis->m_pFT5406Buffer->Point[0].xl = buf[0x04];
        //         s_pThis->m_pFT5406Buffer->Point[0].yh = buf[0x05];
        //         s_pThis->m_pFT5406Buffer->Point[0].yl = buf[0x06];
        //         break;
        // }
//        s5p4418_disable_interrupt(29);
        //printf("USPiTouchScreenUpdate begin\r\n");
//        USPiTouchScreenUpdate();
        //printf("USPiTouchScreenUpdate end\r\n");
//        s5p4418_enable_interrupt(29);
    }
    DEBUG( TOUCH_INT_ON,  "cpu[%d]:gpiob29 interrupt end\r\n", cpuid);
}

#ifdef SMP
static int get_irq_to_cpu(u32 vic0, u32 vic1)
{
	u32 offset;
	u32 pin_offset;
	u32 gpio;
	u32 pin;
	const u32 GPIO_INTENB[5] = {	S5P4418_GPIOA_BASE + GPIO_DET, 
									S5P4418_GPIOB_BASE + GPIO_DET,
									S5P4418_GPIOC_BASE + GPIO_DET,
									S5P4418_GPIOD_BASE + GPIO_DET,
									S5P4418_GPIOE_BASE + GPIO_DET };
	if(vic0 != 0)
	{	//0 - 31
		offset = __ffs(vic0);
		return s5p4418_irqs[offset].cpuid;
	}else
	{	//32 - 224 除了GPIO以外的中断都是按照Interrupt Sources Description Table分布的
		offset = __ffs(vic1);
		offset += 32;	//加上偏移量

		if((offset < GPIOA_IRQ_NO) || (offset > GPIOE_IRQ_NO))
		{
			return s5p4418_irqs[offset].cpuid;
		}else
		{
			gpio = offset - GPIOA_IRQ_NO;	//0 - 4 对应GPIOA-GPIOE
			DEBUG( INT_ON, "gpio = %d\r\n", gpio);
			pin = readl( GPIO_INTENB[gpio] );
			DEBUG( INT_ON, "pin = %08x\r\n", pin);
			pin_offset = __ffs(pin);					//获取引脚偏移地址
			gpio = 64 + (32 * gpio);		//gpio基础中断地址
			DEBUG( INT_ON, "real gpio[%d], offset[%d]\r\n", gpio, pin_offset);
			DEBUG( INT_ON, "name = %s\r\n", s5p4418_irqs[ gpio + pin_offset ].name);
			return s5p4418_irqs[ gpio + pin_offset ].cpuid;
		}
	}

	return -1;
}
#endif

static void arm_private_timer_interrupt_function(u32 data)
{
	TimerInterruptHandler(TimerGet((u8)data));
}

static void arm_global_timer_interrupt_function(void *d)
{
	if(global_timer_get_interrupt(d))
	{
		global_timer_clear_interrupt(d);
		global_timer_interrupt_handler(d);
	}
}

void s5p4418_InterruptHandler (void)
{
    DEBUG( INT_ON, "s5p4418_InterruptHandler\r\n");
#ifdef SMP
	u32 cpuid = get_cpuid();

	u32 to_cpu;
#endif

	u32 vic0, vic1;
	u32 offset;

#ifdef SMP
	u32 irqstat = readl(GIC_CPU_BASE + GIC_CPU_INTACK);
	u32 irqnr = irqstat & ~0x1c00;

    //printf("interrupt: cpu[%d] irq[%d] irqstat:%08x\r\n", cpuid, irqnr, irqstat);
	if(irqnr > 15 && irqnr < 1021)
	{
        if(irqnr == IRQ_GIC_PPI_PVT)
        {
            writel(PRIVATE_TIMER_BASE + TWD_TIMER_INTSTAT, 1);
            arm_private_timer_interrupt_function(cpuid);
		}
		else if(irqnr == IRQ_GIC_PPI_WDT)
		{
			//do nothing
		}
		else if(irqnr == IRQ_GIC_PPI_GT)
		{
			arm_global_timer_interrupt_function(&global_timer);
		}
		else if(irqnr == IRQ_GIC_PPI_VIC)
        {
#endif
            /* Read vector interrupt controller's irq status */
            vic0 = readl(S5P4418_VIC0_BASE + VIC_IRQSTATUS);
            vic1 = readl(S5P4418_VIC1_BASE + VIC_IRQSTATUS);
#ifdef SMP
            to_cpu = get_irq_to_cpu(vic0, vic1);
#endif
            if(vic0 != 0)
            {
                /* Get interrupt offset */
                offset = __ffs(vic0);
                /* Handle interrupt server function */
#ifdef SMP
                if(cpuid == to_cpu)
                {
#endif
                    (s5p4418_irq_handler[offset].func)(s5p4418_irq_handler[offset].data);
                    /* Clear software interrupt */
                    writel((S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), BIT_SET << offset);

                    /* Set vic address to zero */
                    writel((S5P4418_VIC0_BASE + VIC_ADDRESS), ALL_ZERO);
#ifdef SMP
                }
#endif
            }    
            else if(vic1 != 0)
            {    
                /* Get interrupt offset */
                offset = __ffs(vic1);

                DEBUG( INT_ON, "cpu[%d]-vic0[%08x]-vic1[%08x]-to_cpu[%d]\r\n", cpuid, vic0, vic1, to_cpu);
                /* Handle interrupt server function */
#ifdef SMP
                if(cpuid == to_cpu) //这个中断源GPIOB都分配给了CPU3处理
                {
#endif
                    if(cpuid == 1)
                    {
                        printf("cpu 1 interrupt\r\n");
                    }
                    (s5p4418_irq_handler[offset + IRQ_NOMBER_VIC0].func)(s5p4418_irq_handler[offset + IRQ_NOMBER_VIC0].data);
                    /* Clear software interrupt */
                    //writel(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR, BIT_SET << (offset - IRQ_NOMBER_VIC0));
                    writel(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR, BIT_SET << offset);

                    /* Set all vic address to zero */
                    writel(S5P4418_VIC1_BASE + VIC_ADDRESS, ALL_ZERO);
#ifdef SMP
                }
#endif
            }    
            else 
            {    
                /* Clear all software interrupts */
                writel(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR, INTER_ALL_CLEAR);
                writel(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR, INTER_ALL_CLEAR);

                /* Set vic address to zero */
                writel(S5P4418_VIC0_BASE + VIC_ADDRESS, ALL_ZERO);
                writel(S5P4418_VIC1_BASE + VIC_ADDRESS, ALL_ZERO);
            }
        }    
#ifdef SMP
	}else if(irqnr < 16)
	{    

	}    

	writel(GIC_CPU_BASE + GIC_CPU_EOI, irqstat);
#endif
}

int setIrqHandler(unsigned nIrq, TIRQHandler *pHandler, void *pParam)
{
    if (nIrq >= sizeof(s5p4418_irq_handler) || NULL == pHandler)
    {    
        DEBUG( INT_ON, "setIrqHandler param error: irq = %d\r\n", nIrq);
        return -1;
    }    
    DEBUG( INT_ON, "nIrq = %d\r\n", nIrq);
    s5p4418_irq_handler[nIrq].func = pHandler;
    s5p4418_irq_handler[nIrq].data = pParam;
    return 0;
}

TIRQHandler * getIrqHandler(unsigned nIrq)
{
    if (nIrq >= sizeof(s5p4418_irq_handler))
    {    
        return NULL;
    }    
    return s5p4418_irq_handler[nIrq].func;
}

int s5p4418_interrupt_init (void)
{
    s5p4418_irq_init();
    //GPIOB9引脚设置为ALTFH0时的功能是普通IO 
    gpio_set_cfg(S5P4418_GPIOB(9), ALTFH0); 
    gpio_set_pull(S5P4418_GPIOB(9), GPIO_PULL_UP);
    gpio_direction_input(S5P4418_GPIOB(9));
#if (defined SMP) && (defined CPU1_INTERRUPT)
    request_irq("GPIOB9", CPU1_ID, gpiob9_interrupt_func, IRQ_TYPE_EDGE_FALLING, 0); 
#else
    request_irq("GPIOB9", CPU0_ID, gpiob9_interrupt_func, IRQ_TYPE_EDGE_FALLING, 0); 
#endif
    //触摸屏中断事件
    //GPIOB29引脚设置为ALTFH1时的功能是普通IO
    gpio_set_cfg(S5P4418_GPIOB(29), ALTFH1);
    gpio_set_pull(S5P4418_GPIOB(29), GPIO_PULL_NONE);
    gpio_direction_input(S5P4418_GPIOB(29));
#if (defined SMP) && (defined CPU3_INTERRUPT)
    request_irq("GPIOB29", CPU3_ID, gpiob29_interrupt_func, IRQ_TYPE_LEVEL_LOW, 0);
#else
    request_irq("GPIOB29", CPU0_ID, gpiob29_interrupt_func, IRQ_TYPE_LEVEL_LOW, 0);
#endif

    return 0;
}
