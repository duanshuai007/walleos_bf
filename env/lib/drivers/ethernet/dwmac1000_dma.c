/*******************************************************************************
  This is the driver for the GMAC on-chip Ethernet controller for ST SoCs.
  DWC Ether MAC 10/100/1000 Universal version 3.41a  has been used for
  developing this code.

  This contains the functions to handle the dma.

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

#include "drivers/ethernet/dwmac1000.h"
#include "drivers/ethernet/dwmac_dma.h"
#include "io.h"
#include "s5p4418_tick_delay.h"
#include <errno.h>
/*
#define pr_debug printf
*/

int dwmac1000_dma_init(void *ioaddr, int pbl, int fb, int mb,
			      int burst_len, u32 dma_tx, u32 dma_rx, int atds)
{

	u32 value = readl(ioaddr + DMA_BUS_MODE);
	
	int limit;
	/* DMA SW reset */
	value |= DMA_BUS_MODE_SFT_RESET;
	__writel(value, ioaddr + DMA_BUS_MODE);
	limit = 10;
	while (limit--) {
		if (!(readl(ioaddr + DMA_BUS_MODE) & DMA_BUS_MODE_SFT_RESET))
			break;
		udelay(10);
	}
	if (limit < 0)
		return -EBUSY;

	/*
	 * Set the DMA PBL (Programmable Burst Length) mode
	 * Before stmmac core 3.50 this mode bit was 4xPBL, and
	 * post 3.5 mode bit acts as 8*PBL.
	 * For core rev < 3.5, when the core is set for 4xPBL mode, the
	 * DMA transfers the data in 4, 8, 16, 32, 64 & 128 beats
	 * depending on pbl value.
	 * For core rev > 3.5, when the core is set for 8xPBL mode, the
	 * DMA transfers the data in 8, 16, 32, 64, 128 & 256 beats
	 * depending on pbl value.
	 */
	value = DMA_BUS_MODE_PBL | ((pbl << DMA_BUS_MODE_PBL_SHIFT) |
				    (pbl << DMA_BUS_MODE_RPBL_SHIFT));

	/* Set the Fixed burst mode */
	if (fb)
		value |= DMA_BUS_MODE_FB;

	/* Mixed Burst has no effect when fb is set */
	if (mb)
		value |= DMA_BUS_MODE_MB;

#ifdef CONFIG_NXPMAC_DA
	value |= DMA_BUS_MODE_DA;	/* Rx has priority over tx */

#endif

	if (atds)
		value |= DMA_BUS_MODE_ATDS;

	__writel(value, ioaddr + DMA_BUS_MODE);

	/* In case of GMAC AXI configuration, program the DMA_AXI_BUS_MODE
	 * for supported bursts.
	 *
	 * Note: This is applicable only for revision GMACv3.61a. For
	 * older version this register is reserved and shall have no
	 * effect.
	 *
	 * Note:
	 *  For Fixed Burst Mode: if we directly write 0xFF to this
	 *  register using the configurations pass from platform code,
	 *  this would ensure that all bursts supported by core are set
	 *  and those which are not supported would remain ineffective.
	 *
	 *  For Non Fixed Burst Mode: provide the maximum value of the
	 *  burst length. Any burst equal or below the provided burst
	 *  length would be allowed to perform.
	 */
	__writel(burst_len, ioaddr + DMA_AXI_BUS_MODE);


	/* Mask interrupts by writing to CSR7 */
	__writel(DMA_INTR_DEFAULT_MASK, ioaddr + DMA_INTR_ENA);

	/* RX/TX descriptor base address lists must be written into
	 * DMA CSR3 and CSR4, respectively
	 */


	__writel(dma_tx, ioaddr + DMA_TX_BASE_ADDR);

	__writel(dma_rx, ioaddr + DMA_RCV_BASE_ADDR);

	return 0;
}

void dwmac1000_dma_operation_mode(void *ioaddr, int txmode,
					 int rxmode)
{
	u32 csr6 = readl(ioaddr + DMA_CONTROL);

	if (txmode == SF_DMA_MODE) {
		/* Transmit COE type 2 cannot be done in cut-through mode. */
		csr6 |= DMA_CONTROL_TSF;
		/* Operating on second frame increase the performance
		 * especially when transmit store-and-forward is used.
		 */
		csr6 |= DMA_CONTROL_OSF;
	} else {
		csr6 &= ~DMA_CONTROL_TSF;
		csr6 &= DMA_CONTROL_TC_TX_MASK;
		/* Set the transmit threshold */
		if (txmode <= 32)
			csr6 |= DMA_CONTROL_TTC_32;
		else if (txmode <= 64)
			csr6 |= DMA_CONTROL_TTC_64;
		else if (txmode <= 128)
			csr6 |= DMA_CONTROL_TTC_128;
		else if (txmode <= 192)
			csr6 |= DMA_CONTROL_TTC_192;
		else
			csr6 |= DMA_CONTROL_TTC_256;
	}

	if (rxmode == SF_DMA_MODE) {
		csr6 |= DMA_CONTROL_RSF;
	} else {
		csr6 &= ~DMA_CONTROL_RSF;
		csr6 &= DMA_CONTROL_TC_RX_MASK;
		if (rxmode <= 32)
			csr6 |= DMA_CONTROL_RTC_32;
		else if (rxmode <= 64)
			csr6 |= DMA_CONTROL_RTC_64;
		else if (rxmode <= 96)
			csr6 |= DMA_CONTROL_RTC_96;
		else
			csr6 |= DMA_CONTROL_RTC_128;
	}

	__writel(csr6, ioaddr + DMA_CONTROL);




}

unsigned int dwmac1000_get_hw_feature(void *ioaddr)
{
	unsigned int res = readl(ioaddr + DMA_HW_FEATURE);

	return res;

}

void dwmac1000_rx_watchdog(void *ioaddr, u32 riwt)
{
	__writel(riwt, ioaddr + DMA_RX_WATCHDOG);
}

