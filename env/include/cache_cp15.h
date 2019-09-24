#ifndef _ENV_INCLUDE_CACHE_CP15_H_
#define _ENV_INCLUDE_CACHE_CP15_H_

#ifdef __cplusplus
extern "C" {
#endif

void    icache_enable(void);
void    icache_disable(void);
int     icache_status(void);
void    dcache_enable(void);
void    dcache_disable(void);
int     dcache_status(void);

#ifdef __cplusplus
}
#endif

#endif

