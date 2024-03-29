/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Jaehoon Chung <jh80.chung@samsung.com>
 * Rajeshawari Shinde <rajeshwari.s@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <string.h>
#include <errno.h>

#include <s5p4418_serial_stdio.h>
#include <drivers/common.h>
#include <alloc.h>
#include <drivers/mmc.h>
#include <drivers/dwmmc.h>
#include <cache_cp15.h>
#include <cache_v7.h>
#include <timer.h>
#include <s5p4418_tick_delay.h>
#include <config.h>

static const u8 MAX_BLK_CNT = 8;

static int dwmci_wait_reset(struct dwmci_host *host, u32 value)
{
	unsigned long timeout = 1000;
	u32 ctrl;

	dwmci_writel(host, DWMCI_CTRL, value);

	while (timeout--) {
		ctrl = dwmci_readl(host, DWMCI_CTRL);
		if (!(ctrl & DWMCI_RESET_ALL))
			return 1;
	}

	return 0;
}

static void dwmci_set_idma_desc(struct dwmci_idmac *idmac,
		u32 desc0, u32 desc1, u32 desc2)
{
	struct dwmci_idmac *desc = idmac;

	desc->flags = desc0;
	desc->cnt = desc1;
	desc->addr = desc2;
	desc->next_addr = (uintptr_t)desc + sizeof(struct dwmci_idmac);
}


void dwmci_prepare_data(struct dwmci_host *host,
		struct mmc_data *data)
{
	unsigned long ctrl;
	unsigned int i = 0, flags, cnt, blk_cnt;
	//ulong data_start, data_end;
    ulong start_addr;
	ALLOC_CACHE_ALIGN_BUFFER(struct dwmci_idmac, cur_idmac, data->blocks);
	memset(cur_idmac, 0x0, sizeof(struct dwmci_idmac));

	blk_cnt = data->blocks;

	dwmci_wait_reset(host, DWMCI_CTRL_FIFO_RESET);

	//data_start = (ulong)cur_idmac;
	dwmci_writel(host, DWMCI_DBADDR, (uintptr_t)cur_idmac);

	if (data->flags == MMC_DATA_READ)
		start_addr = (uintptr_t)data->dest;
	else
		start_addr = (uintptr_t)data->src;

	do {
		flags = DWMCI_IDMAC_OWN | DWMCI_IDMAC_CH ;
		flags |= (i == 0) ? DWMCI_IDMAC_FS : 0;
		if (blk_cnt <= MAX_BLK_CNT) {
			flags |= DWMCI_IDMAC_LD;
			cnt = data->blocksize * blk_cnt;
		} else
			cnt = data->blocksize * MAX_BLK_CNT;

		dwmci_set_idma_desc(cur_idmac, flags, cnt,
				start_addr + (i * PAGE_SIZE));

		if (blk_cnt <= MAX_BLK_CNT)
			
			break;
		blk_cnt -= MAX_BLK_CNT;
		cur_idmac++;
		i++;
	} while(1);

	//data_end = (ulong)cur_idmac;
	
// #ifdef CONFIG_MMU_ENABLE
 	flush_dcache_all();
// #endif

	ctrl = dwmci_readl(host, DWMCI_CTRL);
	ctrl |= DWMCI_IDMAC_EN | DWMCI_DMA_EN;
	dwmci_writel(host, DWMCI_CTRL, ctrl);

	ctrl = dwmci_readl(host, DWMCI_BMOD);
	ctrl |= DWMCI_BMOD_IDMAC_FB | DWMCI_BMOD_IDMAC_EN;
	dwmci_writel(host, DWMCI_BMOD, ctrl);

	dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
	dwmci_writel(host, DWMCI_BYTCNT, data->blocksize * data->blocks);
}

static int dwmci_set_transfer_mode(struct dwmci_host *host,
		struct mmc_data *data)
{
    UNUSED(host);
	unsigned long mode;

	mode = DWMCI_CMD_DATA_EXP;
	if (data->flags & MMC_DATA_WRITE)
		mode |= DWMCI_CMD_RW;

	return mode;
}

static int dwmci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
		struct mmc_data *data)
{
	struct dwmci_host *host = mmc->priv;
	int flags = 0;
    u32 i;
	u32 timeout = 100000;
	u32 retry = 10000;
	u32 mask, ctrl;
	ulong start = get_timer(0);

	while (dwmci_readl(host, DWMCI_STATUS) & DWMCI_BUSY) {
		if (get_timer(start) > timeout) {
			return TIMEOUT;
		}
	}

	dwmci_writel(host, DWMCI_RINTSTS, DWMCI_INTMSK_ALL);

	if (data)
		dwmci_prepare_data(host, data);

	dwmci_writel(host, DWMCI_CMDARG, cmd->cmdarg);

	if (data)
		flags = dwmci_set_transfer_mode(host, data);

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
		return -1;

	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		flags |= DWMCI_CMD_ABORT_STOP;
	else
		flags |= DWMCI_CMD_PRV_DAT_WAIT;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		flags |= DWMCI_CMD_RESP_EXP;
		if (cmd->resp_type & MMC_RSP_136)
			flags |= DWMCI_CMD_RESP_LENGTH;
	}

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= DWMCI_CMD_CHECK_CRC;

	flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG);

	dwmci_writel(host, DWMCI_CMD, flags);

	for (i = 0; i < retry; i++) {
		mask = dwmci_readl(host, DWMCI_RINTSTS);
		if (mask & DWMCI_INTMSK_CDONE) {
			if (!data)
				dwmci_writel(host, DWMCI_RINTSTS, mask);
			break;
		}
	}

	if (i == retry)
		return TIMEOUT;

	if (mask & DWMCI_INTMSK_RTO) {
		return TIMEOUT;
	} else if (mask & DWMCI_INTMSK_RE) {
		return -1;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP3);
			cmd->response[1] = dwmci_readl(host, DWMCI_RESP2);
			cmd->response[2] = dwmci_readl(host, DWMCI_RESP1);
			cmd->response[3] = dwmci_readl(host, DWMCI_RESP0);
		} else {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP0);
		}
	}

	if (data) {
		do {
			mask = dwmci_readl(host, DWMCI_RINTSTS);
			if (mask & (DWMCI_DATA_ERR | DWMCI_DATA_TOUT)) {
				return -1;
			}

			
		} while (!(mask & DWMCI_INTMSK_DTO));

		dwmci_writel(host, DWMCI_RINTSTS, mask);

		ctrl = dwmci_readl(host, DWMCI_CTRL);
		ctrl &= ~(DWMCI_DMA_EN);
		dwmci_writel(host, DWMCI_CTRL, ctrl);
	}

	udelay(100);

	return 0;
}

static int dwmci_setup_bus(struct dwmci_host *host, u32 freq)
{
	u32 div, status;
	int timeout = 10000;
	unsigned long sclk;

	if ((freq == host->clock) || (freq == 0))
		return 0;
	/*
	 * If host->get_mmc_clk didn't define,
	 * then assume that host->bus_hz is source clock value.
	 * host->bus_hz should be set from user.
	 */
	if (host->get_mmc_clk)
		sclk = host->get_mmc_clk(host);
	else if (host->bus_hz)
		sclk = host->bus_hz;
	else {
		return -EINVAL;
	}

	//div = DIV_ROUND_UP(sclk, 2 * freq);
	div = (sclk / (2*freq));

	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

	dwmci_writel(host, DWMCI_CLKDIV, div);
	dwmci_writel(host, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout-- < 0) {
			return -1;
		}
	} while (status & DWMCI_CMD_START);

	dwmci_writel(host, DWMCI_CLKENA, DWMCI_CLKEN_ENABLE |
			DWMCI_CLKEN_LOW_PWR);

	dwmci_writel(host, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout-- < 0) {
			return -1;
		}
	} while (status & DWMCI_CMD_START);

	host->clock = freq;

	return 0;
}

static void dwmci_set_ios(struct mmc *mmc)
{
	struct dwmci_host *host = (struct dwmci_host *)mmc->priv;
	u32 ctype, regs;

	dwmci_setup_bus(host, mmc->clock);
	switch (mmc->bus_width) {
	case 8:
		ctype = DWMCI_CTYPE_8BIT;
		break;
	case 4:
		ctype = DWMCI_CTYPE_4BIT;
		break;
	default:
		ctype = DWMCI_CTYPE_1BIT;
		break;
	}

	dwmci_writel(host, DWMCI_CTYPE, ctype);

	regs = dwmci_readl(host, DWMCI_UHS_REG);
	if (mmc->card_caps & MMC_MODE_DDR_52MHz)
		regs |= DWMCI_DDR_MODE;
	else
		regs &= DWMCI_DDR_MODE;

	dwmci_writel(host, DWMCI_UHS_REG, regs);

	if (host->clksel)
		host->clksel(host);
}

static int dwmci_init(struct mmc *mmc)
{
	struct dwmci_host *host = mmc->priv;

	if (host->board_init)
	{
		host->board_init(host);
	}
	else{
	}

	dwmci_writel(host, DWMCI_PWREN, 1);

	if (!dwmci_wait_reset(host, DWMCI_RESET_ALL)) {
		return -1;
	}

	/* Enumerate at 400KHz */
	dwmci_setup_bus(host, mmc->cfg->f_min);

	dwmci_writel(host, DWMCI_RINTSTS, 0xFFFFFFFF);
	dwmci_writel(host, DWMCI_INTMASK, 0);

	dwmci_writel(host, DWMCI_TMOUT, 0xFFFFFFFF);

	dwmci_writel(host, DWMCI_IDINTEN, 0);
	dwmci_writel(host, DWMCI_BMOD, 1);

	if (host->fifoth_val) {
		dwmci_writel(host, DWMCI_FIFOTH, host->fifoth_val);
	}

	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

	return 0;
}

static const struct mmc_ops dwmci_ops = {
	.send_cmd	= dwmci_send_cmd,
	.set_ios	= dwmci_set_ios,
	.init		= dwmci_init,
};

int add_dwmci(struct dwmci_host *host, u32 max_clk, u32 min_clk)
{
	host->cfg.name = host->name;
	host->cfg.ops = &dwmci_ops;
	host->cfg.f_min = min_clk;
	host->cfg.f_max = max_clk;

	host->cfg.voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

	host->cfg.host_caps = host->caps;

	if (host->buswidth == 8) {
		host->cfg.host_caps |= MMC_MODE_8BIT;
		host->cfg.host_caps &= ~MMC_MODE_4BIT;
	} else {
		host->cfg.host_caps |= MMC_MODE_4BIT;
		host->cfg.host_caps &= ~MMC_MODE_8BIT;
	}
	host->cfg.host_caps |= MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_HC;

	host->cfg.b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	host->mmc = mmc_create(&host->cfg, host);
	if (host->mmc == NULL)
		return -1;
	
	return 0;
}

