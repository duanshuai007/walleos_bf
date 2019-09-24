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
//  File        : nx_DWC_GMAC.c
//  Description :
//  Author      :
//  History     :
//------------------------------------------------------------------------------
#include <nx_chip.h>
#include "drivers/ethernet/nx_gmac.h"
#include "drivers/ethernet/eth_common.h"
#include <io.h>
#include "drivers/ethernet/descs_com.h"
#include <stdio.h>
#include "drivers/ethernet/dwmac1000.h"
#include "drivers/ethernet/dwmac_dma.h"
#include <string.h>
#include <s5p4418_tick_delay.h>
#include <s5p4418_interrupt.h>
#include <alloc.h>
#include <errno.h>
#include <config.h>

extern void net_input(void *data, u32 len);

EMAC_HANDLE_T *hEmac[NUMBER_OF_DWC_GMAC_MODULE];

//const char _default_mac_addr[6] = {0xda, 0xba, 0x00, 0x13, 0x24, 0xfe};
char _default_mac_addr[6] = {0xf6,0x64,0xd1,0xc9,0x3b,0xe4};

//static const char *_str_duplex_mode(int mode)
//{
//    switch (mode)
//    {
//        case E_EMAC_HALFDUPLEX:
//            return "Half Duplex";
//        case E_EMAC_FULLDUPLEX:
//            return "Full Duplex";
//        default:
//            return "Invalid Duplex Mode";
//    }
//}


//static const char *_str_link_speed(int speed)
//{
//    switch (speed)
//    {
//        case E_EMAC_SPEED_1000M:
//            return "1000 BaseT";
//        case E_EMAC_SPEED_100M:
//            return "100 BaseT";
//        case E_EMAC_SPEED_10M:
//            return "10 BaseT";
//        default:
//            return "Invalid Link Speed";
//    }
//}


static boolean NX_DWC_GMAC_CSR_AUTONEGOTIATION(u32 ModuleIndex, EMAC_HANDLE_T *handle)
{
    unsigned short temp_value;

    while(1) {
        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_BMSR, &temp_value);

        if (temp_value & BIT_BMSR_ANEGCOMPLETE)
            break;
    }
    printf("[NXLOG] Auto Negotiation Success.\r\n" );
    return 1;
}

//------------------------------------------------------------------------------
/// @name   DWC_GMAC DMA Interface
//------------------------------------------------------------------------------

static boolean   NX_DWC_GMAC_DMA_SWReset(void)
{
    __writel(0x1, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + DMA_BUS_MODE);

    while(readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + DMA_BUS_MODE) & 0x1);

    return 1;
}
/**
 * stmmac_clear_descriptors: clear descriptors
 * @priv: driver private structure
 * Description: this function is called to clear the tx and rx descriptors
 * in case of both basic and extended descriptors are used.
 */
static void stmmac_clear_descriptors( u32 ModuleIndex )
{
    unsigned int i;
    unsigned int rxsize = CONFIG_DMA_RX_SIZE;
    unsigned int txsize = CONFIG_DMA_TX_SIZE;

    /* Clear the Rx/Tx descriptors */
    for (i = 0; i < rxsize; i++)
    {
        enh_desc_init_rx_desc(&(hEmac[ModuleIndex]->dma_rx_desciptor[i].basic),
                         1, STMMAC_RING_MODE,
                         (i == rxsize - 1));
    }

    for (i = 0; i < txsize; i++)
    {
        enh_desc_init_tx_desc(&(hEmac[ModuleIndex]->dma_tx_desciptor[i].basic),
                         STMMAC_RING_MODE,
                         (i == txsize - 1));
    }
}

static void NX_DWC_GMAC_DMA_INIT_RX_DESC(dma_desc *p, void *buffer)
{
//    p->des0.data = p->des1.data = 0;

//    p->des0.rx.own = 1;
//    p->des1.rx.buffer1_size = PKTSIZE_ALIGN;
//    p->des1.rx.disable_ic = 1;
//    p->des1.rx.end_ring = end_ring;
      p->des2 = (unsigned int)buffer;
//    p->des3 = NULL;
}

static void NX_DWC_GMAC_DMA_INIT_TX_DESC(dma_desc *p)
{
     p->des2 = 0;
}

static boolean   NX_DWC_GMAC_DMA_Init( u32 ModuleIndex )
{
    s32 i;
    void *rx_buf; 
    //void *tx_buf;
    dma_desc *rx_desc, *tx_desc;

    // Setup DMA to Ring mode
    hEmac[ModuleIndex]->dma_rx_desc_offset = 0;
    hEmac[ModuleIndex]->dma_tx_desc_offset = 0;


    for( i=0; i<CONFIG_DMA_RX_SIZE; i++ )
    {
        rx_desc = (dma_desc*)&(hEmac[ModuleIndex]->dma_rx_desciptor[i].basic);
        rx_buf  = (void*)hEmac[ModuleIndex]->dma_rx_buffer[i];

        // Initialize the contersts of the DMA buffers
        NX_DWC_GMAC_DMA_INIT_RX_DESC( rx_desc, rx_buf);
    }

    for( i=0; i<CONFIG_DMA_TX_SIZE; i++ )
    {
        tx_desc = (dma_desc*)&(hEmac[ModuleIndex]->dma_tx_desciptor[i].basic);
        //tx_buf  = (void*)hEmac[ModuleIndex]->dma_tx_buffer[i];

        // Initialize the contersts of the DMA buffers
        NX_DWC_GMAC_DMA_INIT_TX_DESC( tx_desc);
    }


    stmmac_clear_descriptors(ModuleIndex);

    int pbl = 8;
    int fb = 0;
    int mb = 0;
    int burst_len = 0;
    u32 dma_tx = (u32)&(hEmac[ModuleIndex]->dma_tx_desciptor[0]);
    u32 dma_rx = (u32)&(hEmac[ModuleIndex]->dma_rx_desciptor[0]);
    int atds = 1;
    dwmac1000_dma_init((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, pbl, fb, mb, burst_len, dma_tx, dma_rx, atds);

    return 1;
}

//------------------------------------------------------------------------------
/// @name   DWC_GMAC MII Interface
//------------------------------------------------------------------------------

boolean NX_DWC_GMAC_MII_IS_POLL_BUSY(u32 ModuleIndex, u32 TIMEOUT_MS)
{
    UNUSED(ModuleIndex);
    UNUSED(TIMEOUT_MS);
    while(1) {
        //if (!(macr_read(REG_MAC_MII_ADDR) & BIT_MIIADR_BUSY))
        if(!(readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR) & BIT_MIIADR_BUSY))
        {
            return 0;
        }
    }

    return 1;
}

boolean NX_DWC_GMAC_MII_WRITE(u32 ModuleIndex, u16 PHY_DEV_ID, u32 REG, u32 VAL)
{
    u32 MII_ADDR;
    MII_ADDR = readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);
    MII_ADDR &= BIT_MIIADR_CLK_MASK;
    MII_ADDR |= ((PHY_DEV_ID & BIT_MIIADR_PA_MASK) << BIT_MIIADR_PA_SHIFT) | ((REG & BIT_MIIADR_REG_MASK) << BIT_MIIADR_REG_SHIFT) | BIT_MIIADR_WRITE | BIT_MIIADR_BUSY;

    if (NX_DWC_GMAC_MII_IS_POLL_BUSY (ModuleIndex, EMAC_BUSY_TIMEOUT))
        return 0;

    __writel(VAL, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_DATA);
    __writel(MII_ADDR, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);

    if (NX_DWC_GMAC_MII_IS_POLL_BUSY (ModuleIndex, EMAC_BUSY_TIMEOUT))
        return 0;

    return 1;
}

boolean NX_DWC_GMAC_MII_READ(u32 ModuleIndex, u16 PHY_DEV_ID, u32 REG, u16 *VAL)
{
    u32 MII_ADDR;
    MII_ADDR = readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);
    MII_ADDR &= BIT_MIIADR_CLK_MASK;
    MII_ADDR |= ((PHY_DEV_ID & BIT_MIIADR_PA_MASK) << BIT_MIIADR_PA_SHIFT) | ((REG & BIT_MIIADR_REG_MASK) << BIT_MIIADR_REG_SHIFT) | BIT_MIIADR_BUSY;

    if (NX_DWC_GMAC_MII_IS_POLL_BUSY (ModuleIndex, EMAC_BUSY_TIMEOUT))
    {
            return 0;
    }
 
    __writel(MII_ADDR, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);
 
    if (NX_DWC_GMAC_MII_IS_POLL_BUSY (ModuleIndex, EMAC_BUSY_TIMEOUT))
    {
        return 0;
    }
 
    *VAL = (u16)readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_DATA);
 
    return 1;
}


//------------------------------------------------------------------------------
/// @name   DWC_GMAC PHY Interface
//------------------------------------------------------------------------------

static boolean   NX_DWC_GMAC_PHY_LINKUP(u32 ModuleIndex, EMAC_HANDLE_T *handle)
{
    unsigned short temp_value;

    NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_BMSR, &temp_value);

    if (!(temp_value & BIT_BMSR_LSTATUS))
    {

        return 0;
    }


    return 1;
}



static boolean   NX_DWC_GMAC_PHY_FIND(u32 ModuleIndex, unsigned short id)
{
    unsigned short phy_id1, phy_id2;
    int phy_dev_id;


    for (phy_dev_id=0; phy_dev_id<32; phy_dev_id++)
    {
        NX_DWC_GMAC_MII_READ(ModuleIndex, phy_dev_id, REG_MII_PHYSID1, &phy_id1);

        NX_DWC_GMAC_MII_READ(ModuleIndex, phy_dev_id, REG_MII_PHYSID2, &phy_id2);

        if (phy_id1 == id)
        {
            // @ todo
            //handle->phy_id1 = phy_id1;
            //handle->phy_id2 = phy_id2;
            return phy_dev_id;
        }
    }

    printf("Can not find PHY\r\n");
    // @ todo
    //handle->phy_id1 = -1;
    //handle->phy_id2 = -1;
    return 0;
}


static boolean   NX_DWC_GMAC_PHY_INIT(u32 ModuleIndex, EMAC_HANDLE_T *handle)
{
    // @ todo
#if 0
        handle->phy_dev_id = 1;
        handle->duplex_mode = E_EMAC_FULLDUPLEX;
        handle->link_speed = E_EMAC_SPEED_100M;
#else

        int phy_dev_id = 4;
        unsigned short temp_value1, temp_value2;

         if ((NX_DWC_GMAC_PHY_FIND(ModuleIndex, PHY_ID)) < 0)
         {
             return 0;
         }

        handle->phy_dev_id = phy_dev_id;
        // phy reset

        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_BMCR, &temp_value1);
        //temp_value1 |= BIT_BMCR_RESET;
        temp_value1 = (temp_value1 & ~BIT_BMCR_PDOWN);
        NX_DWC_GMAC_MII_WRITE(ModuleIndex, handle->phy_dev_id, REG_MII_BMCR, temp_value1);
       //wait_msec(100L);                          // MUST BE DELAY HERE

        unsigned short  auto_nego = 0;
        // Advertise Setting
        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_BMSR, &temp_value1);
        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_ADVERTISE, &auto_nego);
        auto_nego |= BIT_ADVERTISE_ALL|0x01|BIT_ADVERTISE_PAUSE_CAP|BIT_ADVERTISE_PAUSE_ASYM;

        if (temp_value1 & BIT_BMSR_100FULL)
        {
            auto_nego |= BIT_ADVERTISE_100FULL;
        }
        if (temp_value1 & BIT_BMSR_100HALF)
        {
            auto_nego |= BIT_ADVERTISE_100HALF;
        }
        if (temp_value1 & BIT_BMSR_10FULL)
        {
            auto_nego |= BIT_ADVERTISE_10FULL;
        }
        if (temp_value1 & BIT_BMSR_10HALF)
        {
            auto_nego |= BIT_ADVERTISE_10HALF;
        }


        NX_DWC_GMAC_MII_WRITE(ModuleIndex, handle->phy_dev_id, REG_MII_ADVERTISE, auto_nego);

        unsigned short  giga_ctrl = 0;
        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_CTRL1000, &giga_ctrl);
        NX_DWC_GMAC_MII_WRITE(ModuleIndex, handle->phy_dev_id, REG_MII_CTRL1000, giga_ctrl);


        // Wait Auto Negotiation
        if (NX_DWC_GMAC_CSR_AUTONEGOTIATION(ModuleIndex, handle) < 0){
            return 0;
        }

        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_SPECIFIC_STATUS, &temp_value1);

        switch (temp_value1 & BIT_SPCIFIC_SPEED_MASK)
        {
            case BIT_SPCIFIC_SPEED_1000:
                printf("1000M Linked\r\n");
                handle->link_speed = E_EMAC_SPEED_1000M;
                break;
            case BIT_SPCIFIC_SPEED_100:
                printf("100M Linked\r\n");
                handle->link_speed = E_EMAC_SPEED_100M;
                break;
            default:
                printf("Unkown Linked\r\n");
            case BIT_SPCIFIC_SPEED_10:
                printf("10M Linked\r\n");
                handle->link_speed = E_EMAC_SPEED_10M;
                break;
        }

        switch (temp_value1 & BIT_SPCIFIC_DPX_MASK)
        {
            case BIT_SPCIFIC_DPX_FULL:
                printf("Full Duplex\r\n");
                handle->duplex_mode = E_EMAC_FULLDUPLEX;
                break;
            default:
                printf("Unkown Linked\r\n");
            case BIT_SPCIFIC_DPX_HALF:
                printf("Full Duplex\r\n");
                handle->duplex_mode = E_EMAC_HALFDUPLEX;
                break;
        }

        // Copper Duplex Mode Set
        NX_DWC_GMAC_MII_READ(ModuleIndex, handle->phy_dev_id, REG_MII_BMCR, &temp_value2);
        if (handle->duplex_mode == E_EMAC_HALFDUPLEX)
            temp_value2 &= ~BIT_BMCR_FULLDPLX;
        else
            temp_value2 |= BIT_BMCR_FULLDPLX;

        NX_DWC_GMAC_MII_WRITE(ModuleIndex, handle->phy_dev_id, REG_MII_BMCR, temp_value2);
#endif

    return 1;
}

static boolean   NX_DWC_GMAC_MDC_DIV(u32 VAL)
{
    u32 temp_data;
    temp_data = readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);
    temp_data &= ~BIT_MIIADR_CLK_MASK;
    temp_data |= (VAL & 0x0f) << 2;
    __writel(temp_data, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_MII_ADDR);
    return 1;
}

static void nxp_set_bus_config(void)
{
    //u32 addr;
    u32 val;
    //int j = 0;
    //int i = 0;
    unsigned int i_slot;
    //unsigned char * pcTmp;
    #define GMACCLKGEN0L_ADDR  (0xC00C8004)
    #define GMACCLKENB_ADDR  (0xC00C8000)

    #define IP_RESET_REGISTER_2_ADDR  (0xC0012008)

    #define GPIOxALTFN1_ADDR (0xC001E024)
    #define GPIOxOUT_ADDR (0xC001E000)
    #define GPIOxOUTENB_ADDR (0xC001E004)
    #define NX_VA_BASE_REG_PL301_DISP (0xC005E000)
    #define NX_VA_BASE_REG_PL301_BOTT (0xC0050000)
    #define NX_PL301_AR_OFFSET              (0x408)
    #define NX_PL301_AW_OFFSET              (0x40C)
    #define NX_BASE_REG_PL301_DISP_AR           (NX_VA_BASE_REG_PL301_DISP | NX_PL301_AR_OFFSET)
    #define NX_BASE_REG_PL301_DISP_AW           (NX_VA_BASE_REG_PL301_DISP | NX_PL301_AW_OFFSET)
    #define SLOT_NUM_POS    24
    #define SI_IF_NUM_POS   0

    #define DISBUS_SI_SLOT_1ST_DISPLAY  0
    #define DISBUS_SI_SLOT_2ND_DISPLAY  1
    #define DISBUS_SI_SLOT_GMAC         2

    const unsigned char g_DispBusSI[3] = {
        DISBUS_SI_SLOT_1ST_DISPLAY,
        DISBUS_SI_SLOT_2ND_DISPLAY,
        DISBUS_SI_SLOT_GMAC
    };
    unsigned int num_si = readl(NX_VA_BASE_REG_PL301_DISP + 0xFC0);
    //unsigned int num_mi = readl(NX_VA_BASE_REG_PL301_BOTT + 0xFC4);
 /* Set progamming for AR */
    // Slave Interface
    for (i_slot = 0; i_slot < num_si; i_slot++)
    {   
        __writel( (0xFF000000 | i_slot),  NX_BASE_REG_PL301_DISP_AR);
        val = readl(NX_BASE_REG_PL301_DISP_AR);
        if (val != g_DispBusSI[i_slot])
        {   
            __writel( (i_slot << SLOT_NUM_POS) | (g_DispBusSI[i_slot] << SI_IF_NUM_POS),  NX_BASE_REG_PL301_DISP_AR );
        }    
    }    

    /* Set progamming for AW */
    // Slave Interface
    for (i_slot = 0; i_slot < num_si; i_slot++)
    {   
        __writel( (0xFF000000 | i_slot),  NX_BASE_REG_PL301_DISP_AW);
        val = readl(NX_BASE_REG_PL301_DISP_AW);
        if (val != g_DispBusSI[i_slot])
        {   
            __writel( (i_slot << SLOT_NUM_POS) | (g_DispBusSI[i_slot] << SI_IF_NUM_POS),  NX_BASE_REG_PL301_DISP_AW );
        }   
    }    
}

void getMacAddr(u8 *macAddr)
{
    dwmac1000_get_umac_addr((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, macAddr, 0);    
}

boolean NX_DWC_GMAC_DRV_INIT(u32 ModuleIndex, EMAC_HANDLE_T *sEmacHandle)
{

    u8 temp_mac[6];
    u32 temp_value;

    hEmac[ModuleIndex] = sEmacHandle;

    memset((u8*)hEmac[ModuleIndex], 0, sizeof(EMAC_HANDLE_T));
    memcpy((u8*)hEmac[ModuleIndex]->mac_addr, _default_mac_addr, 6);

    bd_gpio_init();

    // HW Sequence 0 : Phy Reset Deassert
	// HW Sequence 1 : Phy Give Tx / Rx Clock to EMAC
	// HW Sequence 2 : EMAC Reset Deassert
	// HW Sequence 3 : REG_DMA_AXI_BUS_MODE:0 SWR to 0
	// if HW Sequence 3 is not activate....check HW Reset Signals...
    nxp_set_bus_config();

    mdelay(1000);

    nxpmac_init();

    nxp_plat_initialize();
#if 1
    stmmac_mdio_reset();

    // (0) wait for Phy-SW Reset De-Assert & Hardware Reset(RSTCON) & Clock Divisor
    while (readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + DMA_BUS_MODE) & BIT_DMA_BM_SWR);

    // (1) Check EMAC RTL Version
    temp_value = readl(PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + GMAC_VERSION);
    printf("[NXLOG]EMAC Init : RTL version (%d.%d), Core version (%d.%d)\r\n",
                ((temp_value & 0xf0) >> 4), (temp_value & 0xf),
                ((temp_value & 0xf000) >> 12), ((temp_value & 0xf00) >> 8));

    // (2) DMA Reset
    NX_DWC_GMAC_DMA_SWReset();

    // (3) set MAC Address
    dwmac1000_get_umac_addr((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, temp_mac, 0);
    
    if (!memcmp((u8*)hEmac[ModuleIndex]->mac_addr, temp_mac, 6))
    {

        memcpy((u8*)hEmac[ModuleIndex]->mac_addr, temp_mac, 6);
    }
    

    dwmac1000_set_umac_addr((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, (u8*)hEmac[ModuleIndex]->mac_addr, 0);
    //printf("[NXLOG]MAC Address Set %02x:%02x:%02x:%02x:%02x:%02x\r\n",
      //          hEmac[ModuleIndex]->mac_addr[0], hEmac[ModuleIndex]->mac_addr[1], hEmac[ModuleIndex]->mac_addr[2],
        //        hEmac[ModuleIndex]->mac_addr[3], hEmac[ModuleIndex]->mac_addr[4], hEmac[ModuleIndex]->mac_addr[5]);


    hEmac[ModuleIndex]->valid = 1;

    NX_DWC_GMAC_MDC_DIV(0x02);//??????

    // (4) phy init
    if(NX_DWC_GMAC_PHY_INIT(ModuleIndex, hEmac[ModuleIndex]) == 0)
    {
        //printf("NX_DWC_GMAC_PHY_INIT FAILED\r\n");
        return 0;
    }
    int ddd = 0;
	//printf("NX_DWC ENTER WHILE\r\n");
    while(1)
    {

        if (ddd % (1000*1000) == 0)
        {
           // (5) Link Check
           if(NX_DWC_GMAC_PHY_LINKUP(ModuleIndex, hEmac[ModuleIndex]))
           {
               //printf("[NXLOG]Ethernet Link Up\r\n");
               hEmac[ModuleIndex]->link_status = 1;
           }
           else
           {
               hEmac[ModuleIndex]->link_status = 0;
               //printf("[NXLOG]Ethernet Link Down\r\n");
 //              return 0;
           }

        }

        if (ddd == 2*1000*1000)
        {
            break;
        }

        ddd++;
    }

	//printf("NX_DWC EXIT WHILE\r\n");
    //printf("EMAC Linked : %s / %s\r\n", _str_duplex_mode(hEmac[ModuleIndex]->duplex_mode), _str_link_speed(hEmac[ModuleIndex]->link_speed));

    // (6) If Linked, then Setup DMA
    NX_DWC_GMAC_DMA_Init(ModuleIndex);

	//printf("NX_DWC NX_DWC_GMAC_DMA_Init\r\n");

    void *ioaddr = (void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0;

    dwmac1000_set_umac_addr(ioaddr, (unsigned char *)_default_mac_addr, 0);

	//printf("NX_DWC dwmac1000_set_umac_addr\r\n");

    int mtu = 1500;
    dwmac1000_core_init(ioaddr, mtu);
	//printf("NX_DWC dwmac1000_core_init\r\n");
    dwmac1000_rx_ipc_enable(ioaddr);
	//printf("NX_DWC dwmac1000_rx_ipc_enable\r\n");

    boolean enable = 1;
    stmmac_set_mac(ioaddr, enable);
	//printf("NX_DWC stmmac_set_mac\r\n");
    
    int txmode = 1;
    int rxmode = 1;

    dwmac1000_dma_operation_mode(ioaddr, txmode, rxmode);
	//printf("NX_DWC dwmac1000_dma_operation_mode\r\n");
    
    stmmac_mmc_setup(ioaddr);
	//printf("NX_DWC stmmac_mmc_setup\r\n"); 
    dwmac_dma_start_tx(ioaddr);
    //printf("NX_DWC dwmac_dma_start_tx\r\n");
    dwmac_dma_start_rx(ioaddr);
	//printf("NX_DWC dwmac_dma_start_rx\r\n");
    dwmac1000_rx_watchdog(ioaddr, MAX_DMA_RIWT);
    //printf("NX_DWC dwmac1000_rx_watchdog\r\n");
    __writel(0, ioaddr + GMAC_HASH_HIGH);
    __writel(0, ioaddr + GMAC_HASH_LOW);


    dwmac1000_set_filter(ioaddr);
	//printf("NX_DWC dwmac1000_set_filter\r\n");
    // wait link state
    while (1)
    {
        NX_DWC_GMAC_MII_IS_POLL_BUSY(0, 3000);
        __writel(0x00002051, ioaddr + GMAC_MII_ADDR);
        NX_DWC_GMAC_MII_IS_POLL_BUSY(0, 3000);
        int value = readl(ioaddr + GMAC_MII_DATA); 
        if (value & 0x4)
        {
            break;
        }
    }


	//printf("NX_DWC genphy_config_advert\r\n");
    //MAC_CTRL_REG
    //int value = readl(ioaddr + GMAC_CONTROL);// 0x0061848c

    //bit(11) 0 - 1 半双工到全双工
    //bit(15) 1 - 0 10/100Mbps到1000Mbps
    __writel(0x00610c8c, ioaddr + GMAC_CONTROL);

    mdelay(100);

    genphy_config_advert();

    
#endif

    return 1;
}

/**
 * stmmac_display_ring: display ring
 * @head: pointer to the head of the ring passed.
 * @size: size of the ring.
 * @extend_desc: to verify if extended descriptors are used.
 * Description: display the control/status and buffer descriptors.
 */
//static void stmmac_display_ring(void *head, void  *phy, int size, int extend_desc)
//{
//    int i;
//    dma_extended_desc *ep = (dma_extended_desc *)head;
//    dma_desc *p = (dma_desc *)head;
//
//    for (i = 0; i < size; i++) {
//        u64 x;
//        if (extend_desc) {
//            x = *(u64 *) ep;
//            printf("%3d [0x%p][0x%p]: 0x%08x 0x%08x 0x%08x 0x%08x [%s]\r\n",
//                i, ep, phy, (unsigned int)x, (unsigned int)(x >> 32),
//                ep->basic.des2, ep->basic.des3, (unsigned int)x&(1<<31)?"-":"D");
//            ep++, phy += sizeof(*ep);
//        } else {
//            x = *(u64 *) p;
//            printf("%3d [0x%p][0x%p]: 0x%08x 0x%08x 0x%08x 0x%08x [%s]\r\n",
//                i, p, phy, (unsigned int)x, (unsigned int)(x >> 32),
//                p->des2, p->des3, (unsigned int)x&(1<<31)?"-":"D");
//            p++, phy += sizeof(*p);
//        }
//    }
//}


//static void stmmac_tx_clean(u32 ModuleIndex)
//{
//    unsigned int txsize = CONFIG_DMA_TX_SIZE;
//
//    while (hEmac[ModuleIndex]->dirty_tx != hEmac[ModuleIndex]->dma_tx_desc_offset) {
//        int last;
//        unsigned int entry = hEmac[ModuleIndex]->dirty_tx % txsize;
//        dma_desc *p;
//        p = (dma_desc *)(&(hEmac[ModuleIndex]->dma_tx_desciptor[entry].basic));
//
//        if (enh_desc_get_tx_owner(p)) {
//            break;
//        }
//
//        /* Verify tx error by looking at the last segment. */
//        last = enh_desc_get_tx_ls(p);
//        if (last) {
//            //统计
//        }
//
//        enh_desc_release_tx_desc(p, STMMAC_RING_MODE);
//
//        hEmac[ModuleIndex]->dirty_tx++;
//    }
//}

int tx_count_frames = 0;
boolean NX_DWC_GMAC_DRV_TX(u32 ModuleIndex, void *data, u32 len)
{
    //unsigned int txsize = CONFIG_DMA_TX_SIZE;
    unsigned int entry;
    unsigned int  nopaged_len = len;
    dma_desc *desc;
    int csum_insertion = 0;

    entry = hEmac[ModuleIndex]->dma_tx_desc_offset;    
    desc = (dma_desc *)(&(hEmac[ModuleIndex]->dma_tx_desciptor[entry].basic)); 

    memcpy(hEmac[ModuleIndex]->dma_tx_buffer[entry], data, len);
    // printf("\r\n send data(%d)\r\n", entry);
    // int loop = 0;
    // for (loop = 0; loop < len; ++loop)
    // {
    //     printf("%02x ", hEmac[ModuleIndex]->dma_tx_buffer[entry][loop]);
    //     if ((loop+1) % 16 == 0)
    //     {
    //         printf("\r\n");
    //     }
    // }
    // printf("\r\n\r\n");   

    desc->des2 = (unsigned int)(hEmac[ModuleIndex]->dma_tx_buffer[entry]);
    enh_desc_prepare_tx_desc(desc, 1, nopaged_len, csum_insertion, STMMAC_RING_MODE);

    enh_desc_close_tx_desc(desc);

    tx_count_frames++;
    if (64 > tx_count_frames)
    {
        enh_desc_clear_tx_ic(desc);
    }
    else
    {
         tx_count_frames = 0;       
    }

    enh_desc_set_tx_owner(desc);

//    flush_dcache_all();

    void *ioaddr = (void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0;
    dwmac_enable_dma_transmission(ioaddr);

    if (desc->des01.etx.end_ring)
    {
//        printf("%s : 777777\r\n", __FUNCTION__);
        hEmac[ModuleIndex]->dma_tx_desc_offset = 0;             // wrap, to first
    }
    else
    {
//        printf("%s : 888888\r\n", __FUNCTION__);
        hEmac[ModuleIndex]->dma_tx_desc_offset++;               // advance to next
    }

    return 1;

}

int g_limit = 32;

boolean NX_DWC_GMAC_DRV_RX(u32 ModuleIndex)
{
//    stmmac_tx_clean(ModuleIndex);     

    //void *ioaddr = (void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0;
//    dwmac_disable_dma_irq(ioaddr);

    //unsigned int rxsize = CONFIG_DMA_RX_SIZE;
    unsigned int entry = hEmac[ModuleIndex]->dma_rx_desc_offset;

//    printf("\r\nNX_DWC_GMAC_DRV_RX ModuleIndex = %d, rxsize = %d, entry = %d\r\n", ModuleIndex, rxsize, entry);

//    flush_dcache_all();

    dma_desc *p = (dma_desc *)(&(hEmac[ModuleIndex]->dma_rx_desciptor[entry].basic));
    int owner = enh_desc_get_rx_owner(p);
//    printf("owner = %d\r\n", owner);


    while(owner != 1 && g_limit--)
//    while(owner != 1)
    {
//        printf("while entry = %d\r\n", entry);

        int frame_len = enh_desc_get_rx_frame_len(p, 2) - 4;

        //const unsigned char *pDataBuf = hEmac[ModuleIndex]->dma_rx_buffer[entry];

        // printf("\r\n receive data(%d)\r\n", entry);
        // int loop = 0;
        // for (loop = 0; loop < frame_len; ++loop)
        // {
        //     printf("%02x ", pDataBuf[loop]);
        //     if ((loop+1) % 16 == 0)
        //     {
        //         printf("\r\n");
        //     }
        // }
        // printf("\r\n\r\n");   
        // printf("RX[%d]:  0x%08x ", entry, pDataBuf);
        // printf("DA=%02x:%02x:%02x:%02x:%02x:%02x",
        //     pDataBuf[0], pDataBuf[1], pDataBuf[2], pDataBuf[3], pDataBuf[4], pDataBuf[5]);
        // pDataBuf+=6;
        // printf(" SA=%02x:%02x:%02x:%02x:%02x:%02x",
        //     pDataBuf[0], pDataBuf[1], pDataBuf[2], pDataBuf[3], pDataBuf[4], pDataBuf[5]);
        // pDataBuf+=6;
        // printf(" Type=%04x\r\n\r\n", pDataBuf[0]<<8|pDataBuf[1]);

//        LWIP_Polling(hEmac[ModuleIndex]->dma_rx_buffer[entry], frame_len);
        net_input(hEmac[ModuleIndex]->dma_rx_buffer[entry], frame_len);
        // *data = &hEmac[ModuleIndex]->dma_rx_buffer[entry];
        // *len = frame_len;

        enh_desc_set_rx_owner(p);

        if (p->des01.erx.end_ring)
        {
//            printf("%s : 777777\r\n", __FUNCTION__);
            hEmac[ModuleIndex]->dma_rx_desc_offset = 0;             // wrap, to first
        }
        else
        {
//            printf("%s : 888888\r\n", __FUNCTION__);
            hEmac[ModuleIndex]->dma_rx_desc_offset++;               // advance to next
        }

        entry = hEmac[ModuleIndex]->dma_rx_desc_offset;

        p = (dma_desc *)(&(hEmac[ModuleIndex]->dma_rx_desciptor[entry].basic));
        owner = enh_desc_get_rx_owner(p);

    }

    g_limit = 32;
//    dwmac_enable_dma_irq(ioaddr);

    return 0;

}

//struct stmmac_extra_stats g_extra_stats;
 
//static void stmmac_interrupt(void * data)
//{
//    dwmac1000_irq_status((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, &g_extra_stats);
//    dwmac_dma_interrupt((void *)PHY_BASEADDR_DWC_GMAC_MODULE_APB0, &g_extra_stats);
//
//    NX_DWC_GMAC_DRV_RX(0);
//
//    // int loop = 0;
//    // printf("Receive the length of packet is %d , the data is below:\r\n", len);
//    // for (loop = 0; loop < len; ++loop)
//    // {
//    //     printf("%02x ", pBuf[loop]);
//    //     if ((loop+1) % 32 == 0)
//    //     {
//    //         printf("\r\n");
//    //     }
//    // }
//    // printf("\r\n");
// 
////    __writel(DMA_INTR_DEFAULT_MASK, PHY_BASEADDR_DWC_GMAC_MODULE_APB0 + DMA_INTR_ENA);
//
//}

EMAC_HANDLE_T *sEmacHandle;
int initNetDriver(void)
{
	//printf("[%s]:sizeof(EMAC_HANDLE_T) = %08x\r\n", __func__, sizeof(EMAC_HANDLE_T));

    char *pTemp = (char *)dma_malloc(sizeof(EMAC_HANDLE_T));
	if(pTemp == NULL)
	{
		printf("[%s]dma_malloc failed\r\n", __func__);
		return -ENOMEM;
	}

    sEmacHandle = (EMAC_HANDLE_T*)(pTemp);

    memset(sEmacHandle, 0 , sizeof(EMAC_HANDLE_T));

 // SDMMC_UtilCMD();
    int result = NX_DWC_GMAC_DRV_INIT(0, sEmacHandle);
    if (result == 0)
    {
        printf("NX_DWC_GMAC_DRV_INIT failed\r\n");
        return 0;
    }

    return 1;
}


int enable_Interrupt(void){
//    request_irq("GMAC", stmmac_interrupt, IRQ_TYPE_NONE, 0);
    return 0;
}

