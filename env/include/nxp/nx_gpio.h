#ifndef _ENV_INCLUDE_NXP_NX_GPIO_H_
#define _ENV_INCLUDE_NXP_NX_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>


typedef enum
{
    NX_GPIO_DRVSTRENGTH_0       = 0UL,
    NX_GPIO_DRVSTRENGTH_1       = 1UL,
    NX_GPIO_DRVSTRENGTH_2       = 2UL,
    NX_GPIO_DRVSTRENGTH_3       = 3UL

}NX_GPIO_DRVSTRENGTH;

/// @brief  GPIO interrupt mode
typedef enum
{
    NX_GPIO_INTMODE_LOWLEVEL    = 0UL,      ///< Low level detect
    NX_GPIO_INTMODE_HIGHLEVEL   = 1UL,      ///< High level detect
    NX_GPIO_INTMODE_FALLINGEDGE = 2UL,      ///< Falling edge detect
    NX_GPIO_INTMODE_RISINGEDGE  = 3UL,      ///< Rising edge detect
    NX_GPIO_INTMODE_BOTHEDGE    = 4UL       ///< both (rise and falling) edge detect

}NX_GPIO_INTMODE;

typedef enum
{
    NX_GPIO_PADFUNC_0           = 0UL,          ///< Alternate function 0
    NX_GPIO_PADFUNC_1           = 1UL,          ///< Alternate function 1
    NX_GPIO_PADFUNC_2           = 2UL,          ///< Alternate function 2
    NX_GPIO_PADFUNC_3           = 3UL           ///< Alternate function 3
}NX_GPIO_PADFUNC ;

/// @brief  Pull I/O mode
typedef enum
{
    NX_GPIO_PADPULL_DN          = 0UL,
    NX_GPIO_PADPULL_UP          = 1UL,
    NX_GPIO_PADPULL_OFF         = 2UL
}NX_GPIO_PADPULL;

enum
{
    GPIO_DRVSTRENGTH_0 = 0,
    GPIO_DRVSTRENGTH_1 = 1,
    GPIO_DRVSTRENGTH_2 = 2,
    GPIO_DRVSTRENGTH_3 = 3
};

/// @brief  GPIO Module's Register List
struct  NX_GPIO_RegisterSet
{
    volatile u32 GPIOxOUT;          ///< 0x00   : Output Register
    volatile u32 GPIOxOUTENB;       ///< 0x04   : Output Enable Register
    volatile u32 GPIOxDETMODE[2];   ///< 0x08   : Event Detect Mode Register
    volatile u32 GPIOxINTENB;       ///< 0x10   : Interrupt Enable Register
    volatile u32 GPIOxDET;          ///< 0x14   : Event Detect Register
    volatile u32 GPIOxPAD;          ///< 0x18   : PAD Status Register
    volatile u32 GPIOxPUENB;        ///< 0x1C   : Pull Up Enable Register
    volatile u32 GPIOxALTFN[2];     ///< 0x20   : Alternate Function Select Register
    volatile u32 GPIOxDETMODEEX;    ///< 0x28   : Event Detect Mode extended Register

    volatile u32 __Reserved[4];     ///< 0x2B   :
    volatile u32 GPIOxDETENB;       ///< 0x3C   : IntPend Detect Enable Register
    // @added charles 20121127 RTL���� �ִµ� proto���� ��� �߰�
    volatile u32 GPIOx_SLEW;                    ///< 0x40
    volatile u32 GPIOx_SLEW_DISABLE_DEFAULT;    ///< 0x44
    volatile u32 GPIOx_DRV1;                    ///< 0x48
    volatile u32 GPIOx_DRV1_DISABLE_DEFAULT;    ///< 0x4C
    volatile u32 GPIOx_DRV0;                    ///< 0x50
    volatile u32 GPIOx_DRV0_DISABLE_DEFAULT;    ///< 0x54
    volatile u32 GPIOx_PULLSEL;                 ///< 0x58
    volatile u32 GPIOx_PULLSEL_DISABLE_DEFAULT; ///< 0x5C
    volatile u32 GPIOx_PULLENB;                 ///< 0x60
    volatile u32 GPIOx_PULLENB_DISABLE_DEFAULT; ///< 0x64
    volatile u32 GPIOx_InputMuxSelect0;         ///< 0x68
    volatile u32 GPIOx_InputMuxSelect1;         ///< 0x6C
    u8 __Reserved1[0x1000-0x70];
};

u32 NX_GPIO_GetPhysicalAddress( u32 ModuleIndex );
boolean NX_GPIO_Initialize( void );
void NX_GPIO_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32     NX_GPIO_GetBaseAddress( u32 ModuleIndex );
void NX_GPIO_SetPadFunction( u32 ModuleIndex, u32 BitNumber, NX_GPIO_PADFUNC padfunc);
NX_GPIO_PADFUNC NX_GPIO_GetPadFunction( u32 ModuleIndex, u32 BitNumber );
void NX_GPIO_SetSlew    ( u32 ModuleIndex, u32 BitNumber, boolean Enable );
void NX_GPIO_SetSlewDisableDefault  ( u32 ModuleIndex, u32 BitNumber, boolean Enable);
void NX_GPIO_SetDriveStrength   ( u32 ModuleIndex, u32 BitNumber, NX_GPIO_DRVSTRENGTH drvstrength);
void NX_GPIO_SetDriveStrengthDisableDefault ( u32 ModuleIndex, u32 BitNumber, boolean Enable);
NX_GPIO_DRVSTRENGTH NX_GPIO_GetDriveStrength    ( u32 ModuleIndex, u32 BitNumber );
void GPIO_SetALTFunction( u32 module_idx, u32 pad_num, u32 alt_function, u32 drivestrength );
void NX_GPIO_SetBit(volatile u32* Value, u32 Bit, boolean Enable);
void    NX_GPIO_SetOutputValue  ( u32 ModuleIndex, u32 BitNumber, boolean Value );
void NX_GPIO_SetDRV1( u32 ModuleIndex, u32 Value );
u32 NX_GPIO_GetDRV1( u32 ModuleIndex );
void NX_GPIO_SetDRV0( u32 ModuleIndex, u32 Value );
u32 NX_GPIO_GetDRV0( u32 ModuleIndex );
u32 NX_GPIO_GetNumberOfModule( void );
void NX_GPIO_SetOutputEnable ( u32 ModuleIndex, u32 BitNumber, CBOOL OutputEnb );
void NX_GPIO_SetInterruptMode( u32 ModuleIndex, u32 BitNumber, NX_GPIO_INTMODE IntMode );
void NX_GPIO_SetPullMode ( u32 ModuleIndex, u32 BitNumber, NX_GPIO_PADPULL mode);
void set_gpio_strenth(u32 Group, u32 BitNumber, u32 mA);
void NX_GPIO_SetDRV0_DISABLE_DEFAULT ( u32 ModuleIndex, u32 Value );
void NX_GPIO_SetDRV1_DISABLE_DEFAULT ( u32 ModuleIndex, u32 Value );

#ifdef __cplusplus
}
#endif

#endif
