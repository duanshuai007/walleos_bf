//------------------------------------------------------------------------------
//
//  Copyright (C) Nexell Co. 2012
//
//  This confidential and proprietary software may be used only as authorized by a
//  licensing agreement from Nexell Co.
//  The entire notice above must be reproduced on all authorized copies and copies
//  may only be made to the extent permitted by a licensing agreement from Nexell Co.
//
//  Module      :
//  File        : nx_DWC_GMAC.h
//  Description :
//  Author      :
//  History     :
//------------------------------------------------------------------------------
#ifndef _ENV_INCLUDE_DRIVER_ETH_NX_DWC_GMAC_H_
#define _ENV_INCLUDE_DRIVER_ETH_NX_DWC_GMAC_H_

#include <types.h>
#include "descs.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define _SIM_						1
#define PHY_ID                      0x001C
#define NUMBER_OF_DWC_GMAC_CHANNEL  2
#define DMA_PBL_VALUE               (16 << 8)       // Burst Length
#define PKTSIZE                     1518
#define PKTSIZE_ALIGN               1536 
#define PKTBUFSRX                   8
#define PKTALIGN                    32

//------------------------------------------------------------------------------------
// * REG_DMA_OPMODE (0x00001000) Bit Definitions
//------------------------------------------------------------------------------------
enum {
     BIT_DMA_BM_MB                  =   0x04000000  // Mixed Burst
    ,BIT_DMA_BM_AAL                 =   0x02000000  // Address-Aligned Beats
    ,BIT_DMA_BM_8xPBL               =   0x01000000  // 8xPBL Mode
    ,BIT_DMA_BM_USP                 =   0x00800000  // Use Separate PBL
    ,BIT_DMA_BM_RPBL_MASK           =   0x007E0000  // RX DMA PBL Mask
    ,BIT_DMA_BM_RPBL_SHIFT          =   17          // RX DMA PBL Shift
    ,BIT_DMA_BM_FB                  =   0x00010000  // Fixed Burst
    ,BIT_DMA_BM_PR_MASK             =   0x0000C000  // Priority Ratio Mask
    ,BIT_DMA_BM_PR_SHIFT            =   14          // Priority Ratio Shift
    ,BIT_DMA_BM_PBL_MASK            =   0x00003f00  // Programmable Burst Length Mask
    ,BIT_DMA_BM_PBL_SHIFT           =   8           // Programmable Burst Length Shift
    ,BIT_DMA_BM_ATDS                =   0x00000080  // Alternate Descriptor Size
    ,BIT_DMA_BM_DSL_MASK            =   0x0000007C  // Descriptor Skip Length Mask
    ,BIT_DMA_BM_DSL_SHIFT           =   2           //        (in DWORDS)
    ,BIT_DMA_BM_DA                  =   0x00000002  // DMA Arbitration scheme
    ,BIT_DMA_BM_SWR                 =   0x00000001  // Software Reset
};

//------------------------------------------------------------------------------------
// * REG_DMA_AXI_BUS_MODE (0x00001000) Bit Definitions
//------------------------------------------------------------------------------------
enum {
     BIT_DMA_ABM_EN_LPI             =   0x80000000  // Enable Low Power Interface
    ,BIT_DMA_ABM_LPI_XIT_FRM        =   0x40000000  // Unlock on Magic Packet/Remote Wake-Up Frame
    ,BIT_DMA_ABM_WR_OSR_LMT_MASK    =   0x00F00000  // AXI Maximum Write Outstanding Request Limit
    ,BIT_DMA_ABM_WR_OSR_LMT_SHIFT   =   20
    ,BIT_DMA_ABM_RD_OSR_LMT_MASK    =   0x000F0000  // AXI Maximum Read Outstanding Request Limit
    ,BIT_DMA_ABM_RD_OSR_LMT_SHIFT   =   16
    ,BIT_DMA_ABM_ONEKBBE            =   0x00002000
    ,BIT_DMA_ABM_AXI_AAL            =   0x00001000
    ,BIT_DMA_ABM_BLEN256            =   0x00000080
    ,BIT_DMA_ABM_BLEN128            =   0x00000040
    ,BIT_DMA_ABM_BLEN64             =   0x00000020
    ,BIT_DMA_ABM_BLEN32             =   0x00000010
    ,BIT_DMA_ABM_BLEN16             =   0x00000008
    ,BIT_DMA_ABM_BLEN8              =   0x00000004
    ,BIT_DMA_ABM_BLEN4              =   0x00000002
    ,BIT_DMA_ABM_UNDEF              =   0x00000001
};

/************************************************************************************/
/* REG_DMA_STATUS (0x00001014) Bit Definitions                                      */
/************************************************************************************/
enum{
	BIT_DMA_STS_EB_MASK				= 0x03800000, /* Error Bits Mask */
	BIT_DMA_STS_EB_TX_ABORT			= 0x00800000, /* Error Bits - TX Abort */
	BIT_DMA_STS_EB_RX_ABORT			= 0x01000000, /* Error Bits - RX Abort */
	BIT_DMA_STS_TS_MASK				= 0x00700000, /* Transmit Process State */
	BIT_DMA_STS_TS_SHIFT			= 20,
	BIT_DMA_STS_RS_MASK				= 0x000e0000, /* Receive Process State */
	BIT_DMA_STS_RS_SHIFT			= 17,
};

enum{
	BIT_DMA_STS_NIS					= 0x00010000,  /* Normal Interrupt Summary */
	BIT_DMA_STS_AIS             	= 0x00008000,  /* Abnormal Interrupt Summary */
	BIT_DMA_STS_ERI             	= 0x00004000,  /* Early Receive Interrupt */
	BIT_DMA_STS_FBI             	= 0x00002000,  /* Fatal Bus Error Interrupt */
	BIT_DMA_STS_ETI             	= 0x00000400,  /* Early Transmit Interrupt */
	BIT_DMA_STS_RWT             	= 0x00000200,  /* Receive Watchdog Timeout */
	BIT_DMA_STS_RPS             	= 0x00000100,  /* Receive Process Stopped */
	BIT_DMA_STS_RU              	= 0x00000080,  /* Receive Buffer Unavailable */
	BIT_DMA_STS_RI              	= 0x00000040,  /* Receive Interrupt */
	BIT_DMA_STS_UNF             	= 0x00000020,  /* Transmit Underflow */
	BIT_DMA_STS_OVF             	= 0x00000010,  /* Receive Overflow */
	BIT_DMA_STS_TJT             	= 0x00000008,  /* Transmit Jabber Timeout */
	BIT_DMA_STS_TU              	= 0x00000004,  /* Transmit Buffer Unavailable */
	BIT_DMA_STS_TPS             	= 0x00000002,  /* Transmit Process Stopped */
	BIT_DMA_STS_TI              	= 0x00000001,  /* Transmit Interrupt */
};

// DMA interrupt
enum{
	BIT_DMA_INT_NIT					= 0x00010000,  // (NIS)Normal interrupt summary                    RW      0
	BIT_DMA_INT_AIE             	= 0x00008000,  // (AIS)Abnormal interrupt summary                  RW      0
	BIT_DMA_INT_ERE             	= 0x00004000,  // Early receive interrupt              Normal      RW      0
	BIT_DMA_INT_FBE             	= 0x00002000,  // Fatal bus error                      Abnormal    RW      0
	BIT_DMA_INT_ETE             	= 0x00000400,  // Early transmit interrupt             Abnormal    RW      0
	BIT_DMA_INT_RWE             	= 0x00000200,  // Receive Watchdog Timeout             Abnormal    RW      0
	BIT_DMA_INT_RSE             	= 0x00000100,  // Receive process stopped              Abnormal    RW      0
	BIT_DMA_INT_RUE             	= 0x00000080,  // Receive buffer unavailable           Abnormal    RW      0
	BIT_DMA_INT_RIE             	= 0x00000040,  // Completion of frame reception        Normal      RW      0
	BIT_DMA_INT_UNE             	= 0x00000020,  // Transmit underflow                   Abnormal    RW      0
	BIT_DMA_INT_OVE             	= 0x00000010,  // Receive Buffer overflow interrupt    Abnormal    RW      0
	BIT_DMA_INT_TJE             	= 0x00000008,  // Transmit Jabber Timeout              Abnormal    RW      0
	BIT_DMA_INT_TUE             	= 0x00000004,  // Transmit buffer unavailable          Normal      RW      0
	BIT_DMA_INT_TSE             	= 0x00000002,  // Transmit Stopped                     Abnormal    RW      0
	BIT_DMA_INT_TIE             	= 0x00000001,  // Transmit Interrupt                   Normal      RW      0
};

enum{
	NUM_DMA_INT_NIT					= 16,          // (NIS)Normal interrupt summary                    RW      0
	NUM_DMA_INT_AIE             	= 15,          // (AIS)Abnormal interrupt summary                  RW      0
	NUM_DMA_INT_ERE             	= 14,          // Early receive interrupt              Normal      RW      0
	NUM_DMA_INT_FBE             	= 13,          // Fatal bus error                      Abnormal    RW      0
	NUM_DMA_INT_ETE             	= 10,          // Early transmit interrupt             Abnormal    RW      0
	NUM_DMA_INT_RWE             	= 9,           // Receive Watchdog Timeout             Abnormal    RW      0
	NUM_DMA_INT_RSE             	= 8,           // Receive process stopped              Abnormal    RW      0
	NUM_DMA_INT_RUE             	= 7,           // Receive buffer unavailable           Abnormal    RW      0
	NUM_DMA_INT_RIE             	= 6,           // Completion of frame reception        Normal      RW      0
	NUM_DMA_INT_UNE             	= 5,           // Transmit underflow                   Abnormal    RW      0
	NUM_DMA_INT_OVE             	= 4,           // Receive Buffer overflow interrupt    Abnormal    RW      0
	NUM_DMA_INT_TJE             	= 3,           // Transmit Jabber Timeout              Abnormal    RW      0
	NUM_DMA_INT_TUE             	= 2,           // Transmit buffer unavailable          Normal      RW      0
	NUM_DMA_INT_TSE             	= 1,           // Transmit Stopped                     Abnormal    RW      0
	NUM_DMA_INT_TIE             	= 0,           // Transmit Interrupt                   Normal      RW      0
};


/************************************************************************************
 * REG_DMA_OPMODE (0x00001018) Bit Definitions
 ************************************************************************************/
enum {
     BIT_DMA_OPMODE_PASCFALSE_FRAME =   0x04000000  // DT: Disable Dropping of TCP/IP Checksum Error Frames
    ,BIT_DMA_OPMODE_RX_FORCE        =   0x02000000  // DMA Store & Foward
    ,BIT_DMA_OPMODE_TX_FORCE        =   0x00200000  // DMA Store & Foward
    ,BIT_DMA_OPMODE_FLUSH_TX        =   0x00100000  // Tx Flush
    ,BIT_DMA_OPMODE_TTC_MASK        =   0x0001C000  // Transmit Threshold Control
    ,BIT_DMA_OPMODE_TTC_SHIFT       =   14
    ,BIT_DMA_OPMODE_TX_START        =   0x00002000  // Start/Stop Transmission
    ,BIT_DMA_OPMODE_2ND_FRAME       =   0x00000004  // OP Mode Second Frame
    ,BIT_DMA_OPMODE_RX_START        =   0x00000002  // Start/Stop Receive
};

/************************************************************************************
 *
 * Generic MII Registers / Bits Definitions
 *
 ************************************************************************************/


/************************************************************************************
 * Generic MII Registers Offset Definitions (Ref. IEEE 802.3)
 ************************************************************************************/
enum {
     REG_MII_BMCR               =   0x00   /* Basic mode control register */
    ,REG_MII_BMSR               =   0x01   /* Basic mode status register  */
    ,REG_MII_PHYSID1            =   0x02   /* PHYS ID 1                   */
    ,REG_MII_PHYSID2            =   0x03   /* PHYS ID 2                   */
    ,REG_MII_ADVERTISE          =   0x04   /* Advertisement control reg   */
    ,REG_MII_LPA                =   0x05   /* Link partner ability reg    */
    ,REG_MII_EXPANSION          =   0x06   /* Expansion register          */
    ,REG_MII_CTRL1000           =   0x09   /* 1000BASE-T control          */
    ,REG_MII_STAT1000           =   0x0a   /* 1000BASE-T status           */
    ,REG_MII_ESTATUS            =   0x0f   /* Extended Status             */
    ,REG_MII_SPECIFIC_CONFIG    =   0x10   /* Phy Specific Config         */
    ,REG_MII_SPECIFIC_STATUS    =   0x11   /* Phy Specific Status         */
    ,REG_MII_DCOUNTER           =   0x12   /* Disconnect counter          */
    ,REG_MII_FCSCOUNTER         =   0x13   /* False carrier counter       */
    ,REG_MII_NWAYTEST           =   0x14   /* N-way auto-neg test reg     */
    ,REG_MII_EXT_PHY_CTRL       =   0x14
    ,REG_MII_RERRCOUNTER        =   0x15   /* Receive error counter       */
    ,REG_MII_SREVISION          =   0x16   /* Silicon revision            */
    ,REG_MII_RESV1              =   0x17   /* Reserved...                 */
    ,REG_MII_LBRERROR           =   0x18   /* Lpback, rx, bypass error    */
    ,REG_MII_PHYADDR            =   0x19   /* PHY address                 */
    ,REG_MII_RESV2              =   0x1a   /* Reserved...                 */
    ,REG_MII_TPISTATUS          =   0x1b   /* TPI status for 10mbps       */
    ,REG_MII_NCONFIG            =   0x1c   /* Network interface config    */
};


/************************************************************************************
 * REG_MII_BMCR (0x00) Bit Definitions
 ************************************************************************************/
enum {
     BIT_BMCR_RESV              =   0x003f  /* Unused...                   */
    ,BIT_BMCR_SPEED1000         =   0x0040  /* MSB of Speed (1000)         */
    ,BIT_BMCR_CTST              =   0x0080  /* Collision test              */
    ,BIT_BMCR_FULLDPLX          =   0x0100  /* Full duplex                 */
    ,BIT_BMCR_ANRESTART         =   0x0200  /* Auto negotiation restart    */
    ,BIT_BMCR_ISOLATE           =   0x0400  /* Disconnect DP83840 from MII */
    ,BIT_BMCR_PDOWN             =   0x0800  /* Powerdown the DP83840       */
    ,BIT_BMCR_ANENABLE          =   0x1000  /* Enable auto negotiation     */
    ,BIT_BMCR_SPEED100          =   0x2000  /* Select 100Mbps              */
    ,BIT_BMCR_LOOPBACK          =   0x4000  /* TXD loopback bits           */
    ,BIT_BMCR_RESET             =   0x8000  /* Reset the DP83840           */
};

/************************************************************************************
 * REG_MII_BMSR (0x01) Bit Definitions
 ************************************************************************************/
#define BIT_BMSR_ERCAP              0x0001  /* Ext-reg capability          */
#define BIT_BMSR_JCD                0x0002  /* Jabber detected             */
#define BIT_BMSR_LSTATUS            0x0004  /* Link status                 */
#define BIT_BMSR_ANEGCAPABLE        0x0008  /* Able to do auto-negotiation */
#define BIT_BMSR_RFAULT             0x0010  /* Remote fault detected       */
#define BIT_BMSR_ANEGCOMPLETE       0x0020  /* Auto-negotiation complete   */
#define BIT_BMSR_RESV               0x00c0  /* Unused...                   */
#define BIT_BMSR_ESTATEN            0x0100  /* Extended Status in R15      */
#define BIT_BMSR_100HALF2           0x0200  /* Can do 100BASE-T2 HDX       */
#define BIT_BMSR_100FULL2           0x0400  /* Can do 100BASE-T2 FDX       */
#define BIT_BMSR_10HALF             0x0800  /* Can do 10mbps, half-duplex  */
#define BIT_BMSR_10FULL             0x1000  /* Can do 10mbps, full-duplex  */
#define BIT_BMSR_100HALF            0x2000  /* Can do 100mbps, half-duplex */
#define BIT_BMSR_100FULL            0x4000  /* Can do 100mbps, full-duplex */
#define BIT_BMSR_100BASE4           0x8000  /* Can do 100mbps, 4k packets  */

/************************************************************************************
 * REG_MII_ADVERTISE (0x04) Bit Definitions
 ************************************************************************************/
#define BIT_ADVERTISE_SLCT          0x001f  /* Selector bits               */
#define BIT_ADVERTISE_CSMA          0x0001  /* Only selector supported     */
#define BIT_ADVERTISE_10HALF        0x0020  /* Try for 10mbps half-duplex  */
#define BIT_ADVERTISE_1000XFULL     0x0020  /* Try for 1000BASE-X full-duplex */
#define BIT_ADVERTISE_10FULL        0x0040  /* Try for 10mbps full-duplex  */
#define BIT_ADVERTISE_1000XHALF     0x0040  /* Try for 1000BASE-X half-duplex */
#define BIT_ADVERTISE_100HALF       0x0080  /* Try for 100mbps half-duplex */
#define BIT_ADVERTISE_1000XPAUSE    0x0080  /* Try for 1000BASE-X pause    */
#define BIT_ADVERTISE_100FULL       0x0100  /* Try for 100mbps full-duplex */
#define BIT_ADVERTISE_1000XPSE_ASYM 0x0100  /* Try for 1000BASE-X asym pause */
#define BIT_ADVERTISE_100BASE4      0x0200  /* Try for 100mbps 4k packets  */
#define BIT_ADVERTISE_PAUSE_CAP     0x0400  /* Try for pause               */
#define BIT_ADVERTISE_PAUSE_ASYM    0x0800  /* Try for asymetric pause     */
#define BIT_ADVERTISE_RESV          0x1000  /* Unused...                   */
#define BIT_ADVERTISE_RFAULT        0x2000  /* Say we can detect faults    */
#define BIT_ADVERTISE_LPACK         0x4000  /* Ack link partners response  */
#define BIT_ADVERTISE_NPAGE         0x8000  /* Next page bit               */

#define BIT_ADVERTISE_FULL          (BIT_ADVERTISE_100FULL | BIT_ADVERTISE_10FULL | BIT_ADVERTISE_CSMA)
#define BIT_ADVERTISE_ALL           (BIT_ADVERTISE_10HALF | BIT_ADVERTISE_10FULL | BIT_ADVERTISE_100HALF | BIT_ADVERTISE_100FULL)

/************************************************************************************
 * REG_MII_LPA (0x05) Bit Definitions
 ************************************************************************************/
#define BIT_LPA_SLCT                0x001f  /* Same as advertise selector  */
#define BIT_LPA_10HALF              0x0020  /* Can do 10mbps half-duplex   */
#define BIT_LPA_1000XFULL           0x0020  /* Can do 1000BASE-X full-duplex */
#define BIT_LPA_10FULL              0x0040  /* Can do 10mbps full-duplex   */
#define BIT_LPA_1000XHALF           0x0040  /* Can do 1000BASE-X half-duplex */
#define BIT_LPA_100HALF             0x0080  /* Can do 100mbps half-duplex  */
#define BIT_LPA_1000XPAUSE          0x0080  /* Can do 1000BASE-X pause     */
#define BIT_LPA_100FULL             0x0100  /* Can do 100mbps full-duplex  */
#define BIT_LPA_1000XPAUSE_ASYM     0x0100  /* Can do 1000BASE-X pause asym*/
#define BIT_LPA_100BASE4            0x0200  /* Can do 100mbps 4k packets   */
#define BIT_LPA_PAUSE_CAP           0x0400  /* Can pause                   */
#define BIT_LPA_PAUSE_ASYM          0x0800  /* Can pause asymetrically     */
#define BIT_LPA_RESV                0x1000  /* Unused...                   */
#define BIT_LPA_RFAULT              0x2000  /* Link partner faulted        */
#define BIT_LPA_LPACK               0x4000  /* Link partner acked us       */
#define BIT_LPA_NPAGE               0x8000  /* Next page bit               */

#define BIT_LPA_DUPLEX              (BIT_LPA_10FULL | BIT_LPA_100FULL)
#define BIT_LPA_100                 (BIT_LPA_100FULL | BIT_LPA_100HALF | BIT_LPA_100BASE4)

/************************************************************************************
 * REG_MII_EXPANSION (0x06) Bit Definitions
 ************************************************************************************/
#define BIT_EXPANSION_NWAY          0x0001  /* Can do N-way auto-nego      */
#define BIT_EXPANSION_LCWP          0x0002  /* Got new RX page code word   */
#define BIT_EXPANSION_ENABLENPAGE   0x0004  /* This enables npage words    */
#define BIT_EXPANSION_NPCAPABLE     0x0008  /* Link partner supports npage */
#define BIT_EXPANSION_MFAULTS       0x0010  /* Multiple faults detected    */
#define BIT_EXPANSION_RESV          0xffe0  /* Unused...                   */

/************************************************************************************
 * BIT_BMSR_ESTATEN (0x0f) Bit Definitions
 ************************************************************************************/
#define BIT_ESTATUS_1000_TFULL      0x2000  /* Can do 1000BT Full */
#define BIT_ESTATUS_1000_THALF      0x1000  /* Can do 1000BT Half */

/************************************************************************************
 * REG_MII_SPECIFIC_CONFIG (0x10) Bit Definitions
 ************************************************************************************/
#define BIT_SPCONFIG_CRSSELECT      0x0080
#define BIT_SPCONFIG_SLEEPMODE      0x0040

/************************************************************************************
 * REG_MII_SPECIFIC_STATUS (0x11) Bit Definitions
 ************************************************************************************/
#if 0
#define BIT_SPCIFIC_100BASET            0x4000
#define BIT_SPCIFIC_TRANSMITTING        0x2000
#define BIT_SPCIFIC_RECEIVING           0x1000
#define BIT_SPCIFIC_COLLISION           0x0800
#define BIT_SPCIFIC_LINKUP              0x0400
#define BIT_SPCIFIC_FULL_DUPLEX         0x0200
#define BIT_SPCIFIC_AUTO_NEGO_MODE      0x0100
#define BIT_SPCIFIC_AUTO_NEGO_COMPLETE  0x0080
#define BIT_SPCIFIC_PAUSE_CAPABLE       0x0010
#define BIT_SPCIFIC_ERROR               0x0008
#else

#define BIT_SPCIFIC_SPEED_MASK          0xc000
#define BIT_SPCIFIC_SPEED_1000          0x8000
#define BIT_SPCIFIC_SPEED_100           0x4000
#define BIT_SPCIFIC_SPEED_10            0x0000

#define BIT_SPCIFIC_DPX_MASK            0x2000
#define BIT_SPCIFIC_DPX_FULL            0x2000
#define BIT_SPCIFIC_DPX_HALF            0x0000

#define BIT_SPCIFIC_LINK_MASK           0x0400
#define BIT_SPCIFIC_LINK_UP             0x0400
#define BIT_SPCIFIC_LINK_DOWN           0x0000

#endif

/************************************************************************************
 * REG_MII_NWAYTEST (0x14) Bit Definitions
 ************************************************************************************/
#define BIT_NWAYTEST_RESV1          0x00ff  /* Unused...                   */
#define BIT_NWAYTEST_LOOPBACK       0x0100  /* Enable loopback for N-way   */
#define BIT_NWAYTEST_RESV2          0xfe00  /* Unused...                   */

/* 1000BASE-T Control register */
#define BIT_ADVERTISE_1000FULL      0x0200  /* Advertise 1000BASE-T full duplex */
#define BIT_ADVERTISE_1000HALF      0x0100  /* Advertise 1000BASE-T half duplex */

/* 1000BASE-T Status register */
#define BIT_LPA_1000LOCALRXOK       0x2000  /* Link partner local receiver status */
#define BIT_LPA_1000REMRXOK         0x1000  /* Link partner remote receiver status */
#define BIT_LPA_1000FULL            0x0800  /* Link partner 1000BASE-T full duplex */
#define BIT_LPA_1000HALF            0x0400

/* Link partner 1000BASE-T half duplex */

/* Flow control flags */
#define BIT_FLOW_CTRL_TX            0x01
#define BIT_FLOW_CTRL_RX            0x02


/************************************************************************************
 * REG_MAC_CONFIG (0x00000000) Bit Definitions
 ************************************************************************************/
//@modified - sei
#define BIT_CONFIG_SARC2            0x20000000  /* Source Address Insertion */
#define BIT_CONFIG_SARC3            0x30000000  /* Source Address Replacement */
#define BIT_CONFIG_CST              0x02000000  // [   25]  CRC Stripping for Type Frames
#define BIT_CONFIG_TC               0x01000000  // [   24]  Transmit Conf. in RGMII/SGMII
#define BIT_CONFIG_WD               0x00800000  // [   23]  Disable Watchdog on Receive
#define BIT_CONFIG_JD               0x00400000  // [   22]  Jabber disable
#define BIT_CONFIG_BE               0x00200000  // [   21]  Frame Burst Enable
#define BIT_CONFIG_JE               0x00100000  // [   20]  Jumbo Frame
#define BIT_CONFIG_IFG_96BT         0x00000000  // [19:17]  Inter Frame Gap
#define BIT_CONFIG_IFG_88BT         0x00020000
#define BIT_CONFIG_IFG_80BT         0x00040000
#define BIT_CONFIG_IFG_64BT         0x00080000
#define BIT_CONFIG_IFG_40BT         0x000e0000
#define BIT_CONFIG_IFG_MASK         0x000e0000
#define BIT_CONFIG_DCRS             0x00010000  // [   16]  Disable Carrier Sense During TX
#define BIT_CONFIG_PS               0x00008000  // [   15]  Port Select 0:1000 Mbps, 1:100 Mbps
#define BIT_CONFIG_FES              0x00004000  // [   14]  Speed 0:10 Mbps, 1:100 Mbps
#define BIT_CONFIG_DO               0x00002000  // [   13]  Disable RX Own
#define BIT_CONFIG_LM               0x00001000  // [   12]  Loop-back Mode
#define BIT_CONFIG_DM               0x00000800  // [   11]  Duplex Mode
#define BIT_CONFIG_IPC              0x00000400  // [   10]  Checksum Offload
#define BIT_CONFIG_DR               0x00000200  // [    9]  Disable Retry
#define BIT_CONFIG_LUD              0x00000100  // [    8]  Link Up/Down
#define BIT_CONFIG_ACS              0x00000080  // [    7]  Automatic Pad Stripping
#define BIT_CONFIG_BL10             0x00000000  // [ 6: 5]  Back-Off Limit  k = min(n,10) : 2'b00
#define BIT_CONFIG_BL08             0x00000020  // [ 6: 5]  Back-Off Limit  k = min(n, 8) : 2'b01
#define BIT_CONFIG_BL04             0x00000040  // [ 6: 5]  Back-Off Limit  k = min(n, 4) : 2'b10
#define BIT_CONFIG_BL01             0x00000060  // [ 6: 5]  Back-Off Limit  k = min(n, 1) : 2'b11
#define BIT_CONFIG_DC               0x00000010  // [    4]  Deferral Check
#define BIT_CONFIG_TE               0x00000008  // [    3]  Transmitter Enable
#define BIT_CONFIG_RE               0x00000004  // [    2]  Receiver Enable
#define BIT_CONFIG_PRELEN_7B        0x00000000  // [ 1: 0]  Preamble Length for Transmit frames 7 bytes
#define BIT_CONFIG_PRELEN_5B        0x00000001  // [ 1: 0]  Preamble Length for Transmit frames 5 bytes
#define BIT_CONFIG_PRELEN_3B        0x00000002  // [ 1: 0]  Preamble Length for Transmit frames 3 bytes

#define BIT_CONFIG_FULLDUPLEX_INIT_VALUE    ( BIT_CONFIG_JD | BIT_CONFIG_BE | BIT_CONFIG_DCRS | BIT_CONFIG_DM | BIT_CONFIG_LUD)
#define BIT_CONFIG_HALFDUPLEX_INIT_VALUE    ( BIT_CONFIG_JD | BIT_CONFIG_BE | BIT_CONFIG_LUD)


/************************************************************************************
 * REG_MAC_FRAME_FILTER (0x00000004) Bit Definitions
 ************************************************************************************/
enum
{
    E_BIT_MCFILTER_RCV_ALL          = 0x80000000,

    E_BIT_MCFILTER_OFF              = 0x80000000,                   // Filter All
    E_BIT_MCFILTER_ON               = 0x00000000,

    E_BIT_HPF_ENABLE                = 0x00000400,                   // Hash or Perfect Filter
    E_BIT_HPF_DISABLE               = 0x00000000,

    E_BIT_SAF_ENABLE                = 0x00000200,                   // Source Address Filter
    E_BIT_SAF_DISABLE               = 0x00000000,

    E_BIT_SAIF_ENABLE               = 0x00000100,                   // Source Address Inverse Filter
    E_BIT_SAIF_DISABLE              = 0x00000000,

    E_BIT_PC_3                      = 0x000000C0,
    E_BIT_PC_2                      = 0x00000080,
    E_BIT_PC_1                      = 0x00000040,
    E_BIT_PC_0                      = 0x00000000,

    E_BIT_BROADF_ENABLE             = 0x00000000,
    E_BIT_BROADF_DISABLE            = 0x00000020,

    E_BIT_MULTIF_ENABLE             = 0x00000010,
    E_BIT_MULTIF_DISABLE            = 0x00000000,

    E_BIT_DESTF_ENABLE              = 0x00000008,
    E_BIT_DESTF_DISABLE             = 0x00000000,

    E_BIT_HASHF_ENABLE              = 0x00000004,
    E_BIT_HASHF_DISABLE             = 0x00000000,

    E_BIT_UHASHF_ENABLE             = 0x00000002,
    E_BIT_UHASHF_DISABLE            = 0x00000000,

    E_BIT_PMCOUS_ENABLE             = 0x00000001,
    E_BIT_PMCOUS_DISABLE            = 0x00000000,
};

/************************************************************************************
 * REG_MAC_FLOW_CONTROL (0x00000018) Bit Definitions
 ************************************************************************************/
#define BIT_FC_PT_MASK              0xffff0000  /* [31:16] Pause Time Mask */
#define BIT_FC_PT_SHIFT             16          /* Pause Time Shift */
#define BIT_FC_RFE                  0x00000004  /* RX Flow Control Enable */
#define BIT_FC_TFE                  0x00000002  /* TX Flow Control Enable */
#define BIT_FC_FCB_BPA              0x00000001  /* Flow Control Busy ... */

/************************************************************************************
 * REG_MAC_MII_ADDR (0x00000010) Bit Definitions
 ************************************************************************************/
#define BIT_MIIADR_PA_MASK          0x0000001f  /* MII PHY Address Mask */
#define BIT_MIIADR_PA_SHIFT         11          /* MII PHY Address Shift */
#define BIT_MIIADR_REG_MASK         0x0000001f  /* MII Register Mask */
#define BIT_MIIADR_REG_SHIFT        6           /* MII Register Shift */
#define BIT_MIIADR_WRITE            0x00000002  /* MII Write */
#define BIT_MIIADR_BUSY             0x00000001  /* MII Busy */

#define BIT_MIIADR_CLK_MASK         0x0000003c
#define BIT_MIIADR_CLK_SHIFT        2
#define BIT_MIIADR_CLK_VALUE        2




#define GPIOxALTFN1_ADDR (0xC001E024)
#define GPIOxOUT_ADDR (0xC001E000)
#define GPIOxOUTENB_ADDR (0xC001E004)

#define ADVERTISE_SLCT      0x001f  /* Selector bits               */
#define ADVERTISE_CSMA      0x0001  /* Only selector supported     */
#define ADVERTISE_10HALF    0x0020  /* Try for 10mbps half-duplex  */
#define ADVERTISE_1000XFULL 0x0020  /* Try for 1000BASE-X full-duplex */
#define ADVERTISE_10FULL    0x0040  /* Try for 10mbps full-duplex  */
#define ADVERTISE_1000XHALF 0x0040  /* Try for 1000BASE-X half-duplex */
#define ADVERTISE_100HALF   0x0080  /* Try for 100mbps half-duplex */
#define ADVERTISE_1000XPAUSE    0x0080  /* Try for 1000BASE-X pause    */
#define ADVERTISE_100FULL   0x0100  /* Try for 100mbps full-duplex */
#define ADVERTISE_1000XPSE_ASYM 0x0100  /* Try for 1000BASE-X asym pause */
#define ADVERTISE_100BASE4  0x0200  /* Try for 100mbps 4k packets  */
#define ADVERTISE_PAUSE_CAP 0x0400  /* Try for pause               */
#define ADVERTISE_PAUSE_ASYM    0x0800  /* Try for asymetric pause     */
#define ADVERTISE_RESV      0x1000  /* Unused...                   */
#define ADVERTISE_RFAULT    0x2000  /* Say we can detect faults    */
#define ADVERTISE_LPACK     0x4000  /* Ack link partners response  */
#define ADVERTISE_NPAGE     0x8000  /* Next page bit               */

#define ADVERTISE_FULL      (ADVERTISE_100FULL | ADVERTISE_10FULL | \
                  ADVERTISE_CSMA)
#define ADVERTISE_ALL       (ADVERTISE_10HALF | ADVERTISE_10FULL | \
                  ADVERTISE_100HALF | ADVERTISE_100FULL)


/* Indicates what features are advertised by the interface. */
enum{
	ADVERTISED_10baseT_Half			=	(1 << 0),
	ADVERTISED_10baseT_Full     	=	(1 << 1),
	ADVERTISED_100baseT_Half    	=	(1 << 2),
	ADVERTISED_100baseT_Full    	=	(1 << 3),
	ADVERTISED_1000baseT_Half   	=	(1 << 4),
	ADVERTISED_1000baseT_Full   	=	(1 << 5),
	ADVERTISED_Autoneg				=	(1 << 6),
	ADVERTISED_TP           		=	(1 << 7),
	ADVERTISED_AUI          		=	(1 << 8),
	ADVERTISED_MII          		=	(1 << 9),
	ADVERTISED_FIBRE        		=	(1 << 10),
	ADVERTISED_BNC          		=	(1 << 11),
	ADVERTISED_10000baseT_Full  	=	(1 << 12),
	ADVERTISED_Pause				=	(1 << 13),
	ADVERTISED_Asym_Pause       	=	(1 << 14),
	ADVERTISED_2500baseX_Full   	=	(1 << 15),
	ADVERTISED_Backplane        	=	(1 << 16),
	ADVERTISED_1000baseKX_Full  	=	(1 << 17),
	ADVERTISED_10000baseKX4_Full	=   (1 << 18),
	ADVERTISED_10000baseKR_Full		=	(1 << 19),
	ADVERTISED_10000baseR_FEC		=	(1 << 20),
	ADVERTISED_20000baseMLD2_Full	=   (1 << 21),
	ADVERTISED_20000baseKR2_Full	=   (1 << 22),
};
/* 1000BASE-T Control register */
#define ADVERTISE_1000FULL  0x0200  /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF  0x0100  /* Advertise 1000BASE-T half duplex */
#define CTL1000_AS_MASTER   0x0800
#define CTL1000_ENABLE_MASTER   0x1000


/* Basic mode control register. */
enum{
	BMCR_RESV				= 0x003f,  /* Unused...                   */
	BMCR_SPEED1000      	= 0x0040,  /* MSB of Speed (1000)         */
	BMCR_CTST				= 0x0080,  /* Collision test              */
	BMCR_FULLDPLX       	= 0x0100,  /* Full duplex                 */
	BMCR_ANRESTART      	= 0x0200,  /* Auto negotiation restart    */
	BMCR_ISOLATE        	= 0x0400,  /* Isolate data paths from MII */
	BMCR_PDOWN				= 0x0800,  /* Enable low power state      */
	BMCR_ANENABLE       	= 0x1000,  /* Enable auto negotiation     */
	BMCR_SPEED100       	= 0x2000,  /* Select 100Mbps              */
	BMCR_LOOPBACK       	= 0x4000,  /* TXD loopback bits           */
	BMCR_RESET				= 0x8000,  /* Reset to default state      */
};

/************************************************************************************
 * REG_MMC_CONTROL (0x00000100) Bit Definitions
 ************************************************************************************/
enum{
	BIT_MMC_COUNTER_FREEZE  = 0x00000008,  /* Freeze the Counters */
};
//------------------------------------------------------------------------------
/// @name   DWC_GMAC Descriptor
//------------------------------------------------------------------------------

enum
{
    E_EMAC_INVALID_DUPLEX   = 0,
    E_EMAC_HALFDUPLEX       = 1,
    E_EMAC_FULLDUPLEX       = 2
};

enum
{
    E_EMAC_INVALID_SPEED    = 0,
    E_EMAC_SPEED_10M        = 1,
    E_EMAC_SPEED_100M       = 2,
    E_EMAC_SPEED_1000M      = 3
};

/* DMA structure */

#define EMAC_BUSY_TIMEOUT           3000

#define EMAC_AUTO_NEGO_TIMEOUT      5000                // 3sec

#define EMAC_DMA_ALIGN_SIZE         32                  // MUST BE SAME AS PKTALIGN

#define MAX_ETH_FRAME_SIZE          PKTSIZE_ALIGN
#define CONFIG_DMA_RX_SIZE          128                   // MUST BE SAME AS PKTBUFSRX

#if EMAC_TEST_DEBUG
#define CONFIG_DMA_TX_SIZE          15
#else
#define CONFIG_DMA_TX_SIZE          128
#endif


//typedef struct _PACKED_ EMAC_HANDLE_S
typedef struct __attribute__((aligned(128))) EMAC_HANDLE_S
{
    int         valid;              // 32 bit   - 0x0000 ~ 0x0003

    u16         phy_dev_id;         // 16 bit   - 0x0004 ~ 0x0005
    u8          mac_addr[6];        // 48 bit   - 0x0006 ~ 0x000B

    u16         phy_id1;            // 16 bit   - 0x000C ~ 0x000D
    u16         phy_id2;            // 16 bit   - 0x000E ~ 0x000F
    int         link_status;        // 32 bit   - 0x0010 ~ 0x0013

    u32         duplex_mode;        // 32 bit   - 0x0014 ~ 0x0017
    u32         link_speed;         // 32 bit   - 0x0018 ~ 0x001B

    u32         reserved0[1];       // 32 bit   - 0x001C ~ 0x001F

    // @note sei - descriptor 의 address 는 반드시 bus 의 bit width 에 align 되어야 한다.
    // for DMA
    //dma_extended_desc     dma_rx_desciptor[CONFIG_DMA_RX_SIZE];
    dma_extended_desc   dma_rx_desciptor[CONFIG_DMA_RX_SIZE];
    u8                  dma_rx_buffer[CONFIG_DMA_RX_SIZE][PKTSIZE_ALIGN];
    u8                  dma_rx_dummy[32];
    int                 dma_rx_desc_offset;

    u32                 reserved1[3];

    //dma_extended_desc     dma_tx_desciptor[CONFIG_DMA_TX_SIZE];
    dma_extended_desc   dma_tx_desciptor[CONFIG_DMA_TX_SIZE];
    u8                  dma_tx_buffer[CONFIG_DMA_TX_SIZE][PKTSIZE_ALIGN];
    u8                  dma_tx_dummy[32];
    int                 dma_tx_desc_offset;
    u32                 dirty_tx;
    u32                 reserved2[2];
} EMAC_HANDLE_T;

boolean NX_DWC_GMAC_MII_READ(u32 ModuleIndex, u16 PHY_DEV_ID, u32 REG, u16 *VAL);
boolean NX_DWC_GMAC_MII_WRITE(u32 ModuleIndex, u16 PHY_DEV_ID, u32 REG, u32 VAL);
boolean NX_DWC_GMAC_MII_IS_POLL_BUSY(u32 ModuleIndex, u32 TIMEOUT_MS);
boolean NX_DWC_GMAC_DRV_INIT(u32 ModuleIndex, EMAC_HANDLE_T *sEmacHandle);
boolean NX_DWC_GMAC_DRV_TX(u32 ModuleIndex, void *data, u32 len);
int initNetDriver(void);

#ifdef  __cplusplus
}
#endif

#endif // __NX_DWC_GMAC_H__
