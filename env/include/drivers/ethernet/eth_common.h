/*******************************************************************************
  STMMAC Common Header File

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

#ifndef _ENV_INCLUDE_DRIVER_ETH_ETH_COMMON_H_
#define _ENV_INCLUDE_DRIVER_ETH_ETH_COMMON_H_

#include <types.h>

#include "descs.h"
/* Synopsys Core versions */
#define	DWMAC_CORE_3_40	0x34
#define	DWMAC_CORE_3_50	0x35

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

struct stmmac_extra_stats {
	/* Transmit errors */
	unsigned long tx_underflow; //____cacheline_aligned;
	unsigned long tx_carrier;
	unsigned long tx_losscarrier;
	unsigned long vlan_tag;
	unsigned long tx_deferred;
	unsigned long tx_vlan;
	unsigned long tx_jabber;
	unsigned long tx_frame_flushed;
	unsigned long tx_payload_error;
	unsigned long tx_ip_header_error;
	/* Receive errors */
	unsigned long rx_desc;
	unsigned long sa_filter_fail;
	unsigned long overflow_error;
	unsigned long ipc_csum_error;
	unsigned long rx_collision;
	unsigned long rx_crc;
	unsigned long dribbling_bit;
	unsigned long rx_length;
	unsigned long rx_mii;
	unsigned long rx_multicast;
	unsigned long rx_gmac_overflow;
	unsigned long rx_watchdog;
	unsigned long da_rx_filter_fail;
	unsigned long sa_rx_filter_fail;
	unsigned long rx_missed_cntr;
	unsigned long rx_overflow_cntr;
	unsigned long rx_vlan;
	/* Tx/Rx IRQ error info */
	unsigned long tx_undeflow_irq;
	unsigned long tx_process_stopped_irq;
	unsigned long tx_jabber_irq;
	unsigned long rx_overflow_irq;
	unsigned long rx_buf_unav_irq;
	unsigned long rx_process_stopped_irq;
	unsigned long rx_watchdog_irq;
	unsigned long tx_early_irq;
	unsigned long fatal_bus_error_irq;
	/* Tx/Rx IRQ Events */
	unsigned long rx_early_irq;
	unsigned long threshold;
	unsigned long tx_pkt_n;
	unsigned long rx_pkt_n;
	unsigned long normal_irq_n;
	unsigned long rx_normal_irq_n;
	unsigned long napi_poll;
	unsigned long tx_normal_irq_n;
	unsigned long tx_clean;
	unsigned long tx_reset_ic_bit;
	unsigned long irq_receive_pmt_irq_n;
	/* MMC info */
	unsigned long mmc_tx_irq_n;
	unsigned long mmc_rx_irq_n;
	unsigned long mmc_rx_csum_offload_irq_n;
	/* EEE */
	unsigned long irq_tx_path_in_lpi_mode_n;
	unsigned long irq_tx_path_exit_lpi_mode_n;
	unsigned long irq_rx_path_in_lpi_mode_n;
	unsigned long irq_rx_path_exit_lpi_mode_n;
	unsigned long phy_eee_wakeup_error_n;
	/* Extended RDES status */
	unsigned long ip_hdr_err;
	unsigned long ip_payload_err;
	unsigned long ip_csum_bypassed;
	unsigned long ipv4_pkt_rcvd;
	unsigned long ipv6_pkt_rcvd;
	unsigned long rx_msg_type_ext_no_ptp;
	unsigned long rx_msg_type_sync;
	unsigned long rx_msg_type_follow_up;
	unsigned long rx_msg_type_delay_req;
	unsigned long rx_msg_type_delay_resp;
	unsigned long rx_msg_type_pdelay_req;
	unsigned long rx_msg_type_pdelay_resp;
	unsigned long rx_msg_type_pdelay_follow_up;
	unsigned long ptp_frame_type;
	unsigned long ptp_ver;
	unsigned long timestamp_dropped;
	unsigned long av_pkt_rcvd;
	unsigned long av_tagged_pkt_rcvd;
	unsigned long vlan_tag_priority_val;
	unsigned long l3_filter_match;
	unsigned long l4_filter_match;
	unsigned long l3_l4_filter_no_match;
	/* PCS */
	unsigned long irq_pcs_ane_n;
	unsigned long irq_pcs_link_n;
	unsigned long irq_rgmii_n;
	unsigned long pcs_link;
	unsigned long pcs_duplex;
	unsigned long pcs_speed;
};

struct net_device_stats {
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	multicast;
	unsigned long	collisions;
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;
	unsigned long	rx_crc_errors;
	unsigned long	rx_frame_errors;
	unsigned long	rx_fifo_errors;
	unsigned long	rx_missed_errors;
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};


#define STMMAC_RX_COE_NONE	0
#define STMMAC_RX_COE_TYPE1	1
#define STMMAC_RX_COE_TYPE2	2


/* CSR Frequency Access Defines*/
#define CSR_F_35M	35000000
#define CSR_F_60M	60000000
#define CSR_F_100M	100000000
#define CSR_F_150M	150000000
#define CSR_F_250M	250000000
#define CSR_F_300M	300000000

#define	MAC_CSR_H_FRQ_MASK	0x20

#define HASH_TABLE_SIZE 64
#define PAUSE_TIME 0x200

/* Flow Control defines */
#define FLOW_OFF	0
#define FLOW_RX		1
#define FLOW_TX		2
#define FLOW_AUTO	(FLOW_TX | FLOW_RX)

/* PCS defines */
#define STMMAC_PCS_RGMII	(1 << 0)
#define STMMAC_PCS_SGMII	(1 << 1)
#define STMMAC_PCS_TBI		(1 << 2)
#define STMMAC_PCS_RTBI		(1 << 3)

#define SF_DMA_MODE 1		/* DMA STORE-AND-FORWARD Operation Mode */

/* DAM HW feature register fields */
#define DMA_HW_FEAT_MIISEL	0x00000001	/* 10/100 Mbps Support */
#define DMA_HW_FEAT_GMIISEL	0x00000002	/* 1000 Mbps Support */
#define DMA_HW_FEAT_HDSEL	0x00000004	/* Half-Duplex Support */
#define DMA_HW_FEAT_EXTHASHEN	0x00000008	/* Expanded DA Hash Filter */
#define DMA_HW_FEAT_HASHSEL	0x00000010	/* HASH Filter */
#define DMA_HW_FEAT_ADDMAC	0x00000020	/* Multiple MAC Addr Reg */
#define DMA_HW_FEAT_PCSSEL	0x00000040	/* PCS registers */
#define DMA_HW_FEAT_L3L4FLTREN	0x00000080	/* Layer 3 & Layer 4 Feature */
#define DMA_HW_FEAT_SMASEL	0x00000100	/* SMA(MDIO) Interface */
#define DMA_HW_FEAT_RWKSEL	0x00000200	/* PMT Remote Wakeup */
#define DMA_HW_FEAT_MGKSEL	0x00000400	/* PMT Magic Packet */
#define DMA_HW_FEAT_MMCSEL	0x00000800	/* RMON Module */
#define DMA_HW_FEAT_TSVER1SEL	0x00001000	/* Only IEEE 1588-2002 */
#define DMA_HW_FEAT_TSVER2SEL	0x00002000	/* IEEE 1588-2008 PTPv2 */
#define DMA_HW_FEAT_EEESEL	0x00004000	/* Energy Efficient Ethernet */
#define DMA_HW_FEAT_AVSEL	0x00008000	/* AV Feature */
#define DMA_HW_FEAT_TXCOESEL	0x00010000	/* Checksum Offload in Tx */
#define DMA_HW_FEAT_RXTYP1COE	0x00020000	/* IP COE (Type 1) in Rx */
#define DMA_HW_FEAT_RXTYP2COE	0x00040000	/* IP COE (Type 2) in Rx */
#define DMA_HW_FEAT_RXFIFOSIZE	0x00080000	/* Rx FIFO > 2048 Bytes */
#define DMA_HW_FEAT_RXCHCNT	0x00300000	/* No. additional Rx Channels */
#define DMA_HW_FEAT_TXCHCNT	0x00c00000	/* No. additional Tx Channels */
#define DMA_HW_FEAT_ENHDESSEL	0x01000000	/* Alternate Descriptor */
/* Timestamping with Internal System Time */
#define DMA_HW_FEAT_INTTSEN	0x02000000
#define DMA_HW_FEAT_FLEXIPPSEN	0x04000000	/* Flexible PPS Output */
#define DMA_HW_FEAT_SAVLANINS	0x08000000	/* Source Addr or VLAN */
#define DMA_HW_FEAT_ACTPHYIF	0x70000000	/* Active/selected PHY iface */
#define DEFAULT_DMA_PBL		8

/* Max/Min RI Watchdog Timer count value */
#define MAX_DMA_RIWT		0xff
#define MIN_DMA_RIWT		0x20
/* Tx coalesce parameters */
#define STMMAC_COAL_TX_TIMER	40000
#define STMMAC_MAX_COAL_TX_TICK	100000
#define STMMAC_TX_MAX_FRAMES	256
#define STMMAC_TX_FRAMES	64

#define STMMAC_UNAVAIL_RX_TIMER	(1000*1000)	/* uS */

/* Rx IPC status */
enum rx_frame_status {
	good_frame = 0,
	discard_frame = 1,
	csum_none = 2,
	llc_snap = 4,
};

enum dma_irq_status {
	dwmac_tx_hard_error = 0x1,
	dwmac_tx_hard_error_bump_tc = 0x2,
	dwmac_handle_rx = 0x4,
	dwmac_handle_tx = 0x8,
};

#define	CORE_IRQ_TX_PATH_IN_LPI_MODE	(1 << 1)
#define	CORE_IRQ_TX_PATH_EXIT_LPI_MODE	(1 << 2)
#define	CORE_IRQ_RX_PATH_IN_LPI_MODE	(1 << 3)
#define	CORE_IRQ_RX_PATH_EXIT_LPI_MODE	(1 << 4)

#define	CORE_PCS_ANE_COMPLETE		(1 << 5)
#define	CORE_PCS_LINK_STATUS		(1 << 6)
#define	CORE_RGMII_IRQ			(1 << 7)

/* Wake-On-Lan options. */
#define WAKE_PHY		(1 << 0)
#define WAKE_UCAST		(1 << 1)
#define WAKE_MCAST		(1 << 2)
#define WAKE_BCAST		(1 << 3)
#define WAKE_ARP		(1 << 4)
#define WAKE_MAGIC		(1 << 5)
#define WAKE_MAGICSECURE	(1 << 6) /* only meaningful if WAKE_MAGIC */

/* The forced speed, 10Mb, 100Mb, gigabit, 2.5Gb, 10GbE. */
#define SPEED_10		10
#define SPEED_100		100
#define SPEED_1000		1000
#define SPEED_2500		2500
#define SPEED_10000		10000
#define SPEED_UNKNOWN		-1

/* Duplex, half or full. */
#define DUPLEX_HALF		0x00
#define DUPLEX_FULL		0x01
#define DUPLEX_UNKNOWN		0xff

struct rgmii_adv {
	unsigned int pause;
	unsigned int duplex;
	unsigned int lp_pause;
	unsigned int lp_duplex;
};

#define STMMAC_PCS_PAUSE	1
#define STMMAC_PCS_ASYM_PAUSE	2

/* DMA HW capabilities */
struct dma_features {
	unsigned int mbps_10_100;
	unsigned int mbps_1000;
	unsigned int half_duplex;
	unsigned int hash_filter;
	unsigned int multi_addr;
	unsigned int pcs;
	unsigned int sma_mdio;
	unsigned int pmt_remote_wake_up;
	unsigned int pmt_magic_frame;
	unsigned int rmon;
	/* IEEE 1588-2002 */
	unsigned int time_stamp;
	/* IEEE 1588-2008 */
	unsigned int atime_stamp;
	/* 802.3az - Energy-Efficient Ethernet (EEE) */
	unsigned int eee;
	unsigned int av;
	/* TX and RX csum */
	unsigned int tx_coe;
	unsigned int rx_coe_type1;
	unsigned int rx_coe_type2;
	unsigned int rxfifo_over_2048;
	/* TX and RX number of channels */
	unsigned int number_rx_channel;
	unsigned int number_tx_channel;
	/* Alternate (enhanced) DESC mode */
	unsigned int enh_desc;
};

/* GMAC TX FIFO is 8K, Rx FIFO is 16K */
#define BUF_SIZE_16KiB 16384
#define BUF_SIZE_8KiB 8192
#define BUF_SIZE_4KiB 4096
#define BUF_SIZE_2KiB 2048

/* Power Down and WOL */
#define PMT_NOT_SUPPORTED 0
#define PMT_SUPPORTED 1

/* Common MAC defines */
#define MAC_CTRL_REG		0x00000000	/* MAC Control */
#define MAC_ENABLE_TX		0x00000008	/* Transmitter Enable */
#define MAC_RNABLE_RX		0x00000004	/* Receiver Enable */

/* Default LPI timers */
#define STMMAC_DEFAULT_LIT_LS	0x3E8
#define STMMAC_DEFAULT_TWT_LS	0x0

#define STMMAC_CHAIN_MODE	0x1
#define STMMAC_RING_MODE	0x2

//dmmac1000_core.c
extern void dwmac1000_core_init(void *ioaddr, int mtu);
extern int dwmac1000_rx_ipc_enable(void *ioaddr);
extern void dwmac1000_set_umac_addr(void *ioaddr, unsigned char *addr,
				    unsigned int reg_n);
extern void dwmac1000_get_umac_addr(void *ioaddr, unsigned char *addr,
				    unsigned int reg_n);
extern void dwmac1000_set_filter(void *ioaddr);
extern void dwmac1000_flow_ctrl(void *ioaddr, unsigned int duplex,
				unsigned int fc, unsigned int pause_time);
extern void dwmac1000_pmt(void *ioaddr, unsigned long mode);
extern int dwmac1000_irq_status(void *ioaddr,
				struct stmmac_extra_stats *x);
extern void dwmac1000_set_eee_mode(void *ioaddr);
extern void dwmac1000_reset_eee_mode(void *ioaddr);
extern void dwmac1000_set_eee_pls(void *ioaddr, int link);
extern void dwmac1000_set_eee_timer(void *ioaddr, int ls, int tw);
extern void dwmac1000_ctrl_ane(void *ioaddr, boolean restart);
extern void dwmac1000_get_adv(void *ioaddr, struct rgmii_adv *adv);

//dwmac1000_dma.c
extern int dwmac1000_dma_init(void *ioaddr, int pbl, int fb, int mb,
			      int burst_len, u32 dma_tx, u32 dma_rx, int atds);
extern void dwmac1000_dma_operation_mode(void *ioaddr, int txmode,
					 int rxmode);
extern unsigned int dwmac1000_get_hw_feature(void *ioaddr);
extern void dwmac1000_rx_watchdog(void *ioaddr, u32 riwt);

//dmamac_lib.c
extern void dwmac_enable_dma_receive(void *ioaddr);
extern void dwmac_enable_dma_transmission(void *ioaddr);
extern void dwmac_enable_dma_irq(void *ioaddr);
extern void dwmac_disable_dma_irq(void *ioaddr);
extern void dwmac_dma_start_tx(void *ioaddr);
extern void dwmac_dma_stop_tx(void *ioaddr);
extern void dwmac_dma_start_rx(void *ioaddr);
extern void dwmac_dma_stop_rx(void *ioaddr);
extern int dwmac_dma_interrupt(void *ioaddr,
			struct stmmac_extra_stats *x);
extern void dwmac_dma_flush_tx_fifo(void *ioaddr);
extern void stmmac_set_mac_addr(void *ioaddr, u8 addr[6],
			 unsigned int high, unsigned int low);
extern void stmmac_set_mac(void *ioaddr, boolean enable);
extern void stmmac_get_mac_addr(void *ioaddr, unsigned char *addr,
			 unsigned int high, unsigned int low);

//mmc_core.c
extern void stmmac_mmc_setup(void *ioaddr);

//enh_desc.c
extern int enh_desc_get_tx_status(void *data, struct stmmac_extra_stats *x,
				  dma_desc *p, void *ioaddr);
extern int enh_desc_get_tx_len(dma_desc *p);
extern int enh_desc_coe_rdes0(int ipc_err, int type, int payload_err);
extern void enh_desc_get_ext_status(void *data, struct stmmac_extra_stats *x, dma_extended_desc *p);
extern int enh_desc_get_rx_status(void *data, struct stmmac_extra_stats *x, dma_desc *p);
extern void enh_desc_init_rx_desc(dma_desc *p, int disable_rx_ic, int mode, int end);
extern void enh_desc_init_tx_desc(dma_desc *p, int mode, int end);
extern int enh_desc_get_tx_owner(dma_desc *p);
extern int enh_desc_get_rx_owner(dma_desc *p);
extern void enh_desc_set_tx_owner(dma_desc *p);
extern void enh_desc_set_rx_owner(dma_desc *p);
extern int enh_desc_get_tx_ls(dma_desc *p);
extern void enh_desc_release_tx_desc(dma_desc *p, int mode);
extern void enh_desc_prepare_tx_desc(dma_desc *p, int is_fs, int len, int csum_flag, int mode);
extern void enh_desc_clear_tx_ic(dma_desc *p);
extern void enh_desc_close_tx_desc(dma_desc *p);
extern int enh_desc_get_rx_frame_len(dma_desc *p, int rx_coe_type);
extern void enh_desc_enable_tx_timestamp(dma_desc *p);
extern int enh_desc_get_tx_timestamp_status(dma_desc *p);
extern u64 enh_desc_get_timestamp(void *desc, u32 ats);
extern int enh_desc_get_rx_timestamp_status(void *desc, u32 ats);

//nxpmac_main.c
extern int genphy_config_advert(void);
extern void bd_gpio_init(void);
extern int  nxpmac_init(void);
extern int stmmac_mdio_reset(void);
extern int nxp_plat_initialize(void);


//
#endif /* __COMMON_H__ */
