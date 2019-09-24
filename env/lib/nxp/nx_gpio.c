#include <nxp/nx_gpio.h>
#include <nxp/nx_chip.h>
#include <types.h>
#include <io.h>
#include <nxp/nx_prototype.h>
#include <s5p4418_serial_stdio.h>
#include <config.h>
#include <stdio.h>

static  struct
{
    struct NX_GPIO_RegisterSet *pRegister;
} __g_ModuleVariables_gpio[NUMBER_OF_GPIO_MODULE] = { {NULL,}, };

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *  @brief      Get module's physical address.
 *  @param[in]  ModuleIndex     A index of module.
 *  @return     Module's physical address
 */
u32 NX_GPIO_GetPhysicalAddress( u32 ModuleIndex )
{
    static const u32 PhysicalAddr[] = { PHY_BASEADDR_LIST_ALPHA( GPIO ) }; // PHY_BASEADDR_GPIO?_MODULE

    return (u32)PhysicalAddr[ModuleIndex]; //ModuleIndex=3
}

boolean NX_GPIO_Initialize( void )
{
    static boolean bInit = FALSE;
    u32 i;

    if( FALSE == bInit )
    {    
        for( i=0; i < NUMBER_OF_GPIO_MODULE; i++ )
        {    
            __g_ModuleVariables_gpio[i].pRegister = NULL;
        }    

        bInit = TRUE;
    }    

    return TRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set a base address of register set.
 *  @param[in]  ModuleIndex     A index of module.
 *  @param[in]  BaseAddress Module's base address
 *  @return     None.
 */
void NX_GPIO_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
    __g_ModuleVariables_gpio[ModuleIndex].pRegister = (struct NX_GPIO_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a base address of register set
 *  @param[in]  ModuleIndex     A index of module.
 *  @return     Module's base address.
 */
u32     NX_GPIO_GetBaseAddress( u32 ModuleIndex )
{
    return (u32)__g_ModuleVariables_gpio[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set 2bit value
 *  @param[in]  Value       This Value is changed when SetBit2() executed.
 *  @param[in]  Bit         Bit number
 *  @param[in]  BitValue    Set value of bit
 *  @return     None.
 *  @see        NX_GPIO_GetBit2
 */
__inline void NX_GPIO_SetBit2
(
 volatile u32 *Value,
 u32 Bit,
 u32 BitValue
 )
{
    register u32 newvalue = *Value;

    newvalue = (u32)(newvalue & ~(3UL<<(Bit*2)));
    newvalue = (u32)(newvalue | (BitValue<<(Bit*2)));
    write32(Value, newvalue);
}

//--------------------------------------------------------------------------
// Pin Configuration
//--------------------------------------------------------------------------
/**
 *  @brief      Set PAD Fuction
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, 4:GPIOE )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 )
 *  @param[in]  padfunc     Pad Function
 *  @return     None
 *  @remarks        Each gpio pins can use to GPIO Pin or Alternate Function 0 or Alternate Function 1 or \n
 *              Alternate Function 2. So This function Sets gpio pin's function.
 */
void NX_GPIO_SetPadFunction( u32 ModuleIndex, u32 BitNumber, NX_GPIO_PADFUNC padfunc )
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    NX_GPIO_SetBit2( &pRegister->GPIOxALTFN[BitNumber/16], BitNumber%16, (u32)padfunc );
}

__inline u32 NX_GPIO_GetBit2
(
 u32 Value,
 u32 Bit
 )
{
    return (u32)((u32)(Value>>(Bit*2)) & 3UL);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get PAD Fuction
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, 4:GPIOE )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 )
 *  @return     None
 */
NX_GPIO_PADFUNC NX_GPIO_GetPadFunction( u32 ModuleIndex, u32 BitNumber )
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    return (NX_GPIO_PADFUNC)NX_GPIO_GetBit2( read32(&pRegister->GPIOxALTFN[BitNumber/16]), BitNumber%16 );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set GPIO Slew
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, 4:GPIOE )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 )
 *  @param[in]  Enable      Slew On/Off
 *  @return     None
 */
void NX_GPIO_SetSlew    ( u32 ModuleIndex, u32 BitNumber, boolean Enable )
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    NX_GPIO_SetBit(&pRegister->GPIOx_SLEW, BitNumber, Enable);
}

void NX_GPIO_SetSlewDisableDefault  ( u32 ModuleIndex, u32 BitNumber, boolean Enable )
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    NX_GPIO_SetBit(&pRegister->GPIOx_SLEW_DISABLE_DEFAULT, BitNumber, Enable);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set GPIO Drive Strength
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, 4:GPIOE )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 )
 *  @param[in]  DriveStrength
 *  @return     None
 */
void NX_GPIO_SetDriveStrength   ( u32 ModuleIndex, u32 BitNumber, NX_GPIO_DRVSTRENGTH drvstrength)
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    NX_GPIO_SetBit(&pRegister->GPIOx_DRV1, BitNumber, (boolean)(((u32)drvstrength>>0) & 0x1));
    NX_GPIO_SetBit(&pRegister->GPIOx_DRV0, BitNumber, (boolean)(((u32)drvstrength>>1) & 0x1));
}

void NX_GPIO_SetDriveStrengthDisableDefault ( u32 ModuleIndex, u32 BitNumber, boolean Enable )
{
    register struct NX_GPIO_RegisterSet *pRegister;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    NX_GPIO_SetBit(&pRegister->GPIOx_DRV1_DISABLE_DEFAULT, BitNumber, (boolean)(Enable) );
    NX_GPIO_SetBit(&pRegister->GPIOx_DRV0_DISABLE_DEFAULT, BitNumber, (boolean)(Enable) );
}


//------------------------------------------------------------------------------
/**
 *  @brief      Get bit value
 *  @param[in]  Value   Check this value of 1bit state
 *  @param[in]  Bit     Bit number
 *  @return     \b TRUE indicate that bit is Seted.\n
 *              \b FALSE    indicate that bit is cleared.
 *  @see        NX_GPIO_SetBit
 */
__inline boolean NX_GPIO_GetBit
(
 u32 Value,
 u32 Bit
 )
{
    return (boolean)(( Value >> Bit ) & ( 1UL ) );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get GPIO Drive Strength
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, 4:GPIOE )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 )
 *  @return     GPIO GPIO Drive Strength
 */
NX_GPIO_DRVSTRENGTH NX_GPIO_GetDriveStrength    ( u32 ModuleIndex, u32 BitNumber )
{
    register struct NX_GPIO_RegisterSet *pRegister;
    register u32 retvalue;

    pRegister = __g_ModuleVariables_gpio[ModuleIndex].pRegister;

    retvalue  = NX_GPIO_GetBit(read32(&pRegister->GPIOx_DRV0), BitNumber)<<1;
    retvalue |= NX_GPIO_GetBit(read32(&pRegister->GPIOx_DRV1), BitNumber)<<0;

    return (NX_GPIO_DRVSTRENGTH)retvalue;
}


void GPIO_SetALTFunction( u32 module_idx, u32 pad_num, u32 alt_function, u32 drivestrength )
{
    u32 GPIO_MODULE = (module_idx + 10);

    /* Change the GPIOx Alternate Function */
    DEBUG( GPIO_ON, " General Pupose I/O Controller. \r\n");
    DEBUG( GPIO_ON, "--------------------------------------\r\n");
    NX_GPIO_Initialize();
    NX_GPIO_SetBaseAddress( module_idx, NX_GPIO_GetPhysicalAddress( module_idx ) );
    DEBUG( GPIO_ON, " GPIO%X - Base Address: %8X \r\n", GPIO_MODULE, NX_GPIO_GetBaseAddress( module_idx ) );

    NX_GPIO_SetPadFunction( module_idx, pad_num, alt_function );
    DEBUG( GPIO_ON, " GPIO%X%2d PAD ALTFunction : %2d \r\n", GPIO_MODULE, pad_num, NX_GPIO_GetPadFunction( module_idx, pad_num ) );

    NX_GPIO_SetSlew( module_idx, pad_num, TRUE );
    NX_GPIO_SetSlewDisableDefault( module_idx, pad_num, TRUE );
    NX_GPIO_SetDriveStrength( module_idx, pad_num, drivestrength );
    NX_GPIO_SetDriveStrengthDisableDefault( module_idx, pad_num, TRUE );
    DEBUG( GPIO_ON, " GPIO%X%2d PAD Drive Strength : %2d \r\n", GPIO_MODULE, pad_num, NX_GPIO_GetDriveStrength( module_idx, pad_num ) );
}

void NX_GPIO_SetBit(volatile u32* Value, u32 Bit, boolean Enable)
{
    register u32 newvalue;

    newvalue = *Value;

    newvalue &=     ~(1UL       << Bit );
    newvalue |=     (u32)Enable << Bit ;

    write32(Value, newvalue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set gpio output value
 *  @param[in]  ModuleIndex     A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC )
 *  @param[in]  BitNumber   Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOC then Bit number is only 0 ~ 20.
 *  @param[in]  Value       \b TRUE indicate that High Level. \n
 *                          \b FALSE    indicate that Low Level.
 *  @return     None.
 *  @see        NX_GPIO_SetInterruptMode,       NX_GPIO_GetInterruptMode,
 *              NX_GPIO_SetOutputEnable,        NX_GPIO_GetOutputEnable,
 *                                              NX_GPIO_GetOutputValue,
 *              NX_GPIO_GetInputValue,          NX_GPIO_SetPullUpEnable,
 *              NX_GPIO_GetPullUpEnable,        NX_GPIO_SetPadFunction,
 *              NX_GPIO_GetPadFunction,         NX_GPIO_GetValidBit
 */
void NX_GPIO_SetOutputValue( u32 ModuleIndex, u32 BitNumber, boolean Value )
{
    NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOxOUT, BitNumber, Value );
}

void NX_GPIO_SetDRV1( u32 ModuleIndex, u32 Value )
{
	write32(&__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV1, Value );
}

u32	NX_GPIO_GetDRV1( u32 ModuleIndex )
{
	return __g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV1;
}

void NX_GPIO_SetDRV0( u32 ModuleIndex, u32 Value )
{
	write32(&__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV0, Value );
}

u32	NX_GPIO_GetDRV0( u32 ModuleIndex )
{
	return __g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV0;
}

u32 NX_GPIO_GetNumberOfModule( void )
{
	return NUMBER_OF_GPIO_MODULE;
}

void NX_GPIO_SetOutputEnable ( u32 ModuleIndex, u32 BitNumber, CBOOL OutputEnb )
{
	NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOxOUTENB, BitNumber, OutputEnb );
}

void NX_GPIO_SetInterruptMode( u32 ModuleIndex, u32 BitNumber, NX_GPIO_INTMODE IntMode )
{
	NX_GPIO_SetBit2( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOxDETMODE[BitNumber/16], BitNumber%16, (u32)IntMode & 0x03 );
	NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOxDETMODEEX, BitNumber, (u32)(IntMode>>2) );
}

void NX_GPIO_SetPullMode ( u32 ModuleIndex, u32 BitNumber, NX_GPIO_PADPULL mode)
{

	NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLSEL_DISABLE_DEFAULT, BitNumber, TRUE );
	NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLENB_DISABLE_DEFAULT, BitNumber, TRUE );

	if (mode == NX_GPIO_PADPULL_OFF)
	{
		NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLENB, BitNumber, FALSE );
		NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLSEL, BitNumber, FALSE );
	}
	else
	{
		NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLSEL, BitNumber, (mode & 1 ? TRUE : FALSE) );
		NX_GPIO_SetBit( &__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_PULLENB, BitNumber, TRUE );
	}
}

void set_gpio_strenth(u32 Group, u32 BitNumber, u32 mA) 
{
    u32 drv1=0, drv0=0;
    u32 drv1_value, drv0_value;

    switch( mA )
    {   
        case 0 : drv0 = 0; drv1 = 0; break;
        case 1 : drv0 = 0; drv1 = 1; break;
        case 2 : drv0 = 1; drv1 = 0; break;
        case 3 : drv0 = 1; drv1 = 1; break;
        default: drv0 = 0; drv1 = 0; break;
    }   

    drv1_value = NX_GPIO_GetDRV1(Group) & ~(1 << BitNumber);
    drv0_value = NX_GPIO_GetDRV0(Group) & ~(1 << BitNumber);
    if (drv1) drv1_value |= (drv1 << BitNumber);
    if (drv0) drv0_value |= (drv0 << BitNumber);

    NX_GPIO_SetDRV0 ( Group, drv0_value );
    NX_GPIO_SetDRV1 ( Group, drv1_value );
}

void NX_GPIO_SetDRV0_DISABLE_DEFAULT ( u32 ModuleIndex, u32 Value )
{
	write32(&__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV0_DISABLE_DEFAULT, Value );
}

void NX_GPIO_SetDRV1_DISABLE_DEFAULT ( u32 ModuleIndex, u32 Value )
{
	write32(&__g_ModuleVariables_gpio[ModuleIndex].pRegister->GPIOx_DRV1_DISABLE_DEFAULT, Value );
}




