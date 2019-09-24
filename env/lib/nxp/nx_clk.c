#include <nxp/nx_clk.h>
#include <nxp/nx_chip.h>
#include <nxp/nx_prototype.h>
#include <types.h>
#include <io.h>
#include <s5p4418_serial_stdio.h>
#include <config.h>
#include <stdio.h>

struct NX_CLKGEN_RegisterSet* __g_ModuleVariables_clk[NUMBER_OF_CLKGEN_MODULE] = { NULL, };

boolean NX_CLKGEN_Initialize( void )
{
    static boolean bInit = FALSE;
    u32 i;

    if( FALSE == bInit )
    {   
        for( i=0; i < NUMBER_OF_CLKGEN_MODULE; i++ )
        {   
            __g_ModuleVariables_clk[i] = NULL;
        }   

        bInit = TRUE;
    }   

    return TRUE;
}

void NX_CLKGEN_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
    __g_ModuleVariables_clk[ModuleIndex] = (struct NX_CLKGEN_RegisterSet *)BaseAddress;
}

u32 NX_CLKGEN_GetPhysicalAddress( u32 ModuleIndex )
{
    static const u32 PhysicalAddr[] = { PHY_BASEADDR_LIST( CLKGEN ) }; // PHY_BASEADDR_CLKGEN_MODULE
    return (u32)PhysicalAddr[ModuleIndex];
}

u32 NX_CLKGEN_GetBaseAddress( u32 ModuleIndex )
{
    return (u32)__g_ModuleVariables_clk[ModuleIndex];
}

void NX_CLKGEN_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode )
{
    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
    register u32 regvalue, clkmode=0;
    const u32 PCLKMODE_POS  =   3;

    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];

    switch(mode)
    {
        case NX_PCLKMODE_DYNAMIC:   clkmode = 0;        break;
        case NX_PCLKMODE_ALWAYS:    clkmode = 1;        break;
        default: break;
    }

    regvalue = read32(&__g_pRegister->CLKENB);

    regvalue &= ~(1UL<<PCLKMODE_POS);
    regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

    write32(&__g_pRegister->CLKENB, regvalue);
}


void NX_CLKGEN_SetClockBClkMode( u32 ModuleIndex, NX_BCLKMODE mode )
{
    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
    register u32 regvalue, clkmode=0;

    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];

    switch(mode)
    {
        case NX_BCLKMODE_DISABLE:   clkmode = 0;
        case NX_BCLKMODE_DYNAMIC:   clkmode = 2;        break;
        case NX_BCLKMODE_ALWAYS:    clkmode = 3;        break;
        default: break;
    }

    regvalue = read32(&__g_pRegister->CLKENB);
    regvalue &= ~0x3UL;
    regvalue |= ( clkmode & 0x03UL );
    write32(&__g_pRegister->CLKENB, regvalue);
}


//------------------------------------------------------------------------------
/**
 *  @brief      Get current PCLK mode
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @return     Current PCLK mode
 */
//NX_PCLKMODE NX_CLKGEN_GetClockPClkMode( u32 ModuleIndex )
//{
//    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
//    const u32 PCLKMODE_POS  = 3;
//
//    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];
//
//    if( read32(&__g_pRegister->CLKENB) & ( 1UL << PCLKMODE_POS ) )
//    {
//        return NX_PCLKMODE_ALWAYS;
//    }
//
//    return  NX_PCLKMODE_DYNAMIC;
//}

//NX_BCLKMODE NX_CLKGEN_GetClockBClkMode( u32 ModuleIndex )
//{
//    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
//    u32 mode=0;
//
//    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];
//
//    mode = ( read32(&__g_pRegister->CLKENB) & 3UL );
//    switch(mode)
//    {
//        case 0: return NX_BCLKMODE_DISABLE;
//        case 2: return NX_BCLKMODE_DYNAMIC;
//        case 3: return NX_BCLKMODE_ALWAYS ;
//        default: break;
//    }
//    return  NX_BCLKMODE_DISABLE;
//}

//------------------------------------------------------------------------------
/**
 *  @brief      Set clock source of clock generator
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @param[in]  Index   Select clock generator( 0 : clock generator 0 );
 *  @param[in]  ClkSrc  Select clock source of clock generator ( 0: PLL0, 1:PLL1 ).
 *  @remarks    CLKGEN have one clock generator. so \e Index must set to 0.
 *  @return     None.
 */
void NX_CLKGEN_SetClockSource( u32 ModuleIndex, u32 Index, u32 ClkSrc )
{
    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
    const u32 CLKSRCSEL_POS     = 2;
    const u32 CLKSRCSEL_MASK    = 0x07 << CLKSRCSEL_POS;

    register u32 ReadValue;

    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];

    ReadValue = read32(&__g_pRegister->CLKGEN[Index<<1]);

    ReadValue &= ~CLKSRCSEL_MASK;
    ReadValue |= ClkSrc << CLKSRCSEL_POS;

    write32(&__g_pRegister->CLKGEN[Index<<1], ReadValue);
}


//------------------------------------------------------------------------------
/**
 *  @brief      Set clock divisor of specified clock generator.
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @param[in]  Index       Select clock generator( 0 : clock generator 0 )
 *  @param[in]  Divisor     Clock divisor ( 1 ~ 256 ).
 *  @return     None.
 *  @remarks    CLKGEN have one clock generator. so \e Index must set to 0.
 */
void NX_CLKGEN_SetClockDivisor( u32 ModuleIndex, u32 Index, u32 Divisor )
{
    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
    const u32 CLKDIV_POS    =   5;
    const u32 CLKDIV_MASK   =   0xFF << CLKDIV_POS;

    register u32 ReadValue;

    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];

    ReadValue   =   read32(&__g_pRegister->CLKGEN[Index<<1]);

    ReadValue   &= ~CLKDIV_MASK;
    ReadValue   |= (Divisor-1) << CLKDIV_POS;

    write32(&__g_pRegister->CLKGEN[Index<<1], ReadValue);
}



//------------------------------------------------------------------------------
/**
 *  @brief      Set clock generator's operation
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @param[in]  Enable  \b TRUE indicates that Enable of clock generator. \n
 *                      \b FALSE    indicates that Disable of clock generator.
 *  @return     None.
 */
void NX_CLKGEN_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable )
{
    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
    const u32   CLKGENENB_POS   =   2;
    const u32   CLKGENENB_MASK  =   1UL << CLKGENENB_POS;

    register u32 ReadValue;

    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];

    ReadValue   =   read32(&__g_pRegister->CLKENB);
    ReadValue   &=  ~CLKGENENB_MASK;
    ReadValue   |= (u32)Enable << CLKGENENB_POS;

    write32(&__g_pRegister->CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get clock source of specified clock generator.
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @param[in]  Index   Select clock generator( 0 : clock generator 0 );
 *  @return     Clock source of clock generator ( 0:PLL0, 1:PLL1 ).
 *  @remarks    CLKGEN have one clock generator. so \e Index must set to 0.
 */
//u32 NX_CLKGEN_GetClockSource( u32 ModuleIndex, u32 Index )
//{
//    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
//    const u32 CLKSRCSEL_POS     = 2;
//    const u32 CLKSRCSEL_MASK    = 0x07 << CLKSRCSEL_POS;
//
//    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];
//
//    return ( read32(&__g_pRegister->CLKGEN[Index<<1]) & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
//}


//------------------------------------------------------------------------------
/**
 *  @brief      Get clock divisor of specified clock generator.
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @param[in]  Index       Select clock generator( 0 : clock generator 0   );
 *  @return     Clock divisor ( 1 ~ 256 ).
 *  @remarks    CLKGEN have one clock generator. so \e Index must set to 0.
 */
//u32 NX_CLKGEN_GetClockDivisor( u32 ModuleIndex, u32 Index )
//{
//    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
//    const u32 CLKDIV_POS    =   5;
//    const u32 CLKDIV_MASK   =   0xFF << CLKDIV_POS;
//
//    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];
//
//    return ((read32(&__g_pRegister->CLKGEN[Index<<1]) & CLKDIV_MASK) >> CLKDIV_POS) + 1;
//}
//

//------------------------------------------------------------------------------
/**
 *  @brief      Get status of clock generator's operation
 *  @param[in]  ModuleIndex     An index of module ( 0 ~ 1 ).
 *  @return     \b TRUE indicates that Clock generator is enabled. \n
 *              \b FALSE    indicates that Clock generator is disabled.
 */
//boolean NX_CLKGEN_GetClockDivisorEnable( u32 ModuleIndex )
//{
//    register struct NX_CLKGEN_RegisterSet* __g_pRegister;
//    const u32   CLKGENENB_POS   =   2;
//    const u32   CLKGENENB_MASK  =   1UL << CLKGENENB_POS;
//
//    __g_pRegister = __g_ModuleVariables_clk[ModuleIndex];
//
//    return  (boolean)( (read32(&__g_pRegister->CLKENB) & CLKGENENB_MASK) >> CLKGENENB_POS );
//}

void NX_CLKGEN_SetClockOutInv( u32 ModuleIndex, u32 Index, CBOOL OutClkInv )
{
    const u32 OUTCLKINV_POS =   1;
    const u32 OUTCLKINV_MASK    =   1UL << OUTCLKINV_POS;

    register u32 ReadValue;

    ReadValue   =   __g_ModuleVariables_clk[ModuleIndex]->CLKGEN[Index<<1];

    ReadValue   &=  ~OUTCLKINV_MASK;
    ReadValue   |=  OutClkInv << OUTCLKINV_POS;

    writel(&__g_ModuleVariables_clk[ModuleIndex]->CLKGEN[Index<<1], ReadValue);
}

void ClkGen_SetConfig( u32 clkgen_idx, u32 clkgen_subidx, u32 clkgen_src, u32 clkgen_div, boolean status )
{
    /* Clock Generator Initialize */
    DEBUG( CLK_ON, "\r\n Clock Generation Controller. \r\n");
    DEBUG( CLK_ON, "--------------------------------------\r\n");
    NX_CLKGEN_Initialize();
    NX_CLKGEN_SetBaseAddress( clkgen_idx, NX_CLKGEN_GetPhysicalAddress( clkgen_idx ) );
    DEBUG( CLK_ON, " CLKGEN - Base Address: %8X \r\n", NX_CLKGEN_GetBaseAddress( clkgen_idx ) );

    if( (clkgen_subidx & CLKGEN_SUBIDX_PCLK) == CLKGEN_SUBIDX_PCLK )
    {
        NX_CLKGEN_SetClockPClkMode( clkgen_idx, status );
        if( (clkgen_subidx & CLKGEN_SUBIDX_BCLK) == CLKGEN_SUBIDX_BCLK )
            NX_CLKGEN_SetClockBClkMode( clkgen_idx, status );
        DEBUG( CLK_ON, " PClk Enable Bit : %8s \r\n", NX_CLKGEN_GetClockPClkMode( clkgen_idx ) ? "ENABLE" : "DISABLE"  );
        DEBUG( CLK_ON, " BClk Enable Bit : %8s \r\n", NX_CLKGEN_GetClockBClkMode( clkgen_idx ) ? "ENABLE" : "DISABLE"  );
    }

    if( (clkgen_subidx & CLKGEN_SUBIDX_LEVEL_0) == CLKGEN_SUBIDX_LEVEL_0 )
    {
        NX_CLKGEN_SetClockSource( clkgen_idx, CLKGEN_SUBIDX_LEVEL_0, clkgen_src );
        NX_CLKGEN_SetClockDivisor( clkgen_idx, CLKGEN_SUBIDX_LEVEL_0, clkgen_div );
        NX_CLKGEN_SetClockDivisorEnable( clkgen_idx, status );
    }
    if( (clkgen_subidx & CLKGEN_SUBIDX_LEVEL_1) == CLKGEN_SUBIDX_LEVEL_1 )
    {
        NX_CLKGEN_SetClockSource( clkgen_idx, CLKGEN_SUBIDX_LEVEL_1, clkgen_src );
        NX_CLKGEN_SetClockDivisor( clkgen_idx, CLKGEN_SUBIDX_LEVEL_1, clkgen_div );
        NX_CLKGEN_SetClockDivisorEnable( clkgen_idx, status );
    }
    if( (clkgen_subidx & CLKGEN_SUBIDX_LEVEL_2) == CLKGEN_SUBIDX_LEVEL_2 )
    {
        NX_CLKGEN_SetClockSource( clkgen_idx, CLKGEN_SUBIDX_LEVEL_2, clkgen_src );
        NX_CLKGEN_SetClockDivisor( clkgen_idx, CLKGEN_SUBIDX_LEVEL_2, clkgen_div );
        NX_CLKGEN_SetClockDivisorEnable( clkgen_idx, status );
    }
    DEBUG( CLK_ON, " Clock Gen   Index : %8d \r\n", clkgen_idx );
    DEBUG( CLK_ON, " Source Select Bit : %8d \r\n", NX_CLKGEN_GetClockSource( clkgen_idx, 0 ) );
    DEBUG( CLK_ON, " Clock Divisor Bit : %8d \r\n", NX_CLKGEN_GetClockDivisor( clkgen_idx, 0 ) );
    DEBUG( CLK_ON, " Generator Enable Bit : %8s \r\n", NX_CLKGEN_GetClockDivisorEnable( clkgen_idx ) ? "ENABLE" : "DISABLE" );
    DEBUG( CLK_ON, "\r\n");
}

