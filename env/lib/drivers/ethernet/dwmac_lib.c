/*******************************************************************************
  Copyright (C) 2007-2009  STMicroelectronics Ltd

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#include "drivers/ethernet/eth_common.h"
#include "drivers/ethernet/dwmac_dma.h"
#include "io.h"
#include <stdio.h>

#define GMAC_HI_REG_AE		0x80000000

/* add by jhkim : prevent rx overflow */
void dwmac_enable_dma_receive(void *ioaddr)
{
	__writel(1, ioaddr + DMA_RCV_POLL_DEMAND);
}

/* CSR1 enables the transmit DMA to check for new descriptor */
void dwmac_enable_dma_transmission(void *ioaddr)
{
	__writel(1, ioaddr + DMA_XMT_POLL_DEMAND);
}

void dwmac_enable_dma_irq(void *ioaddr)
{
	__writel(DMA_INTR_DEFAULT_MASK, ioaddr + DMA_INTR_ENA);
}

void dwmac_disable_dma_irq(void *ioaddr)
{
	__writel(0, ioaddr + DMA_INTR_ENA);
}

void dwmac_dma_start_tx(void *ioaddr)
{
	u32 value = readl(ioaddr + DMA_CONTROL);
	value |= DMA_CONTROL_ST;
	__writel(value, ioaddr + DMA_CONTROL);
}

void dwmac_dma_stop_tx(void *ioaddr)
{
	u32 value = readl(ioaddr + DMA_CONTROL);
	value &= ~DMA_CONTROL_ST;
	__writel(value, ioaddr + DMA_CONTROL);
}

void dwmac_dma_start_rx(void *ioaddr)
{
	u32 value = readl(ioaddr + DMA_CONTROL);
	value |= DMA_CONTROL_SR;
	__writel(value, ioaddr + DMA_CONTROL);
}

void dwmac_dma_stop_rx(void *ioaddr)
{
	u32 value = readl(ioaddr + DMA_CONTROL);
	value &= ~DMA_CONTROL_SR;
	__writel(value, ioaddr + DMA_CONTROL);
}

int dwmac_dma_interrupt(void *ioaddr,
			struct stmmac_extra_stats *x)
{
	int ret = 0;
	/* read the status register (CSR5) */
	u32 intr_status = readl(ioaddr + DMA_STATUS);

#ifdef DWMAC_DMA_DEBUG
	/* Enable it to monitor DMA rx/tx status in case of critical problems */
	show_tx_process_state(intr_status);
	show_rx_process_state(intr_status);
#endif
	/* ABNORMAL interrupts */
	if (intr_status & DMA_STATUS_AIS) {
		if (intr_status & DMA_STATUS_UNF) {
			ret = dwmac_tx_hard_error_bump_tc;
			x->tx_undeflow_irq++;
		}
		if (intr_status & DMA_STATUS_TJT)
			x->tx_jabber_irq++;

		if (intr_status & DMA_STATUS_OVF)
			x->rx_overflow_irq++;

		if (intr_status & DMA_STATUS_RU)
			x->rx_buf_unav_irq++;
		if (intr_status & DMA_STATUS_RPS)
			x->rx_process_stopped_irq++;
		if (intr_status & DMA_STATUS_RWT)
			x->rx_watchdog_irq++;
		if (intr_status & DMA_STATUS_ETI)
			x->tx_early_irq++;
		if (intr_status & DMA_STATUS_TPS) {
			x->tx_process_stopped_irq++;
			ret = dwmac_tx_hard_error;
		}
		if (intr_status & DMA_STATUS_FBI) {
			x->fatal_bus_error_irq++;
			ret = dwmac_tx_hard_error;
		}
	}
	/* TX/RX NORMAL interrupts */
	if (intr_status & DMA_STATUS_NIS) {
		x->normal_irq_n++;
		if (intr_status & DMA_STATUS_RI) {
			u32 value = readl(ioaddr + DMA_INTR_ENA);
			/* to schedule NAPI on real RIE event. */
			if (value & DMA_INTR_ENA_RIE) {
				x->rx_normal_irq_n++;
				ret |= dwmac_handle_rx;
			}
		}
		if (intr_status & DMA_STATUS_TI) {
			x->tx_normal_irq_n++;
			ret |= dwmac_handle_tx;
		}
		if (intr_status & DMA_STATUS_ERI)
			x->rx_early_irq++;
	}
	/* Optional hardware blocks, interrupts should be disabled */
	if (intr_status &
		     (DMA_STATUS_GPI | DMA_STATUS_GMI | DMA_STATUS_GLI))
	{
		printf("%s: unexpected status %08x\r\n", __func__, intr_status);

	}

	/* Clear the interrupt by writing a logic 1 to the CSR5[15-0] */
	__writel((intr_status & 0x1ffff), ioaddr + DMA_STATUS);

	return ret;
}

void dwmac_dma_flush_tx_fifo(void *ioaddr)
{
	u32 csr6 = readl(ioaddr + DMA_CONTROL);
	__writel((csr6 | DMA_CONTROL_FTF), ioaddr + DMA_CONTROL);

	do {} while ((readl(ioaddr + DMA_CONTROL) & DMA_CONTROL_FTF));
}

void stmmac_set_mac_addr(void *ioaddr, u8 addr[6],
			 unsigned int high, unsigned int low)
{
	unsigned long data;

	data = (addr[5] << 8) | addr[4];
	/* For MAC Addr registers se have to set the Address Enable (AE)
	 * bit that has no effect on the High Reg 0 where the bit 31 (MO)
	 * is RO.
	 */
	__writel(data | GMAC_HI_REG_AE, ioaddr + high);
	data = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0];
	__writel(data, ioaddr + low);
}

/* Enable disable MAC RX/TX */
void stmmac_set_mac(void *ioaddr, boolean enable)
{
	u32 value = readl(ioaddr + MAC_CTRL_REG);

	if (enable)
		value |= MAC_RNABLE_RX | MAC_ENABLE_TX;
	else
		value &= ~(MAC_ENABLE_TX | MAC_RNABLE_RX);

	__writel(value, ioaddr + MAC_CTRL_REG);
}

void stmmac_get_mac_addr(void *ioaddr, unsigned char *addr,
			 unsigned int high, unsigned int low)
{
	unsigned int hi_addr, lo_addr;

	/* Read the MAC address from the hardware */
	hi_addr = readl(ioaddr + high);
	lo_addr = readl(ioaddr + low);

	/* Extract the MAC address from the high and low words */
	addr[0] = lo_addr & 0xff;
	addr[1] = (lo_addr >> 8) & 0xff;
	addr[2] = (lo_addr >> 16) & 0xff;
	addr[3] = (lo_addr >> 24) & 0xff;
	addr[4] = hi_addr & 0xff;
	addr[5] = (hi_addr >> 8) & 0xff;
}

