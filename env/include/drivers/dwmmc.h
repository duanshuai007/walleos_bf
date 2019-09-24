/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Jaehoon Chung <jh80.chung@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ENV_INCLUDE_DRIVERS_DWMMC_H_
#define _ENV_INCLUDE_DRIVERS_DWMMC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <io.h>
#include <drivers/mmc.h>
#include <nxp/nx_chip.h>

enum dwmci_reg{
    DWMCI_CTRL		 =   0x000,
    DWMCI_PWREN		 =   0x004,
    DWMCI_CLKDIV	 =   0x008,
    DWMCI_CLKSRC	 =   0x00C,
    DWMCI_CLKENA	 =   0x010,
    DWMCI_TMOUT		 =   0x014,
    DWMCI_CTYPE		 =   0x018,
    DWMCI_BLKSIZ	 =   0x01C,
    DWMCI_BYTCNT	 =   0x020,
    DWMCI_INTMASK	 =   0x024,
    DWMCI_CMDARG	 =   0x028,
    DWMCI_CMD		 =   0x02C,
    DWMCI_RESP0		 =   0x030,
    DWMCI_RESP1		 =   0x034,
    DWMCI_RESP2		 =   0x038,
    DWMCI_RESP3		 =   0x03C,
    DWMCI_MINTSTS	 =   0x040,
    DWMCI_RINTSTS	 =   0x044,
    DWMCI_STATUS	 =   0x048,
    DWMCI_FIFOTH	 =   0x04C,
    DWMCI_CDETECT	 =   0x050,
    DWMCI_WRTPRT	 =   0x054,
    DWMCI_GPIO		 =   0x058,
    DWMCI_TCMCNT	 =   0x05C,
    DWMCI_TBBCNT	 =   0x060,
    DWMCI_DEBNCE	 =   0x064,
    DWMCI_USRID		 =   0x068,
    DWMCI_VERID		 =   0x06C,
    DWMCI_HCON		 =   0x070,
    DWMCI_UHS_REG	 =   0x074,
    DWMCI_BMOD		 =   0x080,
    DWMCI_PLDMND	 =   0x084,
    DWMCI_DBADDR	 =   0x088,
    DWMCI_IDSTS		 =   0x08C,
    DWMCI_IDINTEN	 =   0x090,
    DWMCI_DSCADDR	 =   0x094,
    DWMCI_BUFADDR	 =   0x098,
    DWMCI_CLKSEL     =   0x09C,
    DWMCI_CLKCTRL    =   0x114,
    DWMCI_DATA		 =   0x200,
};

/* Interrupt Mask register */
enum dwmci_intmask{
    DWMCI_INTMSK_ALL	    = 0xffffffff,
    DWMCI_INTMSK_RE		    = 1 << 1,
    DWMCI_INTMSK_CDONE	    = 1 << 2,
    DWMCI_INTMSK_DTO	    = 1 << 3,
    DWMCI_INTMSK_TXDR	    = 1 << 4,
    DWMCI_INTMSK_RXDR	    = 1 << 5,
    DWMCI_INTMSK_DCRC	    = 1 << 7,
    DWMCI_INTMSK_RTO	    = 1 << 8,
    DWMCI_INTMSK_DRTO	    = 1 << 9,
    DWMCI_INTMSK_HTO	    = 1 << 10,
    DWMCI_INTMSK_FRUN	    = 1 << 11,
    DWMCI_INTMSK_HLE	    = 1 << 12,
    DWMCI_INTMSK_SBE	    = 1 << 13,
    DWMCI_INTMSK_ACD	    = 1 << 14,
    DWMCI_INTMSK_EBE        = 1 << 15,
    DWMCI_DATA_ERR          = ((1<<15)|(1<<13)|1<<12)|(1<<11)|(1<<7),
    DWMCI_DATA_TOUT         = ((1<<10)|(1<<9)),
};

/* Raw interrupt Regsiter */
#if 0
#define DWMCI_DATA_ERR	(DWMCI_INTMSK_EBE | DWMCI_INTMSK_SBE | DWMCI_INTMSK_HLE |\
			DWMCI_INTMSK_FRUN | DWMCI_INTMSK_EBE | DWMCI_INTMSK_DCRC)
#define DWMCI_DATA_TOUT	(DWMCI_INTMSK_HTO | DWMCI_INTMSK_DRTO)
#endif

/* CTRL register */
enum dwmci_ctrl{
    DWMCI_CTRL_RESET	    = 0x1,
    DWMCI_CTRL_FIFO_RESET	= 0x2,
    DWMCI_CTRL_DMA_RESET	= 0x4,
    DWMCI_DMA_EN		    = 1 << 5,
    DWMCI_CTRL_SEND_AS_CCSD	= 1 << 10,
    DWMCI_IDMAC_EN		    = 1 << 25,
    DWMCI_RESET_ALL         = 0x7,
};
/* CMD register */
enum dwmci_cmd{
    DWMCI_CMD_RESP_EXP	    = 1 << 6,
    DWMCI_CMD_RESP_LENGTH	= 1 << 7,
    DWMCI_CMD_CHECK_CRC	    = 1 << 8,
    DWMCI_CMD_DATA_EXP	    = 1 << 9,
    DWMCI_CMD_RW		    = 1 << 10,
    DWMCI_CMD_SEND_STOP	    = 1 << 12,
    DWMCI_CMD_ABORT_STOP	= 1 << 14,
    DWMCI_CMD_PRV_DAT_WAIT	= 1 << 13,
    DWMCI_CMD_UPD_CLK	    = 1 << 21,
    DWMCI_CMD_USE_HOLD_REG	= 1 << 29,
    DWMCI_CMD_START		    = 1 << 31,
};

/* CLKENA register */
enum dwmci_clken{
    DWMCI_CLKEN_ENABLE	    = 1 << 0,
    DWMCI_CLKEN_LOW_PWR	    = 1 << 16,
};

/* Card-type registe */
enum dwmci_cardtype{
    DWMCI_CTYPE_1BIT    = 0,
    DWMCI_CTYPE_4BIT	= 1 << 0,
    DWMCI_CTYPE_8BIT	= 1 << 16,
};

enum dwmci_misc{
/* Status Register */
    DWMCI_BUSY      = (1 << 9),
    RX_WMARK_SHIFT  = 16,
    RX_WMARK_MASK   = 0xfff << 16,
    DWMCI_IDMAC_OWN = 1<<31,
    DWMCI_IDMAC_CH  = 1<<4,
    DWMCI_IDMAC_FS  = 1<<3,
    DWMCI_IDMAC_LD  = 1<<2,
    DWMCI_BMOD_IDMAC_FB     = 1<<1,
    DWMCI_BMOD_IDMAC_RESET  = 1<<0,
    DWMCI_BMOD_IDMAC_EN     = 1<<7,
    DWMCI_DDR_MODE          = 1<<16,
    DWMCI_QUIRK_DISABLE_SMU = 1<< 0,
};

/* FIFOTH Register */
#define MSIZE(x)		((x) << 28)
#define RX_WMARK(x)		((x) << 16)
#define TX_WMARK(x)		(x)

/* CLK DELAY SHIFT Register*/
#define DW_MMC_DRIVE_DELAY(n)       ((n & 0xFF) << 0)   // write
#define DW_MMC_DRIVE_PHASE(n)       ((n & 0x03) <<16)   // write
#define DW_MMC_SAMPLE_DELAY(n)      ((n & 0xFF) << 8)   // read
#define DW_MMC_SAMPLE_PHASE(n)      ((n & 0x03) <<24)   // read/* CLK DELAY SHIFT Register*/
enum dwmci_shift{
    DWMCI_SHIFT_0        =   0x0,
    DWMCI_SHIFT_1        =   0x1,
    DWMCI_SHIFT_2        =   0x2,
    DWMCI_SHIFT_3        =   0x3,
};

#define DWMCI_SET_SAMPLE_CLK(x) (x)
#define DWMCI_SET_DRV_CLK(x)    ((x) << 16)
#define DWMCI_SET_DIV_RATIO(x)  ((x) << 24)

enum sdmmc_clkenb{
    DWMCI_SDMMC0_CLKENB = 0xC00C5000,
    DWMCI_SDMMC1_CLKENB = 0xC00CC000,
    DWMCI_SDMMC2_CLKENB = 0xC00CD000,
};

enum sdmmc_clkgen0l{
    DWMCI_SDMMC0_CLKGEN0L = 0xC00C5004,
    DWMCI_SDMMC1_CLKGEN0L = 0xC00CC004,
    DWMCI_SDMMC2_CLKGEN0L = 0xC00CD004,
};
/*
*   SDMMCCLKGEN0L 
*/
enum sdmmc_ctrl_reg{
    CLKGENENB_OFFSET = 2,
    CLKGENENB_ENABLE = 3,
    OUTCLKINV0_OFFSET = 1,
    CLKSRCSEL0_OFFSET = 2,
    CLKDIV0_OFFSET = 5,
    OUTCLKENB_OFFSET = 15,
    CLK_OUTPUT_NORMAL = 0,
    CLK_OUTPUT_INVERT = 1,
    CLKSRCSEL_PLL0 = 0,
    CLKSRCSEL_PLL1 = 1,
    CLKSRCSEL_PLL2 = 2,
    OUTCLKENB_ENABLE = 0,
    OUTCLKENB_RESERVED = 1,
};
/*
*   SDMMC BASEADDR
*/
enum sdmmc_base{
    SDMMC0_BASEADDR = 0xC0062000,
    SDMMC1_BASEADDR = 0xC0068000,
    SDMMC2_BASEADDR = 0xC0069000,
};

enum sdmmc_dev{
    SDMMC0 = 0,
    SDMMC1,
    SDMMC2,
};

struct dwmci_host {
	char *name;
	void *ioaddr;
	unsigned int quirks;
	unsigned int caps;
	unsigned int version;
	unsigned int clock;
	unsigned int bus_hz;
	unsigned int div;
	int dev_index;
	int dev_id;
	int buswidth;
	u32 clksel_val;
	u32 fifoth_val;
	struct mmc *mmc;

	void (*clksel)(struct dwmci_host *host);
	void (*board_init)(struct dwmci_host *host);
	unsigned int (*get_mmc_clk)(struct dwmci_host *host);

	struct mmc_config cfg;
};

struct dwmci_idmac {
	u32 flags;
	u32 cnt;
	u32 addr;
	u32 next_addr;
};

static inline void dwmci_writel(struct dwmci_host *host, int reg, u32 val)
{
	__writel(val, host->ioaddr + reg);
}

static inline void dwmci_writew(struct dwmci_host *host, int reg, u16 val)
{
	__writew(val, host->ioaddr + reg);
}

static inline void dwmci_writeb(struct dwmci_host *host, int reg, u8 val)
{
	__writeb(val, host->ioaddr + reg);
}
static inline u32 dwmci_readl(struct dwmci_host *host, int reg)
{
	int value = readl(host->ioaddr + reg);
	return value;
}

static inline u16 dwmci_readw(struct dwmci_host *host, int reg)
{
	int value = readw(host->ioaddr + reg);
	return value;
}

static inline u8 dwmci_readb(struct dwmci_host *host, int reg)
{
	int value = readb(host->ioaddr + reg);
	return value;
}

int add_dwmci(struct dwmci_host *host, u32 max_clk, u32 min_clk);

#ifdef __cplusplus
}
#endif

#endif	/* __DWMMC_HW_H */
