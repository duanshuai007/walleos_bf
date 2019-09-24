#include <types.h>
#include <nxp/nx_pwm.h>
#include <nxp/nx_rstcon.h>
#include <nxp/nx_clk.h>
#include <nxp/nx_clockcontrol.h>
#include <nxp/nx_gpio.h>
#include <s5p4418_serial_stdio.h>
#include <io.h>
#include <s5p4418_clk.h>
#include <config.h>
#include <stdio.h>

static struct NX_PWM_RegisterSet *__g_pRegister_pwm[5];

static u32 RSTIDX_PWM[5] =
{
    RSTIDX_PWM_CH0, RSTIDX_PWM_CH1, RSTIDX_PWM_CH2, RSTIDX_PWM_CH3
};

static u32 GPIO_PWM_ALT[5] =
{
    ALT_PWM_CH0, ALT_PWM_CH1, ALT_PWM_CH2, ALT_PWM_CH3
};
static u32 GPIO_PWM_PAD[5] =
{
	GPIO_PWM_CH0_PAD, GPIO_PWM_CH1_PAD, GPIO_PWM_CH2_PAD, GPIO_PWM_CH3_PAD
};
static u32 GPIO_PWM_MODULE[5] =
{
	GPIO_PWM_CH0, GPIO_PWM_CH1, GPIO_PWM_CH2, GPIO_PWM_CH3
};
static u32 CLKIDX_PWM[5] =
{
	CLKIDX_PWM_CH0, CLKIDX_PWM_CH1, CLKIDX_PWM_CH2, CLKIDX_PWM_CH2
};

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return TRUE	indicates that Initialize is successed.
 *			FALSE indicates that Initialize is failed.
 *	@see	NX_PWM_GetNumberOfModule
 */
boolean NX_PWM_Initialize( void )
{
	static boolean bInit = FALSE;

	if( FALSE == bInit )
	{
		__g_pRegister_pwm[0] = (struct NX_PWM_RegisterSet *)NULL;

		bInit = TRUE;
	}

	return bInit;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@return		Module's physical address
 */
u32 NX_PWM_GetPhysicalAddress( u32 ModuleIndex )
{
	static const u32 PhysicalAddr[] = { PHY_BASEADDR_LIST( PWM_ ) };

	return (u32)PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 */
void NX_PWM_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{

	__g_pRegister_pwm[ModuleIndex] = (struct NX_PWM_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@return		Module's base address.
 */
u32		NX_PWM_GetBaseAddress( u32 ModuleIndex )
{
	return (u32)__g_pRegister_pwm[ModuleIndex];
}

void PWM_Util_Init( u32 Channel, u32 ClkGen_Src, u32 ClkGen_Div )
{
	u32 ModuleIndex = PWM;

	DEBUG( PWM_ON, "\r\n External IP to Initialize \r\n");
	DEBUG( PWM_ON, "======================================\r\n");
	/* GPIOx PAD Altnate Function */
	GPIO_SetALTFunction( GPIO_PWM_MODULE[Channel], GPIO_PWM_PAD[Channel], GPIO_PWM_ALT[Channel], GPIO_DRVSTRENGTH_0 );
	/* Clock Generation */
	ClkGen_SetConfig( CLKIDX_PWM[Channel], 0, ClkGen_Src, ClkGen_Div, TRUE );
	/* Reset Generation (1:No Reset) */
	RstGen_SetIP( (u8*)"PWM", Channel, RSTIDX_PWM[Channel], TRUE );

	/* PWM (Prototype Fuction) Initialize */
	DEBUG( PWM_ON, "Pulse Width Modulation.\r\n");
	DEBUG( PWM_ON, "--------------------------------------\r\n");
	NX_PWM_Initialize();
	NX_PWM_SetBaseAddress( ModuleIndex, NX_PWM_GetPhysicalAddress( ModuleIndex ) );
	DEBUG( PWM_ON, " PWM%d - Base Address: %8X \r\n", Channel, NX_PWM_GetBaseAddress( ModuleIndex ) );

	DEBUG( PWM_ON, "======================================\r\n");
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PWM Stop
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@return		None.
 */
void NX_PWM_Stop(u32 Channel)
{
	register struct NX_PWM_RegisterSet *pRegister;
	register u32 modulechannel, regvalue;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister = __g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCON);

	if(modulechannel == 0)
	{
		regvalue &= ~(1UL<<0);
	} else {
		regvalue &= ~(1<<(REG_OFFSET * (modulechannel+1)));
	}

	write32(&pRegister->TCON, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@param[in]	IntNum			Interrupt Number.
 *	@param[in]	Enable	TRUE	indicates that Interrupt Enable.
 *						FALSE	indicates that Interrupt Disable.
 *	@return		None.
 */
void NX_PWM_SetInterruptEnable( u32 ModuleIndex, u32 IntNum, boolean Enable )
{
	register struct NX_PWM_RegisterSet* pRegister;
	register u32 ReadValue;

	pRegister = __g_pRegister_pwm[ModuleIndex];

	ReadValue = read32(&pRegister->TINT_CSTAT) & 0x1F;

	ReadValue &= (u32)(~(1UL << IntNum));
	ReadValue |= (u32)Enable << IntNum ;

	write32(&pRegister->TINT_CSTAT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Dead Zone run
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	Enable		TRUE: enable, FALSE: disable.
 *	@return		none.
 */
boolean NX_PWM_SetDeadZoneEnable(u32 Channel, boolean Enable)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 regvalue;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	if(Channel != 0)
		return FALSE;

	regvalue = read32(&pRegister->TCON);
	if(Enable)
		regvalue |= (1UL<<DEADZONE);
	else
		regvalue &= ~(1UL<<DEADZONE);

	write32(&pRegister->TCON, regvalue);

	return TRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Dead Zone width
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	Length		clock unit width.
 *	@return		none.
 */
void NX_PWM_SetDeadZoneLength(u32 Channel, u32 Length)
{
	register struct NX_PWM_RegisterSet *pRegister;
	register u32 regvalue;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCFG0);
	regvalue &= ~(0xFF<<16);
	regvalue |= (Length & 0xFF)<<16;

	write32(&pRegister->TCFG0, regvalue);
}

//--------------------------------------------------------------------------
// @name	Configuration operations
//--------------------------------------------------------------------------
//@{
//------------------------------------------------------------------------------
/**
 *	@brief		Set prescaler ( register value 0 is bypass, 1 is divide by 2)
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	value		divide value (1 ~ 0x100).
 *	@return		none.
 */
void	NX_PWM_SetPrescaler(u32 Channel, u32 value)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 regvalue;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCFG0);

	if(Channel & 0x6)
		regvalue |= ((value-1) & 0xFF)<<8;		// timer channel 2, 3, 4
	else
		regvalue |= ((value-1) & 0xFF)<<0;		// timer channel 0, 1

	write32(&pRegister->TCFG0, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set divider path
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	divider		.
 *	@return		TRUE: this channel can select which selecting.
 *				FALSE: channel cannot select selecting path divider channel.
 */
boolean	NX_PWM_SetDividerPath(u32 Channel, NX_PWM_DIVIDSELECT divider)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel, regvalue, updatevalue;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	if(modulechannel == 4 && NX_PWM_DIVIDSELECT_TCLK == divider)
		return FALSE;

	updatevalue = divider<<(4*modulechannel);

	regvalue = read32(&pRegister->TCFG1);
	regvalue &= ~(0xF<<modulechannel);
	regvalue |= updatevalue;
	write32(&pRegister->TCFG1, regvalue);

	return TRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set LoadMode is AutoReload or Manual
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	ShotMode	LoadMode (0:Manual, 1:Auto Updata)
 *	@return		None.
 */
void NX_PWM_SetShotMode(u32 Channel, NX_PWM_LOADMODE ShotMode)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel, regvalue;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister = __g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCON);

	if(modulechannel == 0)
	{
		regvalue &= ~(1UL<<AUTO_RELOAD);
		regvalue |= ShotMode<<AUTO_RELOAD;
	} else {
		regvalue &= ~(1UL<<(REG_OFFSET*(modulechannel+1)+AUTO_RELOAD));
		regvalue |= ShotMode<<(REG_OFFSET*(modulechannel+1)+AUTO_RELOAD);
	}
	write32(&pRegister->TCON, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PWM Period (Count).
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	Period		PWM Period ( range : 0 ~ 65535 )
 *	@return		None.
 */
void	NX_PWM_SetPeriod(u32 Channel, u32 Period)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel;

	modulechannel = Channel % NX_PWM_CHANNEL;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	if(modulechannel == 0)
	{
		write32(&pRegister->TCNTB0, Period);
	}
	else if(modulechannel == 1)
	{
		write32(&pRegister->TCNTB1, Period);
	}
	else if(modulechannel == 2)
	{
		write32(&pRegister->TCNTB2, Period);
	}
	else if(modulechannel == 3)
	{
		write32(&pRegister->TCNTB3, Period);
	}	
	else if(modulechannel == 4)
	{
		write32(&pRegister->TCNTB4, Period);
	}

}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PWM Duty (Compare).
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@param[in]	Duty		PWM Duty ( range : 0 ~ 65535 )
 *	@return		None.
 */
boolean NX_PWM_SetDuty(u32 Channel, u32 Duty)
{
	register struct NX_PWM_RegisterSet *pRegister;
	register u32 modulechannel;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	if(modulechannel == 0)
	{
		write32(&pRegister->TCMPB0, Duty);;
	}
	else if(modulechannel == 1)
	{
		write32(&pRegister->TCMPB1, Duty);
	}
	else if(modulechannel == 2)
	{
		write32(&pRegister->TCMPB2, Duty);
	}
	else if(modulechannel == 3)
	{
		write32(&pRegister->TCMPB3, Duty);
	}	
	else if(modulechannel == 4)
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set TCNTx, TCMPx Manual Update
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@return		None.
 */
void NX_PWM_UpdateCounter(u32 Channel)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel, regvalue, updatedonevalue;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister = __g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCON);
	updatedonevalue = regvalue;
	if(modulechannel == 0)
	{
		regvalue |= 1UL<<1;
		updatedonevalue &= ~(1UL<<1);
	} else {
        //1，2，3，4的Manual Update位置是8+1，12+1，16+1，20+1
		regvalue |= 1<<(REG_OFFSET*(modulechannel+1)+UPDATE);
		updatedonevalue &= ~(1<<(REG_OFFSET*(modulechannel+1)+UPDATE));
	}

	write32(&pRegister->TCON, regvalue);
	write32(&pRegister->TCON, updatedonevalue);
}

static void PWM_Delay_ms(u32 Count)
{
	volatile u32 i = 0, j = 0;
	//while(Cnt--);
	for(i = 0; i < Count * 1000; i++)
		for(j = 0; j < 100; j++);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PWM Start(Run)
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@return		None.
 */
void NX_PWM_Run(u32 Channel)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel, regvalue;
    const u32 BIT_OFFSET = 4;

	modulechannel = Channel % NX_PWM_CHANNEL;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCON);
    //开始输出PWM信号
	if(modulechannel == 0)
	{
		regvalue |= 1UL << 0;
	} else 
    {
        //1，2，3，4的start/stop bit位置是8，12，16，20
		regvalue |= 1 << (BIT_OFFSET * ( modulechannel + 1 ));
	}

	write32(&pRegister->TCON, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get PWM Running state.
 *	@param[in]	Channel		An index of timer channel ( 0 ~ x ).
 *	@return		None.
 */
boolean	NX_PWM_IsRun(u32 Channel)
{
	register struct NX_PWM_RegisterSet	*pRegister;
	register u32 modulechannel, regvalue;

	modulechannel = Channel%NX_PWM_CHANNEL;

	pRegister	=	__g_pRegister_pwm[Channel/NX_PWM_CHANNEL];

	regvalue = read32(&pRegister->TCON);

	if(modulechannel == 0)
	{
		regvalue >>= 0;
	}
	else
	{
		regvalue >>= (REG_OFFSET*(modulechannel+1));
	}
	regvalue &= 0x1;

	return (boolean)regvalue;
}

void PWM_IO_Init( PWM_CMD* CmdSet )
{
	u32 ModuleIndex = PWM;
	u32 Channel = CmdSet->Channel;
	const u32 CLK_GEN_DIV = 64;

	PWM_Util_Init( Channel, PLL2, CLK_GEN_DIV );//TCLK (Clock Generation )
	NX_PWM_Stop( Channel );
	NX_PWM_SetInterruptEnable( ModuleIndex, Channel, FALSE );
	NX_PWM_SetDeadZoneEnable( Channel, FALSE );
	NX_PWM_SetDeadZoneLength( Channel, 0 );
	NX_PWM_SetPrescaler( Channel, CmdSet->Prescaler );
	NX_PWM_SetDividerPath( Channel, CmdSet->Divider );
	NX_PWM_SetShotMode( Channel, NX_PWM_LOADMODE_AUTORELOAD );
	NX_PWM_SetPeriod( Channel, CmdSet->Count );
	NX_PWM_SetDuty( Channel, CmdSet->Compare );
	NX_PWM_SetInterruptEnable( ModuleIndex, Channel, TRUE );
	
	NX_PWM_UpdateCounter( Channel );
	NX_PWM_Stop( Channel );
	PWM_Delay_ms ( 5 );
	NX_PWM_SetShotMode( Channel, NX_PWM_LOADMODE_AUTORELOAD );
	NX_PWM_Run( Channel );
}
