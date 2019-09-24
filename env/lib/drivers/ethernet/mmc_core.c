/*******************************************************************************
  DWMAC Management Counters

  Copyright (C) 2011  STMicroelectronics Ltd

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

#include <stdio.h>
#include "io.h"
#include "types.h"

/* MAC Management Counters register offset */

#define MMC_CNTRL		0x00000100	/* MMC Control */
#define MMC_RX_INTR		0x00000104	/* MMC RX Interrupt */
#define MMC_TX_INTR		0x00000108	/* MMC TX Interrupt */
#define MMC_RX_INTR_MASK	0x0000010c	/* MMC Interrupt Mask */
#define MMC_TX_INTR_MASK	0x00000110	/* MMC Interrupt Mask */
#define MMC_DEFAULT_MASK		0xffffffff

/* MMC TX counter registers */

/* Note:
 * _GB register stands for good and bad frames
 * _G is for good only.
 */
#define MMC_TX_OCTETCOUNT_GB		0x00000114
#define MMC_TX_FRAMECOUNT_GB		0x00000118
#define MMC_TX_BROADCASTFRAME_G		0x0000011c
#define MMC_TX_MULTICASTFRAME_G		0x00000120
#define MMC_TX_64_OCTETS_GB		0x00000124
#define MMC_TX_65_TO_127_OCTETS_GB	0x00000128
#define MMC_TX_128_TO_255_OCTETS_GB	0x0000012c
#define MMC_TX_256_TO_511_OCTETS_GB	0x00000130
#define MMC_TX_512_TO_1023_OCTETS_GB	0x00000134
#define MMC_TX_1024_TO_MAX_OCTETS_GB	0x00000138
#define MMC_TX_UNICAST_GB		0x0000013c
#define MMC_TX_MULTICAST_GB		0x00000140
#define MMC_TX_BROADCAST_GB		0x00000144
#define MMC_TX_UNDERFLOW_ERROR		0x00000148
#define MMC_TX_SINGLECOL_G		0x0000014c
#define MMC_TX_MULTICOL_G		0x00000150
#define MMC_TX_DEFERRED			0x00000154
#define MMC_TX_LATECOL			0x00000158
#define MMC_TX_EXESSCOL			0x0000015c
#define MMC_TX_CARRIER_ERROR		0x00000160
#define MMC_TX_OCTETCOUNT_G		0x00000164
#define MMC_TX_FRAMECOUNT_G		0x00000168
#define MMC_TX_EXCESSDEF		0x0000016c
#define MMC_TX_PAUSE_FRAME		0x00000170
#define MMC_TX_VLAN_FRAME_G		0x00000174

/* MMC RX counter registers */
#define MMC_RX_FRAMECOUNT_GB		0x00000180
#define MMC_RX_OCTETCOUNT_GB		0x00000184
#define MMC_RX_OCTETCOUNT_G		0x00000188
#define MMC_RX_BROADCASTFRAME_G		0x0000018c
#define MMC_RX_MULTICASTFRAME_G		0x00000190
#define MMC_RX_CRC_ERRROR		0x00000194
#define MMC_RX_ALIGN_ERROR		0x00000198
#define MMC_RX_RUN_ERROR		0x0000019C
#define MMC_RX_JABBER_ERROR		0x000001A0
#define MMC_RX_UNDERSIZE_G		0x000001A4
#define MMC_RX_OVERSIZE_G		0x000001A8
#define MMC_RX_64_OCTETS_GB		0x000001AC
#define MMC_RX_65_TO_127_OCTETS_GB	0x000001b0
#define MMC_RX_128_TO_255_OCTETS_GB	0x000001b4
#define MMC_RX_256_TO_511_OCTETS_GB	0x000001b8
#define MMC_RX_512_TO_1023_OCTETS_GB	0x000001bc
#define MMC_RX_1024_TO_MAX_OCTETS_GB	0x000001c0
#define MMC_RX_UNICAST_G		0x000001c4
#define MMC_RX_LENGTH_ERROR		0x000001c8
#define MMC_RX_AUTOFRANGETYPE		0x000001cc
#define MMC_RX_PAUSE_FRAMES		0x000001d0
#define MMC_RX_FIFO_OVERFLOW		0x000001d4
#define MMC_RX_VLAN_FRAMES_GB		0x000001d8
#define MMC_RX_WATCHDOG_ERROR		0x000001dc
/* IPC*/
#define MMC_RX_IPC_INTR_MASK		0x00000200
#define MMC_RX_IPC_INTR			0x00000208
/* IPv4*/
#define MMC_RX_IPV4_GD			0x00000210
#define MMC_RX_IPV4_HDERR		0x00000214
#define MMC_RX_IPV4_NOPAY		0x00000218
#define MMC_RX_IPV4_FRAG		0x0000021C
#define MMC_RX_IPV4_UDSBL		0x00000220

#define MMC_RX_IPV4_GD_OCTETS		0x00000250
#define MMC_RX_IPV4_HDERR_OCTETS	0x00000254
#define MMC_RX_IPV4_NOPAY_OCTETS	0x00000258
#define MMC_RX_IPV4_FRAG_OCTETS		0x0000025c
#define MMC_RX_IPV4_UDSBL_OCTETS	0x00000260

/* IPV6*/
#define MMC_RX_IPV6_GD_OCTETS		0x00000264
#define MMC_RX_IPV6_HDERR_OCTETS	0x00000268
#define MMC_RX_IPV6_NOPAY_OCTETS	0x0000026c

#define MMC_RX_IPV6_GD			0x00000224
#define MMC_RX_IPV6_HDERR		0x00000228
#define MMC_RX_IPV6_NOPAY		0x0000022c

/* Protocols*/
#define MMC_RX_UDP_GD			0x00000230
#define MMC_RX_UDP_ERR			0x00000234
#define MMC_RX_TCP_GD			0x00000238
#define MMC_RX_TCP_ERR			0x0000023c
#define MMC_RX_ICMP_GD			0x00000240
#define MMC_RX_ICMP_ERR			0x00000244

#define MMC_RX_UDP_GD_OCTETS		0x00000270
#define MMC_RX_UDP_ERR_OCTETS		0x00000274
#define MMC_RX_TCP_GD_OCTETS		0x00000278
#define MMC_RX_TCP_ERR_OCTETS		0x0000027c
#define MMC_RX_ICMP_GD_OCTETS		0x00000280
#define MMC_RX_ICMP_ERR_OCTETS		0x00000284

/* MMC control register */
/* When set, all counter are reset */
#define MMC_CNTRL_COUNTER_RESET		0x1
/* When set, do not roll over zero after reaching the max value*/
#define MMC_CNTRL_COUNTER_STOP_ROLLOVER	0x2
#define MMC_CNTRL_RESET_ON_READ		0x4	/* Reset after reading */
#define MMC_CNTRL_COUNTER_FREEZER	0x8	/* Freeze counter values to the
						 * current value.*/
#define MMC_CNTRL_PRESET		0x10
#define MMC_CNTRL_FULL_HALF_PRESET	0x20

static void dwmac_mmc_ctrl(void *ioaddr, unsigned int mode)
{
	u32 value = readl(ioaddr + MMC_CNTRL);

	value |= (mode & 0x3F);
	__writel(value, ioaddr + MMC_CNTRL);

}

/* To mask all all interrupts.*/
static void dwmac_mmc_intr_all_mask(void *ioaddr)
{
	__writel(MMC_DEFAULT_MASK, ioaddr + MMC_RX_INTR_MASK);
	__writel(MMC_DEFAULT_MASK, ioaddr + MMC_TX_INTR_MASK);
	__writel(MMC_DEFAULT_MASK, ioaddr + MMC_RX_IPC_INTR_MASK);
}

void stmmac_mmc_setup(void *ioaddr)
{
    unsigned int mode = MMC_CNTRL_RESET_ON_READ | MMC_CNTRL_COUNTER_RESET |
        MMC_CNTRL_PRESET | MMC_CNTRL_FULL_HALF_PRESET;

    dwmac_mmc_intr_all_mask(ioaddr);

    dwmac_mmc_ctrl(ioaddr, mode);
}
