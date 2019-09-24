#ifndef _ENV_INCLUDE_NXP_NX_CLK_H_
#define _ENV_INCLUDE_NXP_NX_CLK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <nxp/nx_clockcontrol.h>
#include <nxp/nx_chip.h>

struct  NX_CLKGEN_RegisterSet
{
    u32 CLKENB;         ///< 0x40 : Clock Enable Register
    u32 CLKGEN[4];          ///< 0x44 : Clock Generate Register
};

enum
{
    CLKGEN_SUBIDX_PCLK      = 3,
    CLKGEN_SUBIDX_BCLK      = 4,
    CLKGEN_SUBIDX_LEVEL_0   = 0,
    CLKGEN_SUBIDX_LEVEL_1   = 1,
    CLKGEN_SUBIDX_LEVEL_2   = 2
};

typedef enum
{
    CLKIDX_PWM_CH0 = CLOCKINDEX_OF_PWM_MODULE,
    CLKIDX_PWM_CH1 = CLOCKINDEX_OF_PWM_MODULE,
    CLKIDX_PWM_CH2 = CLOCKINDEX_OF_Inst_PWM01_MODULE,
    CLKIDX_PWM_CH3 = CLOCKINDEX_OF_Inst_PWM01_MODULE,
    CLKIDX_PWM_CH4 = CLOCKINDEX_OF_Inst_PWM01_MODULE
} CLKIDX_PWM_CH;

boolean NX_CLKGEN_Initialize( void );
void NX_CLKGEN_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32 NX_CLKGEN_GetPhysicalAddress( u32 ModuleIndex );
u32 NX_CLKGEN_GetBaseAddress( u32 ModuleIndex );
void NX_CLKGEN_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode );
void NX_CLKGEN_SetClockBClkMode( u32 ModuleIndex, NX_BCLKMODE mode );
NX_PCLKMODE NX_CLKGEN_GetClockPClkMode( u32 ModuleIndex );
NX_BCLKMODE NX_CLKGEN_GetClockBClkMode( u32 ModuleIndex );
void NX_CLKGEN_SetClockSource( u32 ModuleIndex, u32 Index, u32 ClkSrc );
void NX_CLKGEN_SetClockDivisor( u32 ModuleIndex, u32 Index, u32 Divisor );
void NX_CLKGEN_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable );
u32 NX_CLKGEN_GetClockSource( u32 ModuleIndex, u32 Index );
u32 NX_CLKGEN_GetClockDivisor( u32 ModuleIndex, u32 Index );
boolean NX_CLKGEN_GetClockDivisorEnable( u32 ModuleIndex );
void ClkGen_SetConfig( u32 clkgen_idx, u32 clkgen_subidx, u32 clkgen_src, u32 clkgen_div, boolean status );
void NX_CLKGEN_SetClockOutInv( u32 ModuleIndex, u32 Index, CBOOL OutClkInv );

#ifdef __cplusplus
}
#endif

#endif
