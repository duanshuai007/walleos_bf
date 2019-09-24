#ifndef _ENV_INCLUDE_SMP_H_
#define _ENV_INCLUDE_SMP_H_

#include <drivers/compiler.h>
#include <types.h>

struct sGICD_TYPE{
    u8 ucITLinesNumber;
    u8 ucCPUNumber;
    u8 ucSecurityExtn;
    u8 ucLSPI;
};

//unsigned int __attribute_const__ read_cpuid_mpidr(void);
void secondary_startup(unsigned int cpu);
void platform_smp_prepare_cpus(void);
void gic_cpu_init(void);
void smp_cpu_base_init(void);
void smp_setup_processor_id(void);
void gic_init_bases(void);
void gic_raise_softirq(unsigned int cpu, unsigned int irq);
int cpu_up(unsigned int cpu);

#endif
