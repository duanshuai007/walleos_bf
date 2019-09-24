/*******************************************************************************
  This is the driver for the GMAC on-chip Ethernet controller for ST SoCs.
  DWC Ether MAC 10/100/1000 Universal version 3.41a  has been used for
  developing this code.

  This only implements the mac core functions for this chip.

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
#include "io.h"
#include <stdio.h>

void dwmac1000_core_init(void *ioaddr, int mtu)
{
	u32 value = readl(ioaddr + GMAC_CONTROL);

	value |= GMAC_CORE_INIT;
	if (mtu > 1500)
		value |= GMAC_CONTROL_2K;
	if (mtu > 2000)
		value |= GMAC_CONTROL_JE;


	__writel(value, ioaddr + GMAC_CONTROL);

	/* Mask GMAC interrupts */
	__writel(0x207, ioaddr + GMAC_INT_MASK);

}

int dwmac1000_rx_ipc_enable(void *ioaddr)
{
	u32 value = readl(ioaddr + GMAC_CONTROL);

	value |= GMAC_CONTROL_IPC;

	__writel(value, ioaddr + GMAC_CONTROL);

	value = readl(ioaddr + GMAC_CONTROL);

	return !!(value & GMAC_CONTROL_IPC);
}


void dwmac1000_set_umac_addr(void *ioaddr, unsigned char *addr,
				    unsigned int reg_n)
{
	stmmac_set_mac_addr(ioaddr, addr, GMAC_ADDR_HIGH(reg_n),
			    GMAC_ADDR_LOW(reg_n));

}

void dwmac1000_get_umac_addr(void *ioaddr, unsigned char *addr,
				    unsigned int reg_n)
{

	stmmac_get_mac_addr(ioaddr, addr, GMAC_ADDR_HIGH(reg_n),
			    GMAC_ADDR_LOW(reg_n));

}

void dwmac1000_set_filter(void *ioaddr)
{
	unsigned int value = 0;

	/* Enable Receive all mode (to debug filtering_fail errors) */
	value |= GMAC_FRAME_FILTER_RA;

	__writel(0x02, ioaddr + GMAC_HASH_LOW);
	__writel(0x01, ioaddr + GMAC_HASH_HIGH);
	__writel(0x04, ioaddr + GMAC_FRAME_FILTER);

	// printf("\tFilter: 0x%08x\r\n\tHash: HI 0x%08x, LO 0x%08x\r\n",
	// 	 readl(ioaddr + GMAC_FRAME_FILTER),
	// 	 readl(ioaddr + GMAC_HASH_HIGH), readl(ioaddr + GMAC_HASH_LOW));



}

void dwmac1000_flow_ctrl(void *ioaddr, unsigned int duplex,
				unsigned int fc, unsigned int pause_time)
{
	unsigned int flow = 0;


	if (fc & FLOW_RX) {
		flow |= GMAC_FLOW_CTRL_RFE;
	}
	if (fc & FLOW_TX) {
		flow |= GMAC_FLOW_CTRL_TFE;
	}

	if (duplex) {
		flow |= (pause_time << GMAC_FLOW_CTRL_PT_SHIFT);

	}

	__writel(flow, ioaddr + GMAC_FLOW_CTRL);


}

void dwmac1000_pmt(void *ioaddr, unsigned long mode)
{

	unsigned int pmt = 0;

	if (mode & WAKE_MAGIC) {
		pmt |= power_down | magic_pkt_en;

	}
	if (mode & WAKE_UCAST) {
		pmt |= global_unicast;

	}

	__writel(pmt, ioaddr + GMAC_PMT);
}

int dwmac1000_irq_status(void *ioaddr,
				struct stmmac_extra_stats *x)
{

	u32 intr_status = readl(ioaddr + GMAC_INT_STATUS);


	int ret = 0;

	/* Not used events (e.g. MMC interrupts) are not handled. */
	if ((intr_status & mmc_tx_irq))
		x->mmc_tx_irq_n++;

	if (intr_status & mmc_rx_irq)
		x->mmc_rx_irq_n++;

	if (intr_status & mmc_rx_csum_offload_irq)
		x->mmc_rx_csum_offload_irq_n++;

	if (intr_status & pmt_irq) {
		/* clear the PMT bits 5 and 6 by reading the PMT status reg */
		readl(ioaddr + GMAC_PMT);

		x->irq_receive_pmt_irq_n++;

	}
	/* MAC trx/rx EEE LPI entry/exit interrupts */
	if (intr_status & lpiis_irq) {
		/* Clean LPI interrupt by reading the Reg 12 */
		ret = readl(ioaddr + LPI_CTRL_STATUS);

		if (ret & LPI_CTRL_STATUS_TLPIEN)
			x->irq_tx_path_in_lpi_mode_n++;

		if (ret & LPI_CTRL_STATUS_TLPIEX)
			x->irq_tx_path_exit_lpi_mode_n++;


		if (ret & LPI_CTRL_STATUS_RLPIEN)
			x->irq_rx_path_in_lpi_mode_n++;


		if (ret & LPI_CTRL_STATUS_RLPIEX)
			x->irq_rx_path_exit_lpi_mode_n++;

	}

	if ((intr_status & pcs_ane_irq) || (intr_status & pcs_link_irq)) {
		readl(ioaddr + GMAC_AN_STATUS);
		x->irq_pcs_ane_n++;
	}


	if (intr_status & rgmii_irq) {
		u32 status = readl(ioaddr + GMAC_S_R_GMII);

		x->irq_rgmii_n++;

		/* Save and dump the link status. */
		if (status & GMAC_S_R_GMII_LINK) {
			int speed_value = (status & GMAC_S_R_GMII_SPEED) >>
			    GMAC_S_R_GMII_SPEED_SHIFT;
			x->pcs_duplex = (status & GMAC_S_R_GMII_MODE);

			if (speed_value == GMAC_S_R_GMII_SPEED_125)
				x->pcs_speed = SPEED_1000;
			else if (speed_value == GMAC_S_R_GMII_SPEED_25)
				x->pcs_speed = SPEED_100;
			else
				x->pcs_speed = SPEED_10;

			x->pcs_link = 1;
			// printf("%s: Link is Up - %d/%s\r\n", __func__,
			// 	 (int)x->pcs_speed,
			// 	 x->pcs_duplex ? "Full" : "Half");
		} else {
			x->pcs_link = 0;
//			printf("%s: Link is Down\r\n", __func__);
		}
	}

	return ret;
}

void dwmac1000_set_eee_mode(void *ioaddr)
{
	u32 value;

	/* Enable the link status receive on RGMII, SGMII ore SMII
	 * receive path and instruct the transmit to enter in LPI
	 * state.
	 */
	value = readl(ioaddr + LPI_CTRL_STATUS);

	value |= LPI_CTRL_STATUS_LPIEN | LPI_CTRL_STATUS_LPITXA;

	__writel(value, ioaddr + LPI_CTRL_STATUS);
}

void dwmac1000_reset_eee_mode(void *ioaddr)
{
	u32 value;

	value = readl(ioaddr + LPI_CTRL_STATUS);

	value &= ~(LPI_CTRL_STATUS_LPIEN | LPI_CTRL_STATUS_LPITXA);
	__writel(value, ioaddr + LPI_CTRL_STATUS);
}

void dwmac1000_set_eee_pls(void *ioaddr, int link)
{
	u32 value;

	value = readl(ioaddr + LPI_CTRL_STATUS);

	if (link)
		value |= LPI_CTRL_STATUS_PLS;
	else
		value &= ~LPI_CTRL_STATUS_PLS;

	__writel(value, ioaddr + LPI_CTRL_STATUS);
}

void dwmac1000_set_eee_timer(void *ioaddr, int ls, int tw)
{

	int value = ((tw & 0xffff)) | ((ls & 0x7ff) << 16);

	/* Program the timers in the LPI timer control register:
	 * LS: minimum time (ms) for which the link
	 *  status from PHY should be ok before transmitting
	 *  the LPI pattern.
	 * TW: minimum time (us) for which the core waits
	 *  after it has stopped transmitting the LPI pattern.
	 */
	__writel(value, ioaddr + LPI_TIMER_CTRL);
}

void dwmac1000_ctrl_ane(void *ioaddr, boolean restart)
{
	u32 value;

	value = readl(ioaddr + GMAC_AN_CTRL);
	/* auto negotiation enable and External Loopback enable */
	value = GMAC_AN_CTRL_ANE | GMAC_AN_CTRL_ELE;

	if (restart)
		value |= GMAC_AN_CTRL_RAN;

	__writel(value, ioaddr + GMAC_AN_CTRL);
}

void dwmac1000_get_adv(void *ioaddr, struct rgmii_adv *adv)
{
	u32 value = readl(ioaddr + GMAC_ANE_ADV);

	if (value & GMAC_ANE_FD)
		adv->duplex = DUPLEX_FULL;


	if (value & GMAC_ANE_HD)
		adv->duplex |= DUPLEX_HALF;

	adv->pause = (value & GMAC_ANE_PSE) >> GMAC_ANE_PSE_SHIFT;

	value = readl(ioaddr + GMAC_ANE_LPA);

	if (value & GMAC_ANE_FD)
		adv->lp_duplex = DUPLEX_FULL;


	if (value & GMAC_ANE_HD)
		adv->lp_duplex = DUPLEX_HALF;


	adv->lp_pause = (value & GMAC_ANE_PSE) >> GMAC_ANE_PSE_SHIFT;

}
