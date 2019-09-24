#ifndef _ENV_INCLUDE_NXP_NX_PWM_H_
#define _ENV_INCLUDE_NXP_NX_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nxp/nx_chip.h>
#include <types.h>

#define NUMBER_OF_PWM__MODULE           1
#define PHY_BASEADDR_PWM__MODULE        0xC0018000

#define PWM                 0
#define NX_PWM_CHANNEL      4

typedef enum 
{
	NX_PWM_LOADMODE_ONESHOT		= 0UL,
	NX_PWM_LOADMODE_AUTORELOAD	= 1UL
} NX_PWM_LOADMODE;

enum pwm_tcon_reg_ofs{
    START_STOP      = 0,
    UPDATE          = 1,
    OUTPUT          = 2,
    AUTO_RELOAD     = 3,
    DEADZONE        = 4,
    REG_OFFSET      = 4,
};

typedef enum 
{
	NX_PWM_DIVIDSELECT_1	= 0UL,
	NX_PWM_DIVIDSELECT_2	= 1UL,
	NX_PWM_DIVIDSELECT_4	= 2UL,
	NX_PWM_DIVIDSELECT_8	= 3UL,
	NX_PWM_DIVIDSELECT_16 	= 4UL,
	NX_PWM_DIVIDSELECT_TCLK	= 5UL

} NX_PWM_DIVIDSELECT;

typedef enum
{
	ALT_PWM_CH0 = 1,
	ALT_PWM_CH1 = 2,
	ALT_PWM_CH2 = 2,
	ALT_PWM_CH3 = 2
} ALT_PWM_CH;

typedef enum
{
	GPIO_PWM_CH0_PAD = 1,
	GPIO_PWM_CH1_PAD = 13,
	GPIO_PWM_CH2_PAD = 14,
	GPIO_PWM_CH3_PAD = 0
} PAD_PWM_CH;

typedef enum
{
	GPIO_PWM_CH0 = 3,
	GPIO_PWM_CH1 = 2,
	GPIO_PWM_CH2 = 2,
	GPIO_PWM_CH3 = 3
} GPIO_PWM_CH;

typedef enum
{
    PWM_CH0 = 0,
    PWM_CH1 = 1,
    PWM_CH2 = 2,
    PWM_CH3 = 3,
    PWM_CH4 = 4

} PWM_CH;

typedef struct
{
	u32		Channel;
	u32		Count;
	u32		Compare;
	u32		Prescaler;
	u32		Divider;
	boolean	Slew;
	u32		DriveStregth;
} PWM_CMD;

//------------------------------------------------------------------------------
/// @brief  PWM register set structure
//------------------------------------------------------------------------------
struct NX_PWM_RegisterSet
{
    u32 TCFG0		;	// 0x00	// R/W	// PWM Configuration Register 0 that configures the two 8-bit Prescaler and DeadZone Length
    u32 TCFG1		;	// 0x04	// R/W	// PWM Configuration Register 1 that controls 5 MUX and DMA Mode Select Bit
    u32 TCON       ;	// 0x08	// R/W	// PWM Control Register
    u32 TCNTB0     ;	// 0x0C	// R/W	// PWM 0 Count Buffer Register
    u32 TCMPB0     ;	// 0x10	// R/W	// PWM 0 Compare Buffer Register
    u32 TCNTO0     ;	// 0x14	// R	// PWM 0 Count Observation Register
    u32 TCNTB1     ;	// 0x18	// R/W	// PWM 1 Count Buffer Register
    u32 TCMPB1     ;	// 0x1C	// R/W	// PWM 1 Compare Buffer Register
    u32 TCNTO1     ;	// 0x20	// R	// PWM 1 Count Observation Register
    u32 TCNTB2     ;	// 0x24	// R/W	// PWM 2 Count Buffer Register
    u32 TCMPB2     ;	// 0x28	// R/W	// PWM 2 Compare Buffer Register
    u32 TCNTO2     ;	// 0x2C	// R	// PWM 2 Count Observation Register
    u32 TCNTB3     ;	// 0x30	// R/W	// PWM 3 Count Buffer Register
    u32 TCMPB3     ;	// 0x34	// R/W	// PWM 3 Compare Buffer Register
    u32 TCNTO3     ;	// 0x38	// R	// PWM 3 Count Observation Register
    u32 TCNTB4     ;	// 0x3C	// R/W	// PWM 4 Count Buffer Register
    u32 TCNTO4     ;	// 0x40	// R	// PWM 4 Count Observation Register
    u32 TINT_CSTAT ;	// 0x44	// R/W	// PWM Interrupt Control and Status Register
};

void PWM_IO_Init( PWM_CMD* CmdSet );

#ifdef __cplusplus
}
#endif
#endif	/* _NX_PWM_H_*/

