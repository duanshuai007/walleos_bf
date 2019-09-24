//------------------------------------------------------------------------------
//
//	Copyright (C) Nexell Co. 2012
//
//  This confidential and proprietary software may be used only as authorized by a
//  licensing agreement from Nexell Co.
//  The entire notice above must be reproduced on all authorized copies and copies
//  may only be made to the extent permitted by a licensing agreement from Nexell Co.
//
//	Module		: CLKGEN Ctrl
//	File		: nx_CLKGEN.h
//	Description	:
//	Author		: SOC Team
//	History		:
//------------------------------------------------------------------------------
#ifndef _ENV_INCLUDE_NXP_NX_RSTCON_H_
#define _ENV_INCLUDE_NXP_NX_RSTCON_H_

#include <nxp/nx_prototype.h>
#include <nxp/nx_chip.h>

#ifdef	__cplusplus
extern "C"
{
#endif

struct	NX_RSTCON_RegisterSet
{
    volatile u32	REGRST[__def_RSTCON__RSTREGISTERCNT];			//
};

typedef enum
{
    RSTCON_nDISABLE = 0UL,
    RSTCON_nENABLE	= 1UL
}RSTCON_nRST;

typedef enum
{
    RSTCON_ENABLE	= 1UL,
    RSTCON_DISABLE	= 0UL
}RSTCON_RST;

typedef enum
{
    RSTIDX_PWM_CH0 = RESETINDEX_OF_PWM_MODULE_PRESETn,
    RSTIDX_PWM_CH1 = RESETINDEX_OF_PWM_MODULE_PRESETn,
    RSTIDX_PWM_CH2 = RESETINDEX_OF_PWM_MODULE_PRESETn,
    RSTIDX_PWM_CH3 = RESETINDEX_OF_PWM_MODULE_PRESETn,
    RSTIDX_PWM_CH4 = RESETINDEX_OF_PWM_MODULE_PRESETn
} RSTIDX_PWM_CH;

boolean		NX_RSTCON_Initialize( void );
u32 		NX_RSTCON_GetPhysicalAddress( void );
u32			NX_RSTCON_GetSizeOfRegisterSet( void );
void		NX_RSTCON_SetBaseAddress( u32 BaseAddress );
u32			NX_RSTCON_GetBaseAddress( void );

void		NX_RSTCON_SetnRST(u32 RSTIndex, RSTCON_nRST STATUS);
void		NX_RSTCON_SetRST(u32 RSTIndex, RSTCON_RST STATUS);
RSTCON_nRST	NX_RSTCON_GetnRST(u32 RSTIndex);
RSTCON_RST	NX_RSTCON_GetRST(u32 RSTIndex);

void RstGen_SetIP( u8* module_name, u32 module_idx, u32 reset_idx, boolean status );
#ifdef	__cplusplus
}
#endif

#endif // __NX_CLKGEN_H__
