#include <types.h>
#include <drivers/ethernet/cfg_type.h>
#include <io.h>
#include <stdio.h>
#include <nxp/nx_gpio.h>
#include <nxp/cfg_gpio.h>
#include <nxp/nx_rstcon.h>
#include <nxp/nx_clk.h>
#include <drivers/ethernet/nx_gmac.h>
#include <s5p4418_tick_delay.h>

static inline u32 ethtool_adv_to_mii_adv_t(u32 ethadv)
{
    u32 result = 0;

    if (ethadv & ADVERTISED_10baseT_Half)
        result |= ADVERTISE_10HALF;
    if (ethadv & ADVERTISED_10baseT_Full)
        result |= ADVERTISE_10FULL;
    if (ethadv & ADVERTISED_100baseT_Half)
        result |= ADVERTISE_100HALF;
    if (ethadv & ADVERTISED_100baseT_Full)
        result |= ADVERTISE_100FULL;
    if (ethadv & ADVERTISED_Pause)
        result |= ADVERTISE_PAUSE_CAP;
    if (ethadv & ADVERTISED_Asym_Pause)
        result |= ADVERTISE_PAUSE_ASYM;

    return result;
}
static inline u32 ethtool_adv_to_mii_ctrl1000_t(u32 ethadv)
{
    u32 result = 0;

    if (ethadv & ADVERTISED_1000baseT_Half)
        result |= ADVERTISE_1000HALF;
    if (ethadv & ADVERTISED_1000baseT_Full)
        result |= ADVERTISE_1000FULL;

    return result;
}

static u16 genphy_restart_aneg(void)
{
    u16 ctl;

    NX_DWC_GMAC_MII_READ(0, 4, REG_MII_BMCR, &ctl);

    if ((s16)ctl < 0)
        return ctl;

    ctl |= (BMCR_ANENABLE | BMCR_ANRESTART);

    /* Don't isolate the PHY if we're negotiating */
    ctl &= ~(BMCR_ISOLATE);

    NX_DWC_GMAC_MII_WRITE(0, 4, REG_MII_BMCR, ctl);

    return ctl;
}

int genphy_config_advert(void)
{
    u16 oldadv, adv; 
    int err, changed = 0;
    unsigned int advertise = 0x00000fff;

    NX_DWC_GMAC_MII_READ(0, 4, REG_MII_ADVERTISE, &adv);
    oldadv =  adv;

    adv &= ~(ADVERTISE_ALL | ADVERTISE_100BASE4 | ADVERTISE_PAUSE_CAP |
           ADVERTISE_PAUSE_ASYM);
    adv |= ethtool_adv_to_mii_adv_t(advertise); 

    if (adv != oldadv) {
        err = NX_DWC_GMAC_MII_WRITE(0, 4, REG_MII_ADVERTISE, adv);

        if (err == 0)
        {
            return err;
        }
        changed = 1;
    }      
 
    NX_DWC_GMAC_MII_READ(0, 4, REG_MII_CTRL1000, &adv);
    oldadv =  adv;

    adv &= ~(ADVERTISE_1000FULL | ADVERTISE_1000HALF);
    adv |= ethtool_adv_to_mii_ctrl1000_t(advertise);

    if (adv != oldadv) {
        err = NX_DWC_GMAC_MII_WRITE(0, 4, REG_MII_CTRL1000, adv);
        if (err == 0){
            return err;
        }
        changed = 1;
    }    

    int result = 0;
    if (changed == 0)
    {
        /* Advertisement hasn't changed, but maybe aneg was never on to
         * begin with?  Or maybe phy was isolated? */
        u16 ctl;
        NX_DWC_GMAC_MII_READ(0, 4, REG_MII_BMCR, &ctl);

        if ((s16)ctl < 0)
            return ctl;

        if (!(ctl & BMCR_ANENABLE) || (ctl & BMCR_ISOLATE))
            result = 1; /* do restart aneg */
    }

    if (result > 0)
    {
        result = genphy_restart_aneg();

        printf("result = %d\r\n", result);
    }
    return result;
}

void bd_gpio_init(void)
{
    int index = 4;
    int screen_4_3 = 0;
    int bit;
    int mode, func, out, lv, plup, stren;
    u32 gpio;
    unsigned int i;

    NX_GPIO_Initialize();
    

    for (i = 0; NX_GPIO_GetNumberOfModule() > i; i++) {
        NX_GPIO_SetBaseAddress(i, NX_GPIO_GetPhysicalAddress(i));
    }

    const u32 pads[] = {
    PAD_GPIOE0 , PAD_GPIOE1 , PAD_GPIOE2 , PAD_GPIOE3 , PAD_GPIOE4 , PAD_GPIOE5 , PAD_GPIOE6 , PAD_GPIOE7 , PAD_GPIOE8 , PAD_GPIOE9 ,
    PAD_GPIOE10, PAD_GPIOE11, PAD_GPIOE12, PAD_GPIOE13, PAD_GPIOE14, PAD_GPIOE15, PAD_GPIOE16, PAD_GPIOE17, PAD_GPIOE18, PAD_GPIOE19,
    PAD_GPIOE20, PAD_GPIOE21, PAD_GPIOE22, PAD_GPIOE23, PAD_GPIOE24, PAD_GPIOE25, PAD_GPIOE26, PAD_GPIOE27, PAD_GPIOE28, PAD_GPIOE29,
    PAD_GPIOE30, PAD_GPIOE31
    };

    for (bit = 0; 32 > bit; bit++) {
        if (bit < 7 
            || (bit > 11 && bit < 14) 
            || (bit > 21 && bit < 24) 
            || bit > 24) 
        {   
            continue;
        }   
        gpio  = pads[bit];
        func  = PAD_GET_FUNC(gpio);
        mode  = PAD_GET_MODE(gpio);
        lv    = PAD_GET_LEVEL(gpio);
        stren = PAD_GET_STRENGTH(gpio);
        plup  = PAD_GET_PULLUP(gpio);

        /* get pad alternate function (0,1,2,4) */
        switch (func) {
        case PAD_GET_FUNC(PAD_FUNC_ALT0): func = NX_GPIO_PADFUNC_0; break;
        case PAD_GET_FUNC(PAD_FUNC_ALT1): func = NX_GPIO_PADFUNC_1; break;
        case PAD_GET_FUNC(PAD_FUNC_ALT2): func = NX_GPIO_PADFUNC_2; break;
        case PAD_GET_FUNC(PAD_FUNC_ALT3): func = NX_GPIO_PADFUNC_3; break;
        default: printf("ERROR, unknown alt func (%d.%02d=%d)\r\n", index, bit, func);
            continue;
        }

        switch (mode) {
        case PAD_GET_MODE(PAD_MODE_ALT): out = 0;
        case PAD_GET_MODE(PAD_MODE_IN ): out = 0;
        case PAD_GET_MODE(PAD_MODE_INT): out = 0; break;
        case PAD_GET_MODE(PAD_MODE_OUT): out = 1; break;
        default: printf("ERROR, unknown io mode (%d.%02d=%d)\r\n", index, bit, mode);
            continue;
        }

        NX_GPIO_SetPadFunction(index, bit, func);
        NX_GPIO_SetOutputEnable(index, bit, (out ? CTRUE : CFALSE));
        NX_GPIO_SetOutputValue(index, bit,  (lv  ? CTRUE : CFALSE));
        NX_GPIO_SetInterruptMode(index, bit, (lv));

        NX_GPIO_SetPullMode(index, bit, plup);

        if(1 == screen_4_3)
        {
            set_gpio_strenth(index, bit, stren); /* pad strength */
        }
        else
        {
            if(index > 0)
                set_gpio_strenth(index, bit, stren); /* pad strength */
            else
                set_gpio_strenth(index, bit, 2); /* pad strength */
        }
    }
    NX_GPIO_SetDRV0_DISABLE_DEFAULT(index, 0xFFFFFFFF);
    NX_GPIO_SetDRV1_DISABLE_DEFAULT(index, 0xFFFFFFFF);

}

static void nxp_soc_gpio_set_io_drv(int gpio, int mode)
{
    int drv1 = 0, drv0 = 0;
    int drv1_value, drv0_value;
    int grp, bit;

    if (gpio > (PAD_GPIO_ALV - 1) )
        return;

    grp = PAD_GET_GROUP(gpio);
    bit = PAD_GET_BITNO(gpio);

    switch (mode)
    {
    case 0: drv0 = 0, drv1 = 0; break;
    case 1: drv0 = 0, drv1 = 1; break;
    case 2: drv0 = 1, drv1 = 0; break;
    case 3: drv0 = 1, drv1 = 1; break;
    default: break;
    }

    drv1_value = NX_GPIO_GetDRV1(grp) & ~(1 << bit);

    drv0_value = NX_GPIO_GetDRV0(grp) & ~(1 << bit);

    if (drv1) drv1_value |= (drv1 << bit);
    if (drv0) drv0_value |= (drv0 << bit);


    NX_GPIO_SetDRV0 ( grp, drv0_value );
    NX_GPIO_SetDRV1 ( grp, drv1_value );

}

int  nxpmac_init(void)
{
    u32 addr;

	nxp_soc_gpio_set_io_drv((PAD_GPIO_E +  7), 3);     // PAD_GPIOE7,     GMAC0_PHY_TXD[0]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E +  8), 3);     // PAD_GPIOE8,     GMAC0_PHY_TXD[1]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E +  9), 3);     // PAD_GPIOE9,     GMAC0_PHY_TXD[2]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 10), 3);     // PAD_GPIOE10,    GMAC0_PHY_TXD[3]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 11), 3);     // PAD_GPIOE11,    GMAC0_PHY_TXEN
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 14), 3);     // PAD_GPIOE14,    GMAC0_PHY_RXD[0]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 15), 3);     // PAD_GPIOE15,    GMAC0_PHY_RXD[1]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 16), 3);     // PAD_GPIOE16,    GMAC0_PHY_RXD[2]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 17), 3);     // PAD_GPIOE17,    GMAC0_PHY_RXD[3]
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 18), 3);     // PAD_GPIOE18,    GMAC0_RX_CLK
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 19), 3);     // PAD_GPIOE19,    GMAC0_PHY_RX_DV
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 20), 3);     // PAD_GPIOE20,    GMAC0_GMII_MDC
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 21), 3);     // PAD_GPIOE21,    GMAC0_GMII_MDI
    nxp_soc_gpio_set_io_drv((PAD_GPIO_E + 24), 3);     // PAD_GPIOE24,    GMAC0_GTX_CLK

    printf("NXP mac 1000Base-T gpio init\r\n");

// Clock control
    NX_CLKGEN_Initialize();
    addr = NX_CLKGEN_GetPhysicalAddress(CLOCKINDEX_OF_DWC_GMAC_MODULE);
    NX_CLKGEN_SetBaseAddress(CLOCKINDEX_OF_DWC_GMAC_MODULE, (u32)IO_ADDRESS(addr));

    NX_CLKGEN_SetClockSource(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 4);     // Sync mode for 100 & 10Base-T : External RX_clk
    NX_CLKGEN_SetClockDivisor(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 1);    // Sync mode for 100 & 10Base-T

    NX_CLKGEN_SetClockOutInv(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, CFALSE);    // TX Clk invert off : 100 & 10Base-T
//      NX_CLKGEN_SetClockOutInv(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, CTRUE);     // TX clk invert on : 100 & 10Base-T

    NX_CLKGEN_SetClockDivisorEnable(CLOCKINDEX_OF_DWC_GMAC_MODULE, CTRUE);

    // Reset control
    NX_RSTCON_Initialize();
    addr = NX_RSTCON_GetPhysicalAddress();
    NX_RSTCON_SetBaseAddress((u32)IO_ADDRESS(addr));
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_ENABLE);
    udelay(100);
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_DISABLE);
    udelay(100);
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_ENABLE);
    udelay(100);
#if 1

    writel(GPIOxALTFN1_ADDR, 0x00010555);

    writel(GPIOxALTFN1_ADDR, 0x00010555);
    writel(GPIOxOUT_ADDR, 0x00401000);
    writel(GPIOxOUTENB_ADDR, 0x00401000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00001000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00401000);
#endif

    return 0;
}

static int gmac_phy_reset(void)
{
    writel(GPIOxOUT_ADDR, 0x00401000);
    writel(GPIOxOUTENB_ADDR, 0x00401000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00001000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00401000);

    return 0;
}

int stmmac_mdio_reset(void)
{
    gmac_phy_reset();

    writel(0xc0060010, 0);

    return 0;

}

int nxp_plat_initialize(void)
{

// Clock control
    NX_CLKGEN_Initialize();
    u32 addr = NX_CLKGEN_GetPhysicalAddress(CLOCKINDEX_OF_DWC_GMAC_MODULE);
    NX_CLKGEN_SetBaseAddress(CLOCKINDEX_OF_DWC_GMAC_MODULE, (u32)IO_ADDRESS(addr));

    NX_CLKGEN_SetClockSource(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 4);     // Sync mode for 100 & 10Base-T : External RX_clk
    NX_CLKGEN_SetClockDivisor(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 1);    // Sync mode for 100 & 10Base-T

    NX_CLKGEN_SetClockOutInv(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, CFALSE);    // TX Clk invert off : 100 & 10Base-T
//      NX_CLKGEN_SetClockOutInv(CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, CTRUE);     // TX clk invert on : 100 & 10Base-T

    NX_CLKGEN_SetClockDivisorEnable(CLOCKINDEX_OF_DWC_GMAC_MODULE, CTRUE);

    // Reset control
    NX_RSTCON_Initialize();
    addr = NX_RSTCON_GetPhysicalAddress();
    NX_RSTCON_SetBaseAddress((u32)IO_ADDRESS(addr));
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_ENABLE);
    udelay(100);
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_DISABLE);
    udelay(100);
    NX_RSTCON_SetnRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_ENABLE);
    udelay(100);


    writel(GPIOxALTFN1_ADDR, 0x00010555);

    writel(GPIOxALTFN1_ADDR, 0x00010555);
    writel(GPIOxOUT_ADDR, 0x00401000);
    writel(GPIOxOUTENB_ADDR, 0x00401000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00001000);

    udelay(100);
    writel(GPIOxOUTENB_ADDR, 0x00401000);
    writel(GPIOxOUT_ADDR, 0x00401000);

	return 0;
}
