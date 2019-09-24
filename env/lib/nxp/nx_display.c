#include <errno.h>
#include <nxp/nx_display.h>
#include <nxp/nx_chip.h>
#include <nxp/nx_gpio.h>
#include <s5p4418_serial_stdio.h>
#include <io.h>

static NX_LVDS_RegisterSet *__g_pRegister_lvds[NUMBER_OF_LVDS_MODULE];

//------------------------------------------------------------------------------
//
//	DISPLAYTOP Interface
//
//------------------------------------------------------------------------------
static struct
{
	struct NX_DISPLAYTOP_RegisterSet *pRegister;
} __g_ModuleVariables = { NULL, };

//--------------
// Register Map ��ü�� CLKGEN�� ������ �� ����ϹǷ�. �״�� ������.
//--------------
static struct
{
	struct NX_DISPTOP_CLKGEN_RegisterSet *__g_pRegister;

} __g_ModuleVariablesClk[NUMBER_OF_DISPTOP_CLKGEN_MODULE] = { {NULL, }, };

static struct
{
	struct NX_DPC_RegisterSet *pRegister;

} __g_ModuleVariablesDpc[NUMBER_OF_DPC_MODULE] = { {NULL,}, };

static struct
{
	struct NX_MLC_RegisterSet *pRegister;

} __g_ModuleVariablesMlc[NUMBER_OF_MLC_MODULE] = { {NULL,}, };

struct NX_RSTCON_RegisterSet
{
	volatile u32	REGRST[__def_RSTCON__RSTREGISTERCNT];
};

//------------------------------------------------------------------------------
//	Module Interface
//------------------------------------------------------------------------------

/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b TRUE	indicate that Initialize is successed.\n
 *			\b FALSE	indicate that Initialize is failed.
 *	@see	NX_DISPLAYTOP_GetNumberOfModule
 */
boolean NX_DISPLAYTOP_Initialize( void )
{
	static boolean bInit = FALSE;
	u32 i;

	if( FALSE == bInit )
	{
		for(i=0; i<NUMBER_OF_DISPLAYTOP_MODULE; i++)
		{
			__g_ModuleVariables.pRegister = NULL;
		}
		bInit = TRUE;
	}
	return TRUE;
}

boolean NX_DISPTOP_CLKGEN_Initialize( void )
{
	static boolean bInit = FALSE;
	u32 i;

	if( FALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_DISPTOP_CLKGEN_MODULE; i++ )
		{
			__g_ModuleVariablesClk[i].__g_pRegister = NULL;
		}
		bInit = TRUE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b TRUE	indicates that	Initialize is successed.\n
 *			\b FALSE	Initialize is failed.\n
 *	@see								NX_DPC_GetNumberOfModule
 */
boolean NX_DPC_Initialize( void )
{
	static boolean bInit = FALSE;
	u32 i;

	if( FALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_DPC_MODULE; i++ )
		{
			__g_ModuleVariablesDpc[i].pRegister = NULL;
		}
		bInit = TRUE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b TRUE	indicates that Initialize is successed.\n
 *			\b FALSE	indicates that Initialize is failed.\n
 *	@see	NX_MLC_GetNumberOfModule
 */
boolean NX_MLC_Initialize( void )
{
	static boolean bInit = FALSE;
	u32 i;

	if( FALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_MLC_MODULE; i++ )
		{
			__g_ModuleVariablesMlc[i].pRegister = NULL;
		}
		bInit = TRUE;
	}
	return TRUE;
}

void NX_DISPLAYTOP_SetBaseAddress( u32 BaseAddress )
{
	__g_ModuleVariables.pRegister = (struct NX_DISPLAYTOP_RegisterSet *)BaseAddress;

}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see										NX_DISPLAYTOP_GetSizeOfRegisterSet,
 *				NX_DISPLAYTOP_SetBaseAddress,			NX_DISPLAYTOP_GetBaseAddress,
 *				NX_DISPLAYTOP_OpenModule,				NX_DISPLAYTOP_CloseModule,
 *				NX_DISPLAYTOP_CheckBusy,				NX_DISPLAYTOP_CanPowerDown
 */
u32 NX_DISPLAYTOP_GetPhysicalAddress( void )
{
	static const u32 PhysicalAddr[] = { PHY_BASEADDR_DISPLAYTOP_MODULE }; // PHY_BASEADDR_UART?_MODULE
	return (u32)(PhysicalAddr[0] + PHY_BASEADDR_DISPLAYTOP_MODULE_OFFSET);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b TRUE	indicate that Initialize is successed. \n
 *				\b FALSE	indicate that Initialize is failed.
 *	@see		NX_DISPLAYTOP_GetPhysicalAddress,		NX_DISPLAYTOP_GetSizeOfRegisterSet,
 *				NX_DISPLAYTOP_SetBaseAddress,			NX_DISPLAYTOP_GetBaseAddress,
 *												NX_DISPLAYTOP_CloseModule,
 *				NX_DISPLAYTOP_CheckBusy,				NX_DISPLAYTOP_CanPowerDown
 */
boolean NX_DISPLAYTOP_OpenModule( void )
{
	return TRUE;
}

//---------- RSTCON �� ���� prototype
u32 NX_DISPLAYTOP_GetResetNumber ( void )
{
	const u32 ResetPinNumber[NUMBER_OF_DISPLAYTOP_MODULE] =
	{
		RESETINDEX_OF_DISPLAYTOP_MODULE_i_Top_nRST,
	};

	return (u32)ResetPinNumber[0];
}
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's reset index.
 *	@return		Module's reset index.\n
 *				It is equal to RESETINDEX_OF_DUALDISPLAY?_MODULE_i_nRST in <nx_chip.h>.
 *	@see		NX_RSTCON_Enter,
 *				NX_RSTCON_Leave,
 *				NX_RSTCON_GetStatus
 */
u32 NX_DUALDISPLAY_GetResetNumber ( u32 ModuleIndex )
{
	const u32 ResetNumber[] =
	{
		RESETINDEX_OF_DUALDISPLAY_MODULE_i_nRST
	};

	return	ResetNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 */
void NX_MLC_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
	__g_ModuleVariablesMlc[ModuleIndex].pRegister = (struct NX_MLC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		Module's physical address
 */
u32 NX_MLC_GetPhysicalAddress( u32 ModuleIndex )
{
	const u32 PhysicalAddr[] =
	{
		PHY_BASEADDR_MLC0_MODULE, PHY_BASEADDR_MLC1_MODULE
	};

	return	PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see		NX_MLC_GetClockPClkMode,
 *				NX_MLC_SetClockBClkMode,		NX_MLC_GetClockBClkMode,
 */
void NX_MLC_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode )
{
	const u32 PCLKMODE_POS	=	3;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	u32 clkmode=0;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: break;
	}
	regvalue = pRegister->MLCCLKENB;
	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

	write32(&pRegister->MLCCLKENB,regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set System Bus Clock's operation Mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	mode		BCLK Mode
 *	@return		None.
 *	@see		NX_MLC_SetClockPClkMode,		NX_MLC_GetClockPClkMode,
 *				NX_MLC_GetClockBClkMode,
 */
void NX_MLC_SetClockBClkMode( u32 ModuleIndex, NX_BCLKMODE mode )
{
	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;
	u32 clkmode=0;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;
	switch(mode)
	{
		case NX_BCLKMODE_DISABLE:	clkmode = 0;		break;
		case NX_BCLKMODE_DYNAMIC:	clkmode = 2;		break;
		case NX_BCLKMODE_ALWAYS:	clkmode = 3;		break;
		default: break;
	}

	regvalue = pRegister->MLCCLKENB;
	regvalue &= ~(0x3);
	regvalue |= clkmode & 0x3;

	write32(&pRegister->MLCCLKENB, regvalue);
}

/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 */
void NX_DPC_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
	__g_ModuleVariablesDpc[ModuleIndex].pRegister = (struct NX_DPC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@return		Module's physical address
 */
u32 NX_DPC_GetPhysicalAddress( u32 ModuleIndex )
{
	const u32 PhysicalAddr[] =
	{
		PHY_BASEADDR_DPC0_MODULE, PHY_BASEADDR_DPC1_MODULE
	};

	return	PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_DPC_GetClockPClkMode,
 *				NX_DPC_SetClockSource,			NX_DPC_GetClockSource,
 *				NX_DPC_SetClockDivisor,			NX_DPC_GetClockDivisor,
 *				NX_DPC_SetClockOutInv,			NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *				NX_DPC_SetClockOutDelay,		NX_DPC_GetClockOutDelay,
 *				NX_DPC_SetClockDivisorEnable,	NX_DPC_GetClockDivisorEnable
 */
void NX_DPC_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode )
{
	const u32 PCLKMODE_POS = 3;

	register u32 regvalue;
	register struct NX_DPC_RegisterSet* pRegister;
	u32 clkmode=0;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: break;
	}

	regvalue = pRegister->DPCCLKENB;
	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

	write32(&pRegister->DPCCLKENB, regvalue);
}

void NX_DISPTOP_CLKGEN_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
	__g_ModuleVariablesClk[ModuleIndex].__g_pRegister = (struct NX_DISPTOP_CLKGEN_RegisterSet *)BaseAddress;
}

u32 NX_DISPTOP_CLKGEN_GetPhysicalAddress( u32 ModuleIndex )
{
	static const u32 PhysicalAddr[] = { PHY_BASEADDR_DISPTOP_CLKGEN0_MODULE, PHY_BASEADDR_DISPTOP_CLKGEN1_MODULE,
										PHY_BASEADDR_DISPTOP_CLKGEN2_MODULE, PHY_BASEADDR_DISPTOP_CLKGEN3_MODULE,
										PHY_BASEADDR_DISPTOP_CLKGEN4_MODULE}; // PHY_BASEADDR_DISPTOP_CLKGEN_MODULE
	return (u32)PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b TRUE	indicate that Initialize is successed.\n
 *			\b FALSE	indicate that Initialize is failed.
 
 *	@see	NX_LVDS_GetNumberOfModule
 */
boolean NX_LVDS_Initialize( void )
{
	static boolean bInit = FALSE;
	u32 i;
	if( FALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_LVDS_MODULE; i++ )
		{
			__g_pRegister_lvds[i] = NULL;
		}
		bInit = TRUE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number. \n
 *				It is equal to NUMBER_OF_LVDS_MODULE in <nx_chip.h>.
 *	@see		NX_LVDS_Initialize
 */
u32 NX_LVDS_GetNumberOfModule( void )
{
	return NUMBER_OF_LVDS_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address. \n
 *				It is equal to PHY_BASEADDR_LVDS?_MODULE in <nx_chip.h>.
 *	@see		NX_LVDS_GetSizeOfRegisterSet,
 *				NX_LVDS_SetBaseAddress,			NX_LVDS_GetBaseAddress,
 *				NX_LVDS_OpenModule,				NX_LVDS_CloseModule,
 *				NX_LVDS_CheckBusy,
 */
u32 NX_LVDS_GetPhysicalAddress( u32 ModuleIndex )
{
	const u32 PhysicalAddr[NUMBER_OF_LVDS_MODULE] =
	{
		PHY_BASEADDR_LVDS_MODULE
	};
	return PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_LVDS_GetPhysicalAddress,		NX_LVDS_GetSizeOfRegisterSet,
 *				NX_LVDS_GetBaseAddress,
 *				NX_LVDS_OpenModule,				NX_LVDS_CloseModule,
 *				NX_LVDS_CheckBusy,
 */
void NX_LVDS_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress )
{
	__g_pRegister_lvds[ModuleIndex] = (NX_LVDS_RegisterSet *)BaseAddress;
}


void NX_DISPTOP_CLKGEN_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode )
{
	const u32 PCLKMODE_POS = 3;

	register u32 regvalue;
	register struct NX_DISPTOP_CLKGEN_RegisterSet* __g_pRegister;

	u32 clkmode=0;

	__g_pRegister = __g_ModuleVariablesClk[ModuleIndex].__g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: break;
	}

	regvalue = __g_pRegister->CLKENB;
	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

	write32(&__g_pRegister->CLKENB, regvalue);
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set the priority of layers.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	priority		the priority of layers
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *											NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetLayerPriority( u32 ModuleIndex, NX_MLC_PRIORITY priority )
{
	const u32 PRIORITY_POS	= 8;
	const u32 PRIORITY_MASK	= 0x03 << PRIORITY_POS;

	const u32 DIRTYFLAG_POS		= 3;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32 regvalue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( PRIORITY_MASK | DIRTYFLAG_MASK );
	regvalue |= (priority << PRIORITY_POS);

	write32(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the background color.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	color	24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }
 *	@return		None.
 *	@remark		The background color is default color that is shown in regions which
 *				any layer does not include. the result of this function will be
 *				applied to MLC after calling function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize
 */
void NX_MLC_SetBackground( u32 ModuleIndex, u32 color )
{
	register struct NX_MLC_RegisterSet* pRegister;
	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	write32(&pRegister->MLCBGCOLOR, color);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable interlace mode.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnb			Set it to TRUE to enable interlace mode.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *											NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetFieldEnable( u32 ModuleIndex, boolean bEnb )
{
	const u32 FIELDENB_POS = 0;
	const u32 FIELDENB_MASK = 1UL << FIELDENB_POS;
	const u32 DIRTYFLAG_POS = 3;
	const u32 DIRTYFLAG_MASK = 1UL << DIRTYFLAG_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;
	regvalue = pRegister->MLCCONTROLT;
	regvalue &= ~( FIELDENB_MASK | DIRTYFLAG_MASK );
	regvalue |= (bEnb<<FIELDENB_POS);

	write32(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set power of RGB layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bRed	\b TRUE indicates that Red gamma table is power on. \n
 *						\b FALSE indicates that Red gamma table is power off.
 *	@param[in]	bGreen	\b TRUE indicates that Green gamma table is power on. \n
 *						\b FALSE indicates that Green gamma table is power off.
 *	@param[in]	bBlue	\b TRUE indicates that Blue gamma table is power on. \n
 *						\b FALSE indicates that Blue gamma table is power off.
 *	@return		None.
 *	@remarks	Gamma table must on before MLC gamma enable.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerGamaTablePowerMode( u32 ModuleIndex, boolean bRed, boolean bGreen, boolean bBlue )
{
	const u32 BGAMMATABLE_PWD_BITPOS = 11;
	const u32 GGAMMATABLE_PWD_BITPOS = 9;
	const u32 RGAMMATABLE_PWD_BITPOS = 3;
	const u32 BGAMMATABLE_PWD_MASK = ( 1 << BGAMMATABLE_PWD_BITPOS );
	const u32 GGAMMATABLE_PWD_MASK = ( 1 << GGAMMATABLE_PWD_BITPOS );
	const u32 RGAMMATABLE_PWD_MASK = ( 1 << RGAMMATABLE_PWD_BITPOS );

	register u32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;
	ReadValue &= ~( BGAMMATABLE_PWD_MASK | GGAMMATABLE_PWD_MASK | RGAMMATABLE_PWD_MASK );
	ReadValue |= ( ((u32)bRed << RGAMMATABLE_PWD_BITPOS) |
					((u32)bGreen << GGAMMATABLE_PWD_BITPOS) |
					((u32)bBlue << BGAMMATABLE_PWD_BITPOS) );

	write32(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set sleep mode of RGB layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bRed	\b TRUE indicates that Red gamma table is sleep on. \n
 *						\b FALSE indicates that Red gamma table is sleep off.
 *	@param[in]	bGreen	\b TRUE indicates that Green gamma table is sleep on. \n
 *						\b FALSE indicates that Green gamma table is sleep off.
 *	@param[in]	bBlue	\b TRUE indicates that Blue gamma table is sleep on. \n
 *						\b FALSE indicates that Blue gamma table is sleep off.
 *	@return		None.
 *	@remarks	Sleep mode is only usable when gamma table is power ON.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerGamaTableSleepMode( u32 ModuleIndex, boolean bRed, boolean bGreen, boolean bBlue )
{
	const u32 BGAMMATABLE_SLD_BITPOS = 10;
	const u32 GGAMMATABLE_SLD_BITPOS = 8;
	const u32 RGAMMATABLE_SLD_BITPOS = 2;
	const u32 BGAMMATABLE_SLD_MASK = ( 1 << BGAMMATABLE_SLD_BITPOS );
	const u32 GGAMMATABLE_SLD_MASK = ( 1 << GGAMMATABLE_SLD_BITPOS );
	const u32 RGAMMATABLE_SLD_MASK = ( 1 << RGAMMATABLE_SLD_BITPOS );

	register u32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	if( bRed	)	{ ReadValue &= ~RGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	RGAMMATABLE_SLD_MASK; }

	if( bGreen)	{ ReadValue &= ~GGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	GGAMMATABLE_SLD_MASK; }

	if( bBlue )	{ ReadValue &= ~BGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	BGAMMATABLE_SLD_MASK; }

	write32(&pRegister->MLCGAMMACONT, ReadValue);
}

/**
 *	@brief		Set RGB layer gamma enable or Not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable			\b TRUE indicates that RGB layer's gamma enable. \n
 *								\b FALSE indicates that RGB layer's gamma disable.
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *															NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerGammaEnable( u32 ModuleIndex, boolean bEnable )
{
	const u32 RGBGAMMAEMB_BITPOS	= 1;
	const u32 RGBGAMMAEMB_MASK		= 1 << RGBGAMMAEMB_BITPOS;

	register u32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;
	ReadValue &= ~RGBGAMMAEMB_MASK;
	ReadValue |= (u32)bEnable << RGBGAMMAEMB_BITPOS;

	write32(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set dither enable when using gamma.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable			\b TRUE indicates that dither enable. \n
 *								\b FALSE indicates that dither disable.
 *	@return		None.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetDitherEnableWhenUsingGamma( u32 ModuleIndex, boolean bEnable )
{
	const u32 DITHERENB_BITPOS	= 0;
	const u32 DITHERENB_MASK	= 1 << DITHERENB_BITPOS;

	register struct NX_MLC_RegisterSet*	pRegister;
	register u32 ReadValue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;
	ReadValue &= ~DITHERENB_MASK;
	ReadValue |= ( (u32)bEnable << DITHERENB_BITPOS );

	write32(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Select layer to apply gamma when video layer and RGB layer are overlaped region for alpha blending.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bVideoLayer		\b TRUE indicates that Video layer. \n
 *								\b FALSE indicates that RGB layer.
 *	@return		none.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetGammaPriority( u32 ModuleIndex, boolean bVideoLayer )
{
	const u32 ALPHASELECT_BITPOS	= 5;
	const u32 ALPHASELECT_MASK		= 1 << ALPHASELECT_BITPOS;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32 ReadValue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;
	ReadValue = pRegister->MLCGAMMACONT;
	ReadValue &= ~ALPHASELECT_MASK;
	ReadValue |= ( (u32)bVideoLayer << ALPHASELECT_BITPOS );
	write32(&pRegister->MLCGAMMACONT, ReadValue);
}

//--------------------------------------------------------------------------
// MLC Main Settings
//--------------------------------------------------------------------------
/**
 *	@brief		Set Power On/Off of MLC's Pixel Buffer Unit
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param		bPower	\b TRUE indicates that Power ON of pixel buffer unit. \n
 *						\b FALSE indicates that Power OFF of pixel buffer unit. \n
 *	@return		None.
 *	@remark		When MLC ON, first pixel buffer power ON, set to Normal Mode(pixel buffer) and MLC enable.\n
 *				When MLC Off, first MLC disable, Set pixel buffer to Sleep Mode(pixel buffer) and power OFF.\n
 *	@code
 *				// MLC ON sequence, mi = 0 ( module index )
 *				NX_MLC_SetTopPowerMode( mi, TRUE );			// pixel buffer power on
 *				NX_MLC_SetTopSleepMode( mi, FALSE );			// pixel buffer normal mode
 *				NX_MLC_SetMLCEnable( mi, TRUE );				// mlc enable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				...
 *				// MLC OFF sequence
 *				NX_MLC_SetMLCEnable( mi, FALSE );				// mlc disable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				while( TRUE == NX_MLC_GetTopDirtyFlag(mi, ))	// wait until mlc is disabled
 *				{ NULL; }
 *				NX_MLC_SetTopSleepMode( mi, TRUE );			// pixel buffer sleep mode
 *				NX_MLC_SetTopPowerMode( mi, FALSE );			// pixel buffer power off
 *	@endcode
 *	@see									NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetTopPowerMode( u32 ModuleIndex, boolean bPower )
{
	const u32 PIXELBUFFER_PWD_POS	= 11;
	const u32 PIXELBUFFER_PWD_MASK	= 1UL << PIXELBUFFER_PWD_POS;
	const u32 DITTYFLAG_MASK		= 1UL << 3;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32 regvalue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;
	regvalue &= ~( PIXELBUFFER_PWD_MASK | DITTYFLAG_MASK );
	regvalue |= (bPower << PIXELBUFFER_PWD_POS);

	write32(&pRegister->MLCCONTROLT, regvalue);
}

/**
 *	@brief		Set Sleep Mode Enable/Disalbe of MLC's Pixel Buffer Unit
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param		bSleep	\b TRUE indicates that Sleep Mode Enable of pixel buffer unit. \n
 *						\b FALSE indicates that Sleep Mode Disable of pixel buffer unit. \n
 *	@return		None.
 *	@remark		When MLC ON, first pixel buffer power ON, set to Normal Mode(pixel buffer) and MLC enable.\n
 *				When MLC Off, first MLC disable, Set pixel buffer to Sleep Mode(pixel buffer) and power OFF.\n
 *	@code
 *				// MLC ON sequence,	mi = 0  ( module index )
 *				NX_MLC_SetTopPowerMode( mi, TRUE );			// pixel buffer power on
 *				NX_MLC_SetTopSleepMode( mi, FALSE );			// pixel buffer normal mode
 *				NX_MLC_SetMLCEnable( mi, TRUE );				// mlc enable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				...
 *				// MLC OFF sequence
 *				NX_MLC_SetMLCEnable( mi, FALSE );				// mlc disable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				while( TRUE == NX_MLC_GetTopDirtyFlag( mi ))	// wait until mlc is disabled
 *				{ NULL; }
 *				NX_MLC_SetTopSleepMode( mi, TRUE );			// pixel buffer sleep mode
 *				NX_MLC_SetTopPowerMode( mi, FALSE );			// pixel buffer power off
 *	@endcode
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *											NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetTopSleepMode( u32 ModuleIndex, boolean bSleep )
{
	const u32 PIXELBUFFER_SLD_POS	= 10;
	const u32 PIXELBUFFER_SLD_MASK	= 1UL << PIXELBUFFER_SLD_POS;
	const u32 DITTYFLAG_MASK		= 1UL << 3;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32 regvalue;

	bSleep = (boolean)((u32)bSleep ^ 1);

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;
	regvalue &= ~( PIXELBUFFER_SLD_MASK | DITTYFLAG_MASK );
	regvalue |= (bSleep << PIXELBUFFER_SLD_POS);

	write32(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the lock size for memory access.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer			the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	locksize		lock size for memory access, 4, 8, 16 are only valid.
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.\n
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetLockSize( u32 ModuleIndex, u32 layer, u32 locksize )
{
	const u32 LOCKSIZE_MASK		= 3UL<<12;

	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32 regvalue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	locksize >>= 3;	// divide by 8

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue &= ~(LOCKSIZE_MASK|DIRTYFLAG_MASK);
		regvalue |= (locksize<<12);

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Alpha blending.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	bEnb	Set it to TRUE to enable alpha blending.
 *	@param[in]	alpha	alpha blending factor, 0 ~ 15.
 *				- When it is set to 0, this layer color is fully transparent.
 *				- When it is set to 15, this layer becomes fully opaque.
 *	@return		None.
 *	@remark		The argument 'alpha' has only affect when the color format has
 *				no alpha component. The formula for alpha blending is as follows.
 *				- If alpha is 0 then a is 0, else a is ALPHA + 1.\n
 *					color = this layer color * a / 16 + lower layer color * (16 - a) / 16.
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.\n
 *				Only one layer must apply to alpha function.
 *
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *														NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetAlphaBlending( u32 ModuleIndex, u32 layer, boolean bEnb, u32 alpha )
{
	const u32 BLENDENB_POS	= 2;
	const u32 BLENDENB_MASK	= 0x01 << BLENDENB_POS;

	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const u32 ALPHA_POS		= 28;
	const u32 ALPHA_MASK	= 0xF << ALPHA_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
			regvalue &= ~( BLENDENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<BLENDENB_POS);

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCTPCOLOR;
		regvalue &= ~ALPHA_MASK;
		regvalue |= alpha << ALPHA_POS;

		write32(&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;
		regvalue &= ~( BLENDENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<BLENDENB_POS);
//		printf(0, "regvalue = %d\r\n", regvalue);
//		printf(0, "&pRegister->MLCRGBLAYER[layer].MLCCONTROL=%x\r\n",&pRegister->MLCRGBLAYER[layer].MLCCONTROL);

	//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
		write32(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);

//		printf(0, "alpha << ALPHA_POS = %d\r\n", (alpha << ALPHA_POS));
//		printf(0, "&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR=%x\r\n",&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR);
	//	pRegister->MLCVIDEOLAYER.MLCTPCOLOR = alpha << ALPHA_POS;
		write32(&pRegister->MLCVIDEOLAYER.MLCTPCOLOR, alpha << ALPHA_POS);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set color inversion.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	bEnb	Set it to TRUE to enable color inversion.
 *	@param[in]	color	Specifies the extended color to be used for color inversion.\n
 *						24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }\n
 *						You can get this argument from specific color format
 *						by using the function NX_MLC_GetExtendedColor().
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *														NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetColorInversion( u32 ModuleIndex, u32 layer, boolean bEnb, u32 color )
{
	const u32 INVENB_POS	= 1;
	const u32 INVENB_MASK	= 0x01 << INVENB_POS;

	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const u32 INVCOLOR_POS	= 0;
	const u32 INVCOLOR_MASK	= ((1<<24)-1) << INVCOLOR_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue &= ~( INVENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<INVENB_POS);

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCINVCOLOR;
		regvalue &= ~INVCOLOR_MASK;
		regvalue |= (color & INVCOLOR_MASK);

		write32(&pRegister->MLCRGBLAYER[layer].MLCINVCOLOR, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set transparency.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	bEnb	Set it to TRUE to enable transparency.
 *	@param[in]	color	Specifies the extended color to be used as the transparency color.\n
 *						24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }\n
 *						You can get this argument from specific color format
 *						by using the function NX_MLC_GetExtendedColor().
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetTransparency( u32 ModuleIndex, u32 layer, boolean bEnb, u32 color )
{
	const u32 TPENB_POS		= 0;
	const u32 TPENB_MASK	= 0x01 << TPENB_POS;

	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const u32 TPCOLOR_POS	= 0;
	const u32 TPCOLOR_MASK	= ((1<<24)-1) << TPCOLOR_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
			regvalue &= ~( TPENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb << TPENB_POS);

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCTPCOLOR;
		regvalue &= ~TPCOLOR_MASK;
		regvalue |= (color & TPCOLOR_MASK);

		write32(&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR, regvalue);
	}
}

//------------------------------------------------------------------------------
//	RGB Layer Specific Operations
//------------------------------------------------------------------------------
/**
 *	@brief		Set the invalid area of RGB Layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	region	select region ( 0 or 1 )
 *	@param[in]	sx		the x-coordinate of the upper-left corner of the layer, 0 ~ +2047.
 *	@param[in]	sy		the y-coordinate of the upper-left corner of the layer, 0 ~ +2047.
 *	@param[in]	ex		the x-coordinate of the lower-right corner of the layer, 0 ~ +2047.
 *	@param[in]	ey		the y-coordinate of the lower-right corner of the layer, 0 ~ +2047.
 *	@param[in]	bEnb	\b TRUE indicates that invalid region Enable,\n
 *						\b FALSE indicates that invalid region Disable.
 *	@return		None.
 *	@remark		Each RGB Layer support two invalid region. so \e region argument must set to 0 or 1.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see													NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerInvalidPosition( u32 ModuleIndex, u32 layer, u32 region, s32 sx, s32 sy, s32 ex, s32 ey, boolean bEnb )
{
	const u32 INVALIDENB_POS = 28;

	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		if( 0 == region )
		{
			write32(&pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT0, ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) ));
			write32(&pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM0, ( ((sy&0x7FF)<<16) | (ey&0x7FF) ));
		}
		else
		{
			write32(&pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT1, ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) ));
			write32(&pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM1, ( ((sy&0x7FF)<<16) | (ey&0x7FF) ));
		}
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the image format of the RGB layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	format	the RGB format
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *														NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetFormatRGB( u32 ModuleIndex, u32 layer, NX_MLC_RGBFMT format )
{
	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;
	const u32 FORMAT_MASK		= 0xFFFF0000UL;
	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue &= ~(FORMAT_MASK | DIRTYFLAG_MASK);
		regvalue |= (u32)format;

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the layer position.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	sx		the x-coordinate of the upper-left corner of the layer, -2048 ~ +2047.
 *	@param[in]	sy		the y-coordinate of the upper-left corner of the layer, -2048 ~ +2047.
 *	@param[in]	ex		the x-coordinate of the lower-right corner of the layer, -2048 or 0 ~ +2047.
 *	@param[in]	ey		the y-coordinate of the lower-right corner of the layer, -2048 or 0 ~ +2047.
 *	@return		None.
 *	@remark		If layer is 2(video layer) then x, y-coordinate of the lower-right
 *				corner of the layer must be a positive value.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *														NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetPosition( u32 ModuleIndex, u32 layer, s32 sx, s32 sy, s32 ex, s32 ey )
{
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		write32(&pRegister->MLCRGBLAYER[layer].MLCLEFTRIGHT, (((u32)sx & 0xFFFUL)<<16) | ((u32)ex & 0xFFFUL));
		write32(&pRegister->MLCRGBLAYER[layer].MLCTOPBOTTOM,	(((u32)sy & 0xFFFUL)<<16) | ((u32)ey & 0xFFFUL));
	}
	else if( 2 == layer )
	{
		write32(&pRegister->MLCRGBLAYER2.MLCLEFTRIGHT, (((u32)sx & 0xFFFUL)<<16) | ((u32)ex & 0xFFFUL));
		write32(&pRegister->MLCRGBLAYER2.MLCTOPBOTTOM,	(((u32)sy & 0xFFFUL)<<16) | ((u32)ey & 0xFFFUL));
	}
	else if( 3 == layer )
	{
		write32(&pRegister->MLCVIDEOLAYER.MLCLEFTRIGHT, (((u32)sx & 0xFFFUL)<<16) | ((u32)ex & 0xFFFUL));
		write32(&pRegister->MLCVIDEOLAYER.MLCTOPBOTTOM, (((u32)sy & 0xFFFUL)<<16) | ((u32)ey & 0xFFFUL));
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set stride for horizontal and vertical.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer		the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	hstride	the horizontal stride specifying the number of
 *							bytes from one pixel to the next. Generally, this
 *							value has bytes per pixel. You have to set it only
 *							to a positive value.
 *	@param[in]	vstride		the vertical stride specifying the number of
 *							bytes from one scan line of the image buffer to
 *							the next. Generally, this value has bytes per a
 *							line. You can set it to a negative value for
 *							vertical flip.
 *	@return		None.
 *	@remarks	The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerStride( u32 ModuleIndex, u32 layer, s32 hstride, s32 vstride )
{
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		write32((volatile u32 *)&pRegister->MLCRGBLAYER[layer].MLCHSTRIDE, hstride);
		write32((volatile u32 *)&pRegister->MLCRGBLAYER[layer].MLCVSTRIDE, vstride);
	}
	else if( 2 == layer )
	{
		write32((volatile u32 *)&pRegister->MLCRGBLAYER2.MLCHSTRIDE, hstride);
		write32((volatile u32 *)&pRegister->MLCRGBLAYER2.MLCVSTRIDE, vstride);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set an address of the image buffer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number( 0: RGB0, 1: RGB1 ).
 *	@param[in]	addr	an address of the image buffer.
 *	@return		None.
 *	@remark		Normally, the argument 'addr' specifies an address of upper-left
 *				corner of the image. but you have to set it to an address of
 *				lower-left corner for vertical mirror.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *															NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void NX_MLC_SetRGBLayerAddress( u32 ModuleIndex, u32 layer, u32 addr )
{
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		write32(&pRegister->MLCRGBLAYER[layer].MLCADDRESS, addr);
	}
	else if( 2 == layer )
	{
		write32(&pRegister->MLCRGBLAYER2.MLCADDRESS, addr);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable MLC.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnb			Set it to TRUE to enable MLC.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *											NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetMLCEnable( u32 ModuleIndex, boolean bEnb )
{
	const u32 MLCENB_POS	= 1;
	const u32 MLCENB_MASK	= 1UL<<MLCENB_POS;
	const u32 DIRTYFLAG_POS		= 3;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;
	regvalue &= ~( MLCENB_MASK | DIRTYFLAG_MASK );
	regvalue |= (bEnb<<MLCENB_POS);

	write32(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Apply modified MLC Top registers to MLC.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		 None.
 *	@remark		MLC has dual register set architecture. Therefore you have to set a
 *				dirty flag to apply modified settings to MLC's current settings.
 *				If a dirty flag is set, MLC will update current settings to
 *				register values on a vertical blank. You can also check whether MLC
 *				has been updated by using function NX_MLC_GetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *											NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetTopDirtyFlag( u32 ModuleIndex )
{
	const u32 DIRTYFLAG = 1UL<<3;

	register struct NX_MLC_RegisterSet* pRegister;
	register u32	regvalue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	regvalue  =	pRegister->MLCCONTROLT;
	regvalue |= DIRTYFLAG;

	write32(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable the layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer			the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	bEnb			Set it to TRUE to enable corresponding layer
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *														NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetLayerEnable( u32 ModuleIndex, u32 layer, boolean bEnb )
{
	const u32 LAYERENB_POS	= 5;
	const u32 LAYERENB_MASK	= 0x01 << LAYERENB_POS;
	const u32 DIRTYFLAG_POS		= 4;
	const u32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register u32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue &= ~( LAYERENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<LAYERENB_POS);

		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;
		regvalue &= ~( LAYERENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<LAYERENB_POS);

		write32(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
	}
}

//--------------------------------------------------------------------------
//	Per Layer Operations
//------------------------------------------------------------------------------
/**
 *	@brief		Apply modified register values to corresponding layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@return		None.
 *	@remark		Each layer has dual register set architecture. Therefore you have to
 *				set a dirty flag to apply modified settings to each layer's current
 *				settings. If a dirty flag is set, each layer will update current
 *				settings to register values on a vertical blank. You can also check
 *				whether each layer has been updated by using function NX_MLC_GetDirtyFlag().
 *	@see												NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void NX_MLC_SetDirtyFlag( u32 ModuleIndex, u32 layer )
{
	register struct NX_MLC_RegisterSet* pRegister;
	register u32    regvalue;
	const u32 DIRTYFLG_MASK = 1UL << 4;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;

	if ( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue |= DIRTYFLG_MASK;
		write32(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue  = pRegister->MLCVIDEOLAYER.MLCCONTROL;
		regvalue |= DIRTYFLG_MASK;
		write32(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
	}
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	ClkSrc	Select clock source of clock generator.\n
 *						0:PLL0, 1:PLL1, 2:SVLCK, 3:P(S)VCLK, 4:~P(S)VCLK, 5:AVCLK \n
 *						6:~SVLCK, 7:ClKGEN0's Output( Only use Clock generator1 )
 *	@return		None.
 *	@remarks	DPC controller have two clock generator. so \e Index must set to 0 or 1.\n
 *				Only Clock generator 1 can set to ClkGEN0's output.
 *	@see		NX_DPC_SetClockPClkMode,		NX_DPC_GetClockPClkMode,
 *												NX_DPC_GetClockSource,
 *				NX_DPC_SetClockDivisor,			NX_DPC_GetClockDivisor,
 *				NX_DPC_SetClockOutInv,			NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *				NX_DPC_SetClockOutDelay,		NX_DPC_GetClockOutDelay,
 *				NX_DPC_SetClockDivisorEnable,	NX_DPC_GetClockDivisorEnable
 */
void NX_DPC_SetClockSource( u32 ModuleIndex, u32 Index, u32 ClkSrc )
{
	const u32 CLKSRCSEL_POS		= 2;
	const u32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register struct NX_DPC_RegisterSet*	pRegister;
	register u32 ReadValue;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	ReadValue = pRegister->DPCCLKGEN[Index][0];
	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

	write32(&pRegister->DPCCLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	DPC controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_DPC_SetClockPClkMode,		NX_DPC_GetClockPClkMode,
 *				NX_DPC_SetClockSource,			NX_DPC_GetClockSource,
 *												NX_DPC_GetClockDivisor,
 *				NX_DPC_SetClockOutInv,			NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *				NX_DPC_SetClockOutDelay,		NX_DPC_GetClockOutDelay,
 *				NX_DPC_SetClockDivisorEnable,	NX_DPC_GetClockDivisorEnable
 */
void NX_DPC_SetClockDivisor( u32 ModuleIndex, u32 Index, u32 Divisor )
{
	const u32 CLKDIV_POS	=	5;
	const u32 CLKDIV_MASK	=	((1<<8)-1) << CLKDIV_POS;

	register struct NX_DPC_RegisterSet*	pRegister;
	register u32 ReadValue;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;
	ReadValue = pRegister->DPCCLKGEN[Index][0];
	ReadValue &= ~CLKDIV_MASK;
	ReadValue |= (Divisor-1) << CLKDIV_POS;

	write32(&pRegister->DPCCLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock output delay of specifed clock generator
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	delay	Select clock output delay of clock generator.\n
 *						0:0ns, 1:0.5ns, 2:1.0ns, 3:1.5ns, 4:2.0ns, 5:2.5ns, \n
 *						6:3.0ns 7:3.5ns
 *	@return		None.
 *	@remarks	DPC controller have two clock generator. so \e Index must set to 0 or 1.\n
 *	@see		NX_DPC_SetClockPClkMode,		NX_DPC_GetClockPClkMode,
 *				NX_DPC_SetClockSource,			NX_DPC_GetClockSource,
 *				NX_DPC_SetClockDivisor,			NX_DPC_GetClockDivisor,
 *				NX_DPC_SetClockOutInv,			NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *												NX_DPC_GetClockOutDelay,
 *				NX_DPC_SetClockDivisorEnable,	NX_DPC_GetClockDivisorEnable
 */
void NX_DPC_SetClockOutDelay( u32 ModuleIndex, u32 Index, u32 delay )
{
	const u32 OUTCLKDELAY_POS	= 0;
	const u32 OUTCLKDELAY_MASK	= 0x1F << OUTCLKDELAY_POS ;

	register struct NX_DPC_RegisterSet* pRegister;
	register u32 	ReadValue;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;
	ReadValue = pRegister->DPCCLKGEN[Index][1];
	ReadValue &= ~OUTCLKDELAY_MASK;
	ReadValue |= (u32)delay << OUTCLKDELAY_POS;

	write32(&pRegister->DPCCLKGEN[Index][1], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set inverting of output clock
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	OutClkInv	\b TRUE indicates that Output clock Invert (Rising Edge). \n
 *							\b FALSE indicates that Output clock Normal (Fallng Edge).
 *	@return		None.
 *	@remarks	DPC controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_DPC_SetClockPClkMode,		NX_DPC_GetClockPClkMode,
 *				NX_DPC_SetClockSource,			NX_DPC_GetClockSource,
 *				NX_DPC_SetClockDivisor,			NX_DPC_GetClockDivisor,
 *												NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *				NX_DPC_SetClockOutDelay,		NX_DPC_GetClockOutDelay,
 *				NX_DPC_SetClockDivisorEnable,	NX_DPC_GetClockDivisorEnable
 */
void NX_DPC_SetClockOutInv( u32 ModuleIndex, u32 Index, boolean OutClkInv )
{
	const u32 OUTCLKINV_POS		=	1;
	const u32 OUTCLKINV_MASK	=	1UL << OUTCLKINV_POS;

	register struct NX_DPC_RegisterSet*	pRegister;
	register u32 	ReadValue;

	pRegister   = __g_ModuleVariablesDpc[ModuleIndex].pRegister;
	ReadValue	= pRegister->DPCCLKGEN[Index][0];

	ReadValue	&=	~OUTCLKINV_MASK;
	ReadValue	|=	OutClkInv << OUTCLKINV_POS;

//	pRegister->DPCCLKGEN[Index][0]	=	ReadValue;
	
//	printf(0, "DPCInv_ReadValue =%u \r\n", ReadValue);
//	printf(0, "&pRegister->DPCCLKGEN[Index][0] =%x \r\n", &pRegister->DPCCLKGEN[Index][0]);
	write32(&pRegister->DPCCLKGEN[Index][0], ReadValue);
}

//--------------------------------------------------------------------------
// TFT LCD specific control function
//--------------------------------------------------------------------------
/**
 *	@brief		Set display mode.
 *	@param[in] ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in] format			Specifies data format.

 *	@param[in] bInterlace		Specifies scan mode.\n
 *									TRUE = Interface, FALSE = Progressive.
 *	@param[in] bInvertField		Specifies internal field polarity.\n
 *									TRUE = Invert Field(Low is even field), FALSE = Normal Field(Low is odd field).

 *	@param[in] bRGBMode			Specifies pixel format.\n
 *									TRUE = RGB Mode, CFASE = YCbCr Mode.
 *	@param[in] bSwapRB			Swap Red and Blue component for RGB output.\n
 *									TRUE = Swap Red and Blue, FALSE = No swap.

 *	@param[in] ycorder			Specifies output order for YCbCr Output.
 *	@param[in] bClipYC			Specifies output range of RGB2YC.\n
 *									TRUE = Y(16 ~ 235), Cb/Cr(16 ~ 240), FALSE = Y/Cb/Cr(0 ~ 255).\n
 *									You have to set to TRUE for ITU-R BT.656 and internal video encoder.
 *	@param[in] bEmbeddedSync	Specifies embedded sync mode(SAV/EAV).\n
 *								TRUE = Enable, FALSE = Disable.\n
 *									You have to set to TRUE for ITU-R BT.656.

 *	@param[in] clock			Specifies the PAD output clock.
 *	@param[in] bInvertClock		Sepcifies the pixel clock polarity.\n
 *									TRUE = rising edge, FALSE = falling edge.
 *
 *	@param[in] bDualView		Specifies the daul view LCD type.\n
 *								TRUE = dual view LCD, FALSE = none.
 *	@return		None.
 *	@see		NX_DPC_GetMode
 */
void NX_DPC_SetMode( u32 ModuleIndex, NX_DPC_FORMAT format,
							boolean bInterlace, boolean bInvertField,
							boolean bRGBMode, boolean bSwapRB,
							NX_DPC_YCORDER ycorder, boolean bClipYC, boolean bEmbeddedSync,
							NX_DPC_PADCLK clock, boolean bInvertClock)
{
	// DPC Control 0 register
	const u32 POLFIELD_POS	= 2;
	const u32 SEAVENB_POS	= 8;
	const u32 SCANMODE_POS	= 9;
	const u32 INTPEND_POS	= 10;
	const u32 RGBMODE_POS	= 12;

	// DPC Control 1 register
	const u32 DITHER_MASK	= 0x3F;
	const u32 YCORDER_POS	= 6;
	const u32 FORMAT_POS	= 8;
	const u32 YCRANGE_POS	= 13;
	const u32 SWAPRB_POS	= 15;

	// DPC Control 2 register
	const u32 PADCLKSEL_POS		= 0;
	const u32 PADCLKSEL_MASK	= 3U<<PADCLKSEL_POS;
	const u32 LCDTYPE_POS		= 7;
	const u32 LCDTYPE_MASK		= 3U<<LCDTYPE_POS;

	register struct NX_DPC_RegisterSet*	pRegister;

	register u32 temp;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	temp  = pRegister->DPCCTRL0;
	temp &= (u32)~(1U<<INTPEND_POS);	// unmask intpend bit.

	if( bInterlace )	temp |= (u32) (1U<<SCANMODE_POS);
	else				temp &= (u32)~(1U<<SCANMODE_POS);
	if( bInvertField )	temp |= (u32) (1U<<POLFIELD_POS);
	else				temp &= (u32)~(1U<<POLFIELD_POS);
	if( bRGBMode )		temp |= (u32) (1U<<RGBMODE_POS);
	else				temp &= (u32)~(1U<<RGBMODE_POS);
	if( bEmbeddedSync )	temp |= (u32) (1U<<SEAVENB_POS);
	else				temp &= (u32)~(1U<<SEAVENB_POS);

	write32(&pRegister->DPCCTRL0, temp);

	temp  = pRegister->DPCCTRL1;
	temp &= (u32)DITHER_MASK;		// mask other bits.
	temp  = (u32)(temp | (ycorder << YCORDER_POS));
	if (format>=16)
	{
		register u32 temp1;
		temp1= pRegister->DPCCTRL2;
		temp1= temp1 | (1<<4);
		write32(&pRegister->DPCCTRL2, temp1);
	}
	else
	{
		register u32 temp1;
		temp1= pRegister->DPCCTRL2;
		temp1= temp1 & ~(1<<4);
		write32(&pRegister->DPCCTRL2, temp1);
	}

	temp  = (u32)(temp | ((format&0xf) << FORMAT_POS));

	if( !bClipYC )	temp |= (u32)(1U<<YCRANGE_POS);
	if( bSwapRB )	temp |= (u32)(1U<<SWAPRB_POS);

	write32(&pRegister->DPCCTRL1, temp);

	temp  = pRegister->DPCCTRL2;
	temp &= (u32)~(PADCLKSEL_MASK | LCDTYPE_MASK );		// TFT or Video Encoder
	temp  = (u32)(temp | (clock<<PADCLKSEL_POS));

	write32(&pRegister->DPCCTRL2, temp);

	// Determines whether invert or not the polarity of the pad clock.
	NX_DPC_SetClockOutInv( ModuleIndex, 0, bInvertClock );
	NX_DPC_SetClockOutInv( ModuleIndex, 1, bInvertClock );
}


/**
 *	@brief		Set parameters for horizontal sync.
 *	@param[in]	ModuleIndex 	An index of module ( 0 : First DPC ).
 *	@param[in]	AVWidth 	Specifies the active video width in clocks.
 *	@param[in]	HSW 		Specifies the horizontal sync width in clocks
 *	@param[in]	HFP 		Specifies the horizontal sync front porch in clocks.
 *	@param[in]	HBP 		Specifies the horizontal sync back porch in clocks.
 *	@param[in]	bInvHSYNC Specifies HSYNC polarity. TRUE = High active, FALSE = Low active.
 *	@return 	None.
 *	@remark 	A sum of arguments except bInvHSYNC has to be less than or equal to 65536.
 *				The unit is VCLK( one clock for a pixel).\n
 *				See follwing figure for more details.
 *	@code
 *
 *						<---------------TOTAL------------------->
 *						<--SW--->
 *		Sync	--------+		+--------------/-/--------------+		+---
 *						|		|								|		|
 *						+-------+								+-------+
 *				<--FP-> 	<--BP--><----ACTIVE VIDEO--->
 *		Active -+					+-------/-/---------+
 *		Video	|		(BLANK) 	|	(ACTIVE DATA)	|		(BLANK)
 *				+-------------------+					+-----------------
 *						<---ASTART-->
 *						<-------------AEND------------->
 *	@endcode
 *	@see		NX_DPC_GetHSync
 */
void NX_DPC_SetHSync( u32 ModuleIndex, u32 AVWidth, u32 HSW, u32 HFP, u32 HBP, boolean bInvHSYNC )
{
	const u32 INTPEND	= 1U<<10;
	const u32 POLHSYNC	= 1U<<0;
	register u32 temp;
	register struct NX_DPC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	write32(&pRegister->DPCHTOTAL, (u32)(HSW + HBP + AVWidth + HFP - 1));

	write32(&pRegister->DPCHSWIDTH, (u32)(HSW - 1));

	write32(&pRegister->DPCHASTART, (u32)(HSW + HBP - 1));

	write32(&pRegister->DPCHAEND, (u32)(HSW + HBP + AVWidth - 1));

	temp  = pRegister->DPCCTRL0;
	temp &= ~INTPEND;	// unmask intpend bit.

	if( bInvHSYNC ) 	temp |= (u32) POLHSYNC;
		else				temp &= (u32)~POLHSYNC;

	write32(&pRegister->DPCCTRL0, temp);
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set parameters for vertical sync.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	AVHeight		Specifies the active video height in lines.
 *	@param[in]	VSW			Specifies the vertical sync width in lines.
 *								When interlace mode, this value is used for odd field.
 *	@param[in]	VFP			Specifies the vertical sync front porch in lines.
 *								When interlace mode, this value is used for odd field.
 *	@param[in]	VBP			Specifies the vertical sync back porch in lines.
 *								When interlace mode, this value is used for odd field.
 *	@param[in]	bInvVSYNC		Specifies VSYNC polarity. TRUE = High active, FALSE = Low active.
 *	@param[in]	EAVHeight		Specifies the active video height in lines for even field.
 *	@param[in]	EVSW			Specifies the vertical sync width in lines for even field.
 *	@param[in]	EVFP			Specifies the vertical sync front porch in lines for even field.
 *	@param[in]	EVBP			Specifies the vertical sync back porch in lines for even field.
 *	@return		None.
 *	@remark		A sum of arguments(AVHeight + VSW + VFP + VBP or AVHeight + EVSW + EVFP + EVBP)
 *				has to be less than or equal to 65536.\n
 *				See follwing figure for more details.
 *	@code
 *
 *						<----------------TOTAL------------------>
 *						<---SW-->
 *		Sync	--------+		+--------------/-/--------------+		+---
 *						|		|								|		|
 *						+-------+								+-------+
 *				<-FP->	<--BP--> <---ACTIVE VIDEO--->
 *		Active -+					+--------/-/--------+
 *		Video	|		(BLANK)		|	(ACTIVE DATA)	|		(BLANK)
 *				+-------------------+					+-----------------
 *						<---ASTART-->
 *						<-------------AEND------------->
 *	@endcode
 *	@see		NX_DPC_GetVSync
 */
void NX_DPC_SetVSync( u32 ModuleIndex, u32 AVHeight, u32 VSW, u32 VFP, u32 VBP, boolean bInvVSYNC,
							u32 EAVHeight, u32 EVSW, u32 EVFP, u32 EVBP )
{
	const u32 INTPEND	= 1U<<10;
	const u32 POLVSYNC	= 1U<<1;

	register u32 temp;

	register struct NX_DPC_RegisterSet* pRegister;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	write32(&pRegister->DPCVTOTAL, (u32)(VSW + VBP + AVHeight + VFP - 1));

	write32(&pRegister->DPCVSWIDTH, (u32)(VSW - 1));

	write32(&pRegister->DPCVASTART, (u32)(VSW + VBP - 1));

	write32(&pRegister->DPCVAEND, (u32)(VSW + VBP + AVHeight - 1));

	write32(&pRegister->DPCEVTOTAL, (u32)(EVSW + EVBP + EAVHeight + EVFP - 1));

	write32(&pRegister->DPCEVSWIDTH, (u32)(EVSW - 1));

	write32(&pRegister->DPCEVASTART, (u32)(EVSW + EVBP - 1));

	write32(&pRegister->DPCEVAEND, (u32)(EVSW + EVBP + EAVHeight - 1));

	temp  = pRegister->DPCCTRL0;
	temp &= ~INTPEND;	// unmask intpend bit.

	if( bInvVSYNC )		temp |= (u32) POLVSYNC;
	else				temp &= (u32)~POLVSYNC;

	write32(&pRegister->DPCCTRL0, temp);
}

/**
 *	@brief		Set offsets for vertical sync.
 *	@param[in]	ModuleIndex 	An index of module ( 0 : First DPC ).
 *	@param[in]	VSSOffset Specifies the number of clocks from the start of horizontal sync to the start of vertical sync,
 *								where horizontal sync is the last one in vertical front porch.
 *								If this value is 0 then the start of vertical sync synchronizes with the start of horizontal sync
 *								which is the new one in vertical sync.
 *								This value has to be less than HTOTAL. When interlace mode, this vaule is used for odd field.
 *	@param[in]	VSEOffset Specifies the number of clocks from the start of horizontal sync to the end of vertical sync,
 *								where horizontal sync is the last one in vertical sync.
 *								If this value is 0 then the end of vertical sync synchronizes with the start of horizontal sync
 *								which is the new one in vertical back porch.
 *								This value has to be less than HTOTAL. When interlace mode, this vaule is used for odd field.
 *	@param[in]	EVSSOffset Specifies the number of clocks from the start of horizontal sync to the start of vertical sync,
 *								where horizontal sync is the last one in vertical front porch.
 *								If this value is 0 then the start of vertical sync synchronizes with the start of horizontal sync
 *								which is the new one in vertical sync.
 *								This value has to be less than HTOTAL and is used for even field.
 *	@param[in]	EVSEOffset Specifies the number of clocks from the start of horizontal sync to the end of vertical sync,
 *								where horizontal sync is the last one in vertical sync.
 *								If this value is 0 then the end of vertical sync synchronizes with the start of horizontal sync
 *								which is the new one in vertical back porch.
 *								This value has to be less than HTOTAL and is used for even field.
 *	@return 	None.
 *	@remark 	All arguments has to be less than HTOTAL or 65536.
 *				The unit is VCLK( one clock for a pixel).\n
 *				See follwing figure for more details.
 *	@code
 *				<---HTOTAL--->
 *		HSYNC	----+ +---------+ +---/-/---+ +---------+ +-------------
 *					| | 		| | 		| | 		| |
 *					+-+ 		+-+ 		+-+ 		+-+
 *
 *		If VSSOffset == 0 and VSEOffset == 0 then
 *
 *		VSYNC	----------------+								+---------------
 *					(VFP)		|			(VSW)				|		(VBP)
 *								+-------------/-/---------------+
 *
 *		,else
 *
 *		VSYNC	--------+								+---------------------
 *						|<---> tVSSO					|<---> tVSEO
 *						+--------------/-/--------------+
 *				<------>						<------>
 *				VSSOffset						VSEOffset
 *				= HTOTAL - tVSSO				= HTOTAL - tVSEO
 *	@endcode
 *	@see		NX_DPC_GetVSyncOffset
 */
void	NX_DPC_SetVSyncOffset( u32 ModuleIndex, u32 VSSOffset, u32 VSEOffset, u32 EVSSOffset, u32 EVSEOffset )
{
	register struct NX_DPC_RegisterSet* pRegister;
	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	write32(&pRegister->DPCVSEOFFSET, (u32)VSEOffset);

	write32(&pRegister->DPCVSSOFFSET, (u32)VSSOffset);

	write32(&pRegister->DPCEVSEOFFSET, (u32)EVSEOffset);
	//	pRegister->DPCEVSSOFFSET	= (u32)EVSSOffset;

	write32(&pRegister->DPCEVSSOFFSET, (u32)EVSSOffset);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set delay values for data and sync signals.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	DelayRGB_PVD	Specifies the delay value for RGB/PVD signal, 0 ~ 16.
 *	@param[in]	DelayHS_CP1		Specifies the delay value for HSYNC/CP1 signal, 0 ~ 63.
 *	@param[in]	DelayVS_FRAM	Specifies the delay value for VSYNC/FRAM signal, 0 ~ 63.
 *	@param[in]	DelayDE_CP2		Specifies the delay value for DE/CP2 signal, 0 ~ 63.
 *	@return		None.
 *	@remarks	Set delay value for TFT LCD's data and sync signal.\n
 *				\b TFT \b LCD \n
 *				The delay valus for data is generally '0' for normal operation.
 *				but the delay values for sync signals depend on the output format.
 *				The unit is VCLK2.\n
 *				The setting values for normal operation is as follows,
 *	@code
 *		+-----------------------+-----------+-------------------------------+
 *		|		FORMAT			| DelayRGB	| DelayHS, VS, DE				|
 *		+-----------------------+-----------+-------------------------------+
 *		| RGB					|	0		|				4				|
 *		+-----------------------+-----------+-------------------------------+
 *		| MRGB					|	0		|				8				|
 *		+-----------------------+-----------+-------------------------------+
 *		| ITU-R BT.601A			|	0		|				6				|
 *		+-----------------------+-----------+-------------------------------+
 *		| ITU-R BT.656 / 601B	|	0		|				12				|
 *		+-----------------------+-----------+-------------------------------+
 *	@endcode
 *	@see		NX_DPC_GetDelay
 */
void	NX_DPC_SetDelay( u32 ModuleIndex, u32 DelayRGB_PVD, u32 DelayHS_CP1, u32 DelayVS_FRAM, u32 DelayDE_CP2 )
{
	const u32 INTPEND_MASK	= 1U<<10;
	const u32 DELAYRGB_POS	= 4;
	const u32 DELAYRGB_MASK	= 0xFU<<DELAYRGB_POS;
	register u32 temp;

	const u32 DELAYDE_POS	= 0;
	const u32 DELAYVS_POS	= 8;
	const u32 DELAYHS_POS	= 0;

	register struct NX_DPC_RegisterSet*	pRegister;

	pRegister = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	temp  = pRegister->DPCCTRL0;
	temp &= (u32)~(INTPEND_MASK | DELAYRGB_MASK);	// unmask intpend & DELAYRGB bits.
	temp  = (u32)(temp | (DelayRGB_PVD<<DELAYRGB_POS));

//	pRegister->DPCCTRL0 = temp;

	write32(&pRegister->DPCCTRL0, temp);

	write32(&pRegister->DPCDELAY0, (u32)((DelayVS_FRAM<<DELAYVS_POS) | (DelayHS_CP1<<DELAYHS_POS)));

	write32(&pRegister->DPCDELAY1, (u32)(DelayDE_CP2<<DELAYDE_POS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set RGB dithering mode.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	DitherR		Specifies the dithering mode for Red component.
 *	@param[in]	DitherG		Specifies the dithering mode for Green component.
 *	@param[in]	DitherB		Specifies the dithering mode for Blue component.
 *	@return		None.
 *	@remark		The dithering is useful method for case which is that the color
 *				depth of destination is less than one of source.
 *	@see		NX_DPC_GetDither
 */
void NX_DPC_SetDither( u32 ModuleIndex, NX_DPC_DITHER DitherR, NX_DPC_DITHER DitherG, NX_DPC_DITHER DitherB )
{
	const u32 DITHER_MASK	= 0x3FU;
	const u32 RDITHER_POS	= 0;
	const u32 GDITHER_POS	= 2;
	const u32 BDITHER_POS	= 4;
	register u32 temp;

	register struct NX_DPC_RegisterSet*	pRegister;

	pRegister   = __g_ModuleVariablesDpc[ModuleIndex].pRegister;

	temp = pRegister->DPCCTRL1;
	temp &= (u32)~DITHER_MASK;	// unmask dithering mode.
	temp = (u32)(temp | ((DitherB<<BDITHER_POS) | (DitherG<<GDITHER_POS) | (DitherR<<RDITHER_POS)));

	write32(&pRegister->DPCCTRL1, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the screen size.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	width		the screen width, 1 ~ 4096.
 *	@param[in]	height		the screen height, 1 ~ 4096.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void NX_MLC_SetScreenSize( u32 ModuleIndex, u32 width, u32 height )
{
	register struct NX_MLC_RegisterSet* pRegister;
	register u32    regvalue;

	pRegister = __g_ModuleVariablesMlc[ModuleIndex].pRegister;
	regvalue  = ((height-1)<<16) | (width-1);

	write32(&pRegister->MLCSCREENSIZE, regvalue);
}

//--------------------------------------------------------------------------
//	Display controller operations
//--------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable Display controller.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	bEnb	Set it to TRUE to enable display controller.
 *	@return		None.
 *	@see		NX_DPC_GetDPCEnable
 */
void	NX_DPC_SetDPCEnable( u32 ModuleIndex, boolean bEnb )
{
	const u32 INTPEND_POS	= 10;
	const u32 INTPEND_MASK	= 1UL << INTPEND_POS;
	const u32 DPCENB_POS	= 15;
	const u32 DPCENB_MASK	= 1UL << DPCENB_POS;

	register struct NX_DPC_RegisterSet* pRegister;
	register u32 ReadValue;

	pRegister	=	__g_ModuleVariablesDpc[ModuleIndex].pRegister;

	ReadValue	=	pRegister->DPCCTRL0;
	ReadValue	&=	~(INTPEND_MASK|DPCENB_MASK);
	ReadValue	|=	(u32)bEnb << DPCENB_POS;

	write32(&pRegister->DPCCTRL0, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	ModuleIndex		An index of module ( 0 : First DPC ).
 *	@param[in]	Enable	\b TRUE	indicates that Enable of clock generator. \n
 *						\b FALSE	indicates that Disable of clock generator.
 *	@return		None.
 *	@see		NX_DPC_SetClockPClkMode,		NX_DPC_GetClockPClkMode,
 *				NX_DPC_SetClockSource,			NX_DPC_GetClockSource,
 *				NX_DPC_SetClockDivisor,			NX_DPC_GetClockDivisor,
 *				NX_DPC_SetClockOutInv,			NX_DPC_GetClockOutInv,
 *				NX_DPC_SetClockOutSelect,		NX_DPC_GetClockOutSelect,
 *				NX_DPC_SetClockOutEnb,			NX_DPC_GetClockOutEnb,
 *				NX_DPC_SetClockOutDelay,		NX_DPC_GetClockOutDelay,
 *												NX_DPC_GetClockDivisorEnable
 */
void			NX_DPC_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable )
{
	const u32	CLKGENENB_POS	=	2;
	const u32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register struct NX_DPC_RegisterSet*	pRegister;
	register u32 	ReadValue;

	pRegister   = __g_ModuleVariablesDpc[ModuleIndex].pRegister;
	ReadValue	= pRegister->DPCCLKENB;
	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (u32)Enable << CLKGENENB_POS;

	write32(&pRegister->DPCCLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get module's reset index.
 *	@return		Module's reset index.\n
 *				It is equal to RESETINDEX_OF_LVDS?_MODULE_i_nRST in <nx_chip.h>.
 *	@see		NX_RSTCON_Enter,
 *				NX_RSTCON_Leave,
 *				NX_RSTCON_GetStatus
 */
u32 NX_LVDS_GetResetNumber ( u32 ModuleIndex )
{
	const u32 ResetNumber[] =
	{
		RESETINDEX_OF_LVDS_MODULE_I_RESETN
	};

	return	ResetNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 1 ).
 *	@param[in]	Enable	\b TRUE	indicates that Enable of clock generator. \n
 *						\b FALSE	indicates that Disable of clock generator.
 *	@return		None.
 *	@see		NX_DISPTOP_CLKGEN_SetClockPClkMode,		NX_DISPTOP_CLKGEN_GetClockPClkMode,
 *				NX_DISPTOP_CLKGEN_SetClockSource,			NX_DISPTOP_CLKGEN_GetClockSource,
 *				NX_DISPTOP_CLKGEN_SetClockDivisor,		NX_DISPTOP_CLKGEN_GetClockDivisor,
 *				NX_DISPTOP_CLKGEN_GetClockDivisorEnable
 */
void NX_DISPTOP_CLKGEN_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable )
{
	const u32	CLKGENENB_POS	=	2;
	const u32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register u32 ReadValue;

	ReadValue	=	__g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKENB;
	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (u32)Enable << CLKGENENB_POS;

	write32(&__g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 1 ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@param[in]	ClkSrc	Select clock source of clock generator ( 0: PLL0, 1:PLL1 ).
 *	@remarks	CLKGEN have one clock generator. so \e Index must set to 0.
 *	@return		None.
 *	@see		NX_DISPTOP_CLKGEN_SetClockPClkMode,		NX_DISPTOP_CLKGEN_GetClockPClkMode,
 *				NX_DISPTOP_CLKGEN_GetClockSource,
 *				NX_DISPTOP_CLKGEN_SetClockDivisor,		NX_DISPTOP_CLKGEN_GetClockDivisor,
 *				NX_DISPTOP_CLKGEN_SetClockDivisorEnable,	NX_DISPTOP_CLKGEN_GetClockDivisorEnable
 */
void NX_DISPTOP_CLKGEN_SetClockSource( u32 ModuleIndex, u32 Index, u32 ClkSrc )
{
	const u32 CLKSRCSEL_POS		= 2;
	const u32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register u32 ReadValue;

	ReadValue = __g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKGEN[Index<<1];
	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

	write32(&__g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKGEN[Index<<1], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 1 ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 )
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	CLKGEN have one clock generator. so \e Index must set to 0.
 *	@see		NX_DISPTOP_CLKGEN_SetClockPClkMode,		NX_DISPTOP_CLKGEN_GetClockPClkMode,
 *				NX_DISPTOP_CLKGEN_SetClockSource,			NX_DISPTOP_CLKGEN_GetClockSource,
 *				NX_DISPTOP_CLKGEN_GetClockDivisor,
 *				NX_DISPTOP_CLKGEN_SetClockDivisorEnable,	NX_DISPTOP_CLKGEN_GetClockDivisorEnable
 */
void NX_DISPTOP_CLKGEN_SetClockDivisor( u32 ModuleIndex, u32 Index, u32 Divisor )
{
	const u32 CLKDIV_POS	=	5;
	const u32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register u32 ReadValue;

	ReadValue	=	__g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKGEN[Index<<1];
	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

	write32(&__g_ModuleVariablesClk[ModuleIndex].__g_pRegister->CLKGEN[Index<<1], ReadValue);
}

void NX_LVDS_SetLVDSCTRL0( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSCTRL0, regvalue);
}

void NX_LVDS_SetLVDSCTRL1( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSCTRL1, regvalue);
}

void NX_LVDS_SetLVDSCTRL2( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSCTRL2, regvalue);
}
void NX_LVDS_SetLVDSCTRL3( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSCTRL3, regvalue);
}
void NX_LVDS_SetLVDSCTRL4( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSCTRL4, regvalue);
}


void NX_LVDS_SetLVDSTMODE0( u32 ModuleIndex, u32 regvalue )
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSTMODE0, regvalue);
}


void NX_LVDS_SetLVDSLOC0 ( u32 ModuleIndex, u32 regvalue ) //'h20
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC0, regvalue);
}
void NX_LVDS_SetLVDSLOC1 ( u32 ModuleIndex, u32 regvalue ) //'h24
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC1, regvalue);
}

void NX_LVDS_SetLVDSLOC2 ( u32 ModuleIndex, u32 regvalue ) //'h28
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC2, regvalue);
}

void NX_LVDS_SetLVDSLOC3 ( u32 ModuleIndex, u32 regvalue ) //'h2C
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC3, regvalue);
}

void NX_LVDS_SetLVDSLOC4 ( u32 ModuleIndex, u32 regvalue ) //'h30
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC4, regvalue);
}

void NX_LVDS_SetLVDSLOC5 ( u32 ModuleIndex, u32 regvalue ) //'h34
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC5, regvalue);
}

void NX_LVDS_SetLVDSLOC6 ( u32 ModuleIndex, u32 regvalue ) //'h38
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOC6, regvalue);
}

void NX_LVDS_SetLVDSLOCMASK0 ( u32 ModuleIndex, u32 regvalue ) //'h40
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOCMASK0, regvalue);
}

void NX_LVDS_SetLVDSLOCMASK1 ( u32 ModuleIndex, u32 regvalue ) //'h44
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOCMASK1, regvalue);
}

void NX_LVDS_SetLVDSLOCPOL0  ( u32 ModuleIndex, u32 regvalue ) //'h48
{
	register NX_LVDS_RegisterSet* pRegister;

	pRegister = __g_pRegister_lvds[ModuleIndex];

	write32(&pRegister->LVDSLOCPOL0, regvalue);
}

void	NX_DISPLAYTOP_SetLVDSMUX( boolean bEnb, u32 SEL )
{
	register struct NX_DISPLAYTOP_RegisterSet *pRegister;
	u32 regvalue;

	pRegister = __g_ModuleVariables.pRegister;

	regvalue = (bEnb<<31) | (SEL<<0);
	write32(&pRegister->LVDS_MUX_CTRL, (u32)regvalue);
}

