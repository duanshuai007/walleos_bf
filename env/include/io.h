#ifndef _ENV_INCLUDE_IO_H_
#define _ENV_INCLUDE_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define __arch_getb(a)          (*(volatile unsigned char *)(a))
#define __arch_getw(a)          (*(volatile unsigned short *)(a))
#define __arch_getl(a)          (*(volatile unsigned int *)(a))
#define __arch_getq(a)          (*(volatile unsigned long long *)(a))
//a -> address v -> 
#define __arch_putb(v,a)        (*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)        (*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)        (*(volatile unsigned int *)(a) = (v))
#define __arch_putq(v,a)        (*(volatile unsigned long long *)(a) = (v))

#define dmb()       __asm__ __volatile__ ("dmb" : : : "memory")
#define dsb()		__asm__ __volatile__ ("dsb" : : : "memory")
#define __iormb()   dmb()
#define __iowmb()   dmb()

#define __writeb(v,c) ({ u8  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
#define __writew(v,c) ({ u16 __v = v; __iowmb(); __arch_putw(__v,c); __v; })
#define __writel(v,c) ({ u32 __v = v; __iowmb(); __arch_putl(__v,c); __v; })
#define __writeq(v,c) ({ u64 __v = v; __iowmb(); __arch_putq(__v,c); __v; })

#define writeb(v,c) __writeb(c,v)
#define writew(v,c) __writew(c,v)
#define writel(v,c) __writel(c,v)
#define writeq(v,c) __writeq(c,v)

#define readb(c)    ({ u8  __v = __arch_getb(c); __iormb(); __v; })
#define readw(c)    ({ u16 __v = __arch_getw(c); __iormb(); __v; })
#define readl(c)    ({ u32 __v = __arch_getl(c); __iormb(); __v; })
#define readq(c)    ({ u64 __v = __arch_getq(c); __iormb(); __v; })

static inline physical_addr_t virt_to_phys(virtual_addr_t virt)
{
    return (physical_addr_t)virt;
}

static inline virtual_addr_t phys_to_virt(physical_addr_t phys)
{
    return (virtual_addr_t)phys;
}

#define read8(a)          (*(volatile unsigned char *)(a))
#define write8(a,v)       (*(volatile unsigned char *)(a) = (v))
#define read32(a)         (*(volatile unsigned int *)(a))
#define write32(a,v)      (*(volatile unsigned int *)(a) = (v))

inline unsigned int IO_ADDRESS(unsigned int addr)
{
    return addr;
}

enum bit{
    BIT_CLEAR   = 0,
    BIT_SET,
};

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */

