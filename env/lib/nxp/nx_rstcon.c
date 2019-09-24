//------------------------------------------------------------------------------
//
//	Copyright (C) Nexell Co. 2012
//
//  This confidential and proprietary software may be used only as authorized by a
//  licensing agreement from Nexell Co.
//  The entire notice above must be reproduced on all authorized copies and copies
//  may only be made to the extent permitted by a licensing agreement from Nexell Co.
//
//	Module		: Interrupt Controller
//	File		: nx_rstcon.c
//	Description	:
//	Author		:
//	History		:
//					2007/04/03	first
//------------------------------------------------------------------------------
#include <nxp/nx_rstcon.h>
//#include <nxp/nx_debug.h>
#include <assert.h>
#include <io.h>
#include <types.h>
#include <s5p4418_serial_stdio.h>
#include <s5p4418_rstcon.h>
#include <config.h>
#include <stdio.h>

static struct NX_RSTCON_RegisterSet *__g_pRegister;

boolean	NX_RSTCON_Initialize( void )
{
    static boolean bInit = FALSE;

    if( FALSE == bInit )
    {
        __g_pRegister = NULL;
        bInit = TRUE;
    }
    return TRUE;
}

u32 NX_RSTCON_GetPhysicalAddress( void )
{
    const u32 PhysicalAddr[] =  {   PHY_BASEADDR_LIST( RSTCON )  }; // PHY_BASEADDR_RSTCON_MODULE
    assert( NUMBER_OF_RSTCON_MODULE == (sizeof(PhysicalAddr)/sizeof(PhysicalAddr[0])) );
    assert( PHY_BASEADDR_RSTCON_MODULE == PhysicalAddr[0] );
    return (u32)PhysicalAddr[0];
}

u32 NX_RSTCON_GetSizeOfRegisterSet( void )
{
    return sizeof(struct NX_RSTCON_RegisterSet);
}

void NX_RSTCON_SetBaseAddress( u32 BaseAddress )
{
    assert( 0 != BaseAddress );
    __g_pRegister = (struct NX_RSTCON_RegisterSet *)BaseAddress;
}

u32 NX_RSTCON_GetBaseAddress( void )
{
    return (u32)__g_pRegister;
}

void NX_RSTCON_SetnRST(u32 RSTIndex, RSTCON_nRST STATUS)
{
    u32 regNum, bitPos, curStat;
    regNum 		= RSTIndex >> 5;
    curStat		= (u32)read32(&__g_pRegister->REGRST[regNum]);
    bitPos		= RSTIndex & RESET_ID_MASK;
    curStat		&= ~(1UL << bitPos);
    curStat		|= (STATUS & 0x01) << bitPos;
    write32(&__g_pRegister->REGRST[regNum], curStat);
}

void NX_RSTCON_SetRST(u32 RSTIndex, RSTCON_RST STATUS)
{
    u32 regNum, bitPos, curStat;
    regNum 		= RSTIndex >> 5;
    curStat		= (u32)read32(&__g_pRegister->REGRST[regNum]);
    bitPos		= RSTIndex & RESET_ID_MASK;
    curStat		&= ~(1UL << bitPos);
    curStat		|= (STATUS & 0x01) << bitPos;
    DEBUG( RSTGEN_ON, "rst : &__g_pRegister->REGRST[regNum] = %08x, write in %08x \r\n", &__g_pRegister->REGRST[regNum], curStat);
	write32(&__g_pRegister->REGRST[regNum], curStat);
}

RSTCON_nRST NX_RSTCON_GetnRST(u32 RSTIndex)
{
    u32 regNum, bitPos, curStat;
    regNum 		= RSTIndex >> 5;
    curStat		= (u32)read32(&__g_pRegister->REGRST[regNum]);
    bitPos	= RSTIndex & RESET_ID_MASK;
    curStat		= 0x01 & (curStat >> bitPos);
    return (RSTCON_nRST) curStat;
}

RSTCON_RST NX_RSTCON_GetRST(u32 RSTIndex)
{
    u32 regNum, bitPos, curStat;
    regNum 		= RSTIndex >> 5;
    curStat		= (u32)read32(&__g_pRegister->REGRST[regNum]);
    bitPos	= RSTIndex & RESET_ID_MASK;
    curStat		= 0x01 & (curStat >> bitPos);

    return (RSTCON_RST) curStat;
}

void RstGen_SetIP( u8* module_name, u32 module_idx, u32 reset_idx, boolean status )
{
    /* Change the Reset Generation Register (1: No Reset)  */
    DEBUG( RSTGEN_ON, "\r\nReset Generation Controller. \r\n");
    DEBUG( RSTGEN_ON, "--------------------------------------\r\n");
    NX_RSTCON_Initialize();
    NX_RSTCON_SetBaseAddress( NX_RSTCON_GetPhysicalAddress() );
    DEBUG( RSTGEN_ON, " RSTCON - Base Address   : %8X \r\n", NX_RSTCON_GetBaseAddress() );
    NX_RSTCON_SetRST( reset_idx, 0 );
    NX_RSTCON_SetRST( reset_idx, status );
    DEBUG( RSTGEN_ON, " %s%d IP Control Register: %8s \r\n", module_name, module_idx, NX_RSTCON_GetRST( reset_idx ) ? "NO RESET" : "RESET" );
    DEBUG( RSTGEN_ON, "\r\n");
}
