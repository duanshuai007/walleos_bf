/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Jaehoon Chung <jh80.chung@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,  MA 02111-1307 USA
 *
 */
#include <stdio.h>
#include <string.h>
#include <s5p4418_serial_stdio.h>
#include <drivers/common.h>
#include <drivers/dwmmc.h>
#include <alloc.h>
#include <errno.h>
#include <s5p4418_rstcon.h>
#include <config.h>

static char *NXP_NAME       = "NXP DWMMC";
#define	DEV_NAME_SDHC		"nxp-sdhc"
static const u32 CLKDIV0_VALUE = 3;

static unsigned int dw_mci_get_clk(struct dwmci_host *host)
{
    UNUSED(host);
	return 100000000;
}

static void dw_mci_set_clk(int dev_index, unsigned rate)
{
    UNUSED(dev_index);
    UNUSED(rate);
	__writel((CLKGENENB_ENABLE << CLKGENENB_OFFSET), DWMCI_SDMMC0_CLKENB);
	__writel((CLKGENENB_ENABLE << CLKGENENB_OFFSET), DWMCI_SDMMC1_CLKENB);
	__writel((CLKGENENB_ENABLE << CLKGENENB_OFFSET), DWMCI_SDMMC2_CLKENB);

	__writel((CLK_OUTPUT_NORMAL << OUTCLKINV0_OFFSET) 
            | (CLKSRCSEL_PLL1 << CLKSRCSEL0_OFFSET) 
            | (CLKDIV0_VALUE << CLKDIV0_OFFSET) 
            | (OUTCLKENB_ENABLE << OUTCLKENB_OFFSET), DWMCI_SDMMC0_CLKGEN0L);

	__writel((CLK_OUTPUT_NORMAL << OUTCLKINV0_OFFSET) 
            | (CLKSRCSEL_PLL1 << CLKSRCSEL0_OFFSET) 
            | (CLKDIV0_VALUE << CLKDIV0_OFFSET) 
            | (OUTCLKENB_ENABLE << OUTCLKENB_OFFSET), DWMCI_SDMMC1_CLKGEN0L);

	__writel((CLK_OUTPUT_NORMAL << OUTCLKINV0_OFFSET) 
            | (CLKSRCSEL_PLL1 << CLKSRCSEL0_OFFSET) 
            | (CLKDIV0_VALUE << CLKDIV0_OFFSET) 
            | (OUTCLKENB_ENABLE << OUTCLKENB_OFFSET), DWMCI_SDMMC2_CLKGEN0L);
}

static void dw_mci_clksel(struct dwmci_host *host)
{
	u32 val;

	val = DWMCI_SET_SAMPLE_CLK(DWMCI_SHIFT_0) |
		DWMCI_SET_DRV_CLK(DWMCI_SHIFT_0) | DWMCI_SET_DIV_RATIO(3);

	dwmci_writel(host, DWMCI_CLKSEL, val);
}

static void dw_mci_clk_delay(u32 val, unsigned long regbase)
{	
	__writel(val, (regbase + DWMCI_CLKCTRL));
}

static inline void mmc_setnrst(u32 addr, u32 RSTIndex, int STATUS)
{
	//u32 regNum;
    u32  bitPos, curStat;
//	regNum 		= RSTIndex >> 5;
	curStat		= readl(addr);
	bitPos		= RSTIndex & RESET_ID_MASK;
	curStat		&= ~(1UL << bitPos);
	curStat		|= (STATUS & 0x01) << bitPos;

	__writel(curStat, addr);
}

static void dw_mci_reset(int ch)
{
	int rst_id = RESET_ID_SDMMC0 + ch;

	mmc_setnrst(S5P4418_SYS_IP_RSTCON1, rst_id, RESET);
	mmc_setnrst(S5P4418_SYS_IP_RSTCON1, rst_id, NO_RESET);
}

struct dwmci_host *host = NULL;

static int dw_mci_init(unsigned long regbase, int bus_width, int index, int max_clock, int ddr)
{
	int  fifo_size = 0x20;

	host = malloc(sizeof(struct dwmci_host));
	if (!host) {
		printf("dwmci_host malloc fail!\r\n");
		return ENOMEM;
	}
	memset(host, 0x00, sizeof(*host));

	dw_mci_set_clk(index, max_clock * 4);

	host->name = NXP_NAME;
	host->ioaddr = (void *)regbase;
	host->buswidth = bus_width;
	host->clksel = dw_mci_clksel;
	host->dev_index = index;
	host->get_mmc_clk = dw_mci_get_clk;
	host->fifoth_val = MSIZE(0x2) | RX_WMARK(fifo_size/2 -1) | TX_WMARK(fifo_size/2);

	if(ddr == 1)
		host->caps |= MMC_MODE_DDR_52MHz;// | MMC_MODE_4BIT | MMC_MODE_HS_52MHz ;
    
    //设置接口速度为400KHZ
	add_dwmci(host, max_clock, 400000);

	dw_mci_reset(index);
	return 0;
}

int board_mmc_init(void)
{
	int err = 0;
	int bus = 0, speed = 0,ddr = 0;

	#if(CONFIG_MMC0_ATTACH == TRUE)
	unsigned int value1 = readl(S5P4418_SYS_IP_RSTCON1);
	__writel(value1 | ( NO_RESET << SDMMC0_i_nRST_OFFSET), S5P4418_SYS_IP_RSTCON1);
	#endif

	#if(CONFIG_MMC0_CLOCK)
	speed = CONFIG_MMC0_CLOCK;
	#else
	speed = 52000000;
	#endif

	#if(CONFIG_MMC0_BUS_WIDTH)
	bus = CONFIG_MMC0_BUS_WIDTH;
	#else
	bus = 4;
	#endif

	#if(CONFIG_MMC0_TRANS_MODE)
	ddr = CONFIG_MMC0_TRANS_MODE;
	#else
	ddr = 0;
	#endif
	err = dw_mci_init(SDMMC0_BASEADDR, bus, SDMMC0, speed, ddr);
	#ifdef CONFIG_MMC0_CLK_DELAY
	dw_mci_clk_delay(CONFIG_MMC0_CLK_DELAY, SDMMC0_BASEADDR);
	#endif

	#if(CONFIG_MMC1_CLOCK)
	speed = CONFIG_MMC1_CLOCK;
	#else
	speed = 52000000;
	#endif

	#if(CONFIG_MMC1_BUS_WIDTH)
	bus = CONFIG_MMC1_BUS_WIDTH;
	#else
	bus = 4;
	#endif
	#if(CONFIG_MMC1_TRANS_MODE)
	ddr = CONFIG_MMC1_TRANS_MODE;
	#else
	ddr = 0;
	#endif
	err = dw_mci_init(SDMMC1_BASEADDR, bus, SDMMC1, speed, ddr);
	#ifdef CONFIG_MMC1_CLK_DELAY
	dw_mci_clk_delay(CONFIG_MMC1_CLK_DELAY, SDMMC1_BASEADDR);
	#endif

	#if(CONFIG_MMC2_ATTACH == TRUE)
	unsigned int value2 = readl(S5P4418_SYS_IP_RSTCON1);
	//printf("value2 = %08x\r\n", value2);
	__writel(value2 | (NO_RESET << SDMMC2_i_nRST_OFFSET), S5P4418_SYS_IP_RSTCON1);
	#endif

	#if(CONFIG_MMC2_CLOCK)
	speed = CONFIG_MMC2_CLOCK;
	#else
	speed = 52000000;
	#endif

	#if(CONFIG_MMC2_BUS_WIDTH)
	bus = CONFIG_MMC2_BUS_WIDTH;
	#else
	bus = 4;
	#endif

	#if(CONFIG_MMC2_TRANS_MODE)
	ddr = CONFIG_MMC2_TRANS_MODE;
	#else
	ddr = 0;
	#endif
	err = dw_mci_init(SDMMC2_BASEADDR, bus, SDMMC2, speed, ddr);

	#ifdef CONFIG_MMC2_CLK_DELAY
	dw_mci_clk_delay(CONFIG_MMC2_CLK_DELAY, SDMMC2_BASEADDR);
	#endif

	return err;
}

