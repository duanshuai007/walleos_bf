#include <stdio.h>
#include <types.h>
#include <i2c/i2c_util.h>
#include <nxp/nx_chip.h>
#include <nxp/nx_rstcon.h>
#include <s5p4418_gpio.h>
#include <s5p4418_rstcon.h>
#include <smp_macros.h>
#include <s5p4418_tick_delay.h>
#include <config.h>
#include <io.h>

//使用i2c1, 对应对GPIO引脚为GPIOD4,GPIOD5

const u32 I2C_DEVICE[NUMBER_OF_I2C_MODULE] = {
	PHY_BASEADDR_I2C0_MODULE, PHY_BASEADDR_I2C1_MODULE , PHY_BASEADDR_I2C2_MODULE };

static void i2c_gpio_set_func(u32 GPIO_POS)
{
	u32 value = 0;
     
	value = readl(S5P4418_GPIOD_BASE + GPIO_ALTFN0);
    value &= ~((ALTFH_MASK << I2C1_SDA_ALT_OFFSET) | (ALTFH_MASK << I2C1_SCL_ALT_OFFSET));
    value |= GPIO_POS;
    writel((S5P4418_GPIOD_BASE + GPIO_ALTFN0), value); 

    //GPIOD24为i2c中断引脚，配置中断功能
	value = readl((S5P4418_GPIOD_BASE + GPIO_DRV0));
	value &= ~(1 << 24);
	writel((S5P4418_GPIOD_BASE + GPIO_DRV0), value);
}

static void i2c_set_pclk_mode(u8 enable)
{
	const u32 PCLK_MODE_OFFSET	= 3;
	
    u32 clock = readl(I2C1_CLK_BASE);
    clock &= ~(1 << PCLK_MODE_OFFSET);
    if(enable)
    {
        clock |= (1 << PCLK_MODE_OFFSET);
    }
    __writel(clock, I2C1_CLK_BASE);
}

//参数 = ENABLE; 使能RESET
//参数 = DISABLE; NO RESET
static void i2c_set_ipreset(u8 enable)
{
    u32 reset;
    reset = readl(I2C1_RESET_BASE);
    reset &= ~(1 << RESET_ID_I2C1);
    if(!enable)
    {
        reset |= (1 << RESET_ID_I2C1);
    }
	__writel(reset, I2C1_RESET_BASE);
}

void I2C_Util_Init( void )
{
    /* I2C GPIO设置     0xFFFFF5FF*/
    i2c_gpio_set_func(((ALTFH1 << I2C1_SDA_ALT_OFFSET) | (ALTFH1 <<I2C1_SCL_ALT_OFFSET)));
    /*I2C 时钟设置*/
    i2c_set_pclk_mode(ENABLE);
    /*I2C reset设置(1:No Reset)*/
    i2c_set_ipreset(DISABLE);
}

void I2C_Util_DeInit( void )
{
	/* GPIOx PAD Altnate Function */
    i2c_gpio_set_func(((ALTFH0 << I2C1_SDA_ALT_OFFSET) | (ALTFH0 <<I2C1_SCL_ALT_OFFSET)));
	/* Clock Generator Initialize */
	i2c_set_pclk_mode(DISABLE);
    /* Reset Generation (1:No Reset) */
    i2c_set_ipreset(ENABLE);
}

static boolean i2c_check_busy(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 ST_BUSY_POS	= 5;
	const u32 ST_BUSY_MASK	= 1 << ST_BUSY_POS;

	return (readl(base + ICSR_OFF) & ST_BUSY_MASK ) >> ST_BUSY_POS;
}

static boolean i2c_check_arbi(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 ARBIT_FAIL_POS	= 3;
	const u32 ARBIT_FAIL_MASK	= 1 << ARBIT_FAIL_POS;

	return (readl(base + ICSR_OFF) & ARBIT_FAIL_MASK ) >> ARBIT_FAIL_POS;
}

static boolean i2c_check_recv(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 ACK_STATUS_POS    = 0;
	const u32 ACK_STATUS_MASK   = 1UL << ACK_STATUS_POS;

	return !(readl(base + ICSR_OFF) & ACK_STATUS_MASK ) >> ACK_STATUS_POS;
}

int i2c_error_chk( u32 channel )
{
	if( !i2c_check_busy( channel ) )
	{
	    printf("i2c err: not busy!\r\n");
        goto ERR;
	}
 
    if( i2c_check_arbi( channel) )
	{
        printf("i2c err: arbitration failed!\r\n");
        goto ERR;
	}
			  
	if ( TRUE != i2c_check_recv( channel ) )
	{
	    printf("i2c err: not-acked!\r\n");
        goto ERR;
	}

	return 0;
ERR:
    return -1;
}

static void i2c_set_mode(u32 channel, u32 mode, u32 signal)
{
	const u32 TX_RX_POS		= 6;
	const u32 ST_BUSY_POS	= 5;
	const u32 TXRX_ENB_MASK	= 1 << 4;
	u32 base = I2C_DEVICE[channel];
	
	int val;
	
	val = readl(base + ICSR_OFF);
	val &= 0xF;
	val |= ((mode << TX_RX_POS) | (signal << ST_BUSY_POS) | (TXRX_ENB_MASK));
	writel(base + ICSR_OFF, val);
}

static void i2c_write_byte(u32 channel, u8 dat)
{
	u32 base = I2C_DEVICE[channel];

	writel(base + IDSR_OFF, dat);
}

static boolean i2c_get_interrupt_pending(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 PEND_POS	= 4;
	const u32 PEND_MASK	= 1 << PEND_POS;

	return (readl(base + ICCR_OFF)& PEND_MASK) >> PEND_POS;
}

static void i2c_clear_interrupt_pending(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 PEND_POS   = 4; 
	const u32 INTC_POS   = 8; 
	const u32 PEND_MASK  = 1UL << PEND_POS;
	const u32 INTC_MASK  = 1UL << INTC_POS;

	int val;

	val = readl(base + ICCR_OFF);
	val &= ~ PEND_MASK;
	val |= INTC_MASK;
	writel(base + ICCR_OFF, val);
}

static void i2c_not_ack(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 NOT_ACK_OFF	= 2;
	const u32 NOT_ACK_MASK  = 1 << NOT_ACK_OFF;

	writel(base + STOPCON_OFF, NOT_ACK_MASK);
}

static u8 i2c_read_byte(u32 channel)
{
	u32 base = I2C_DEVICE[channel];

	return (u8)readl(base + IDSR_OFF);
}

static void i2c_bus_disable(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 TXRX_ENB_MASK = ( 0x01 << 4 );

	int val;

	val = readl(base + ICSR_OFF);
	val &= ~TXRX_ENB_MASK;
	writel(base + ICSR_OFF, val);
}

static void i2c_clockline_release(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 CLK_REL_MASK  = ( 0x01 << 0 );

	writel(base + STOPCON_OFF, CLK_REL_MASK);
}

static void i2c_dataline_release(u32 channel)
{
	u32 base = I2C_DEVICE[channel];
	const u32 DAT_REL_MASK = 1 << 1;

	writel(base + STOPCON_OFF, DAT_REL_MASK);
}

int i2c_master_read( u32 channel, u8 slaveaddr, u8 subaddr, u32 size, u8* buffer )
{
	u32 i   = 0;
	int ret = 0;
	 
    while(i2c_check_busy( channel ));
	/* Start Bit Generation */
    i2c_set_mode( channel, I2C_TXRXMODE_MASTER_TX, I2C_SIGNAL_START );
	i2c_write_byte( channel, (slaveaddr & 0xFE) );
	while( !i2c_get_interrupt_pending( channel) );
	i2c_clear_interrupt_pending( channel );
    if( -1 == (ret = i2c_error_chk(channel) ) )
	    goto ERR_END_TX;
				 
	// Sub Address (Register Address)
	i2c_write_byte( channel, (subaddr & 0xFF) );
	while( !i2c_get_interrupt_pending( channel) );
	i2c_clear_interrupt_pending( channel );
	if( -1 == (ret = i2c_error_chk(channel) ) )
	    goto ERR_END_TX;
								 
	/* No-Stop, Start(Start) */
	i2c_set_mode( channel, I2C_TXRXMODE_MASTER_RX, I2C_SIGNAL_START );
    i2c_write_byte( channel, (slaveaddr & 0xFE) );
	while( !i2c_get_interrupt_pending( channel) );
	i2c_clear_interrupt_pending( channel );
	if( -1 == (ret = i2c_error_chk(channel) ) )
	    goto ERR_END_RX1;
    
    for(i = 0; i < size; i++)
	{
	    if( i == (size - 1) )
		    i2c_not_ack( channel );
											  
	    while( !i2c_get_interrupt_pending( channel) );
		buffer[i] = i2c_read_byte( channel );
		i2c_clear_interrupt_pending( channel );
        if( -1 == (ret = i2c_error_chk( channel) ) )
		    goto ERR_END_RX2;
	}
	i2c_set_mode( channel, I2C_TXRXMODE_MASTER_RX, I2C_SIGNAL_STOP );
	i2c_bus_disable( channel );
								     
	i2c_clockline_release( channel );
	i2c_dataline_release( channel );

    return ret;
					
ERR_END_TX:
    //printf("i2c err TX\r\n");
    i2c_set_mode( channel, I2C_TXRXMODE_MASTER_TX, I2C_SIGNAL_STOP );
    goto END;
ERR_END_RX1:
    //printf("i2c err RX1\r\n");
ERR_END_RX2:
    //printf("i2c err RX2\r\n");
    i2c_set_mode( channel, I2C_TXRXMODE_MASTER_RX, I2C_SIGNAL_STOP );

END:
    i2c_bus_disable( channel );
    i2c_clockline_release( channel );
    //i2c_dataline_release( channel );
    printk("i2c read failed\r\n");
    return -1;
}

int I2C_MasterRead( I2C_CMD* CmdSet, u8* ReadData )
{
    u32 Channel      = CmdSet->Channel;
	u8  SlaveAddress = CmdSet->SlaveAddress;
	u8  SubAddress   = CmdSet->SubAddress;
    u32 TransferSize = CmdSet->TransferSize;

	int ret          = 0;
	ret = i2c_master_read( Channel, SlaveAddress, SubAddress, TransferSize, ReadData );

	return ret;
}

static void i2c_set_interrupt(u32 channel, boolean enable)
{
	const u32 IRQ_ENB_POS   = 5;
	const u32 IRQ_ENB_MASK  = 1 << IRQ_ENB_POS;

	int val;

	val = readl(I2C_DEVICE[channel] + ICCR_OFF);
	val &= ~IRQ_ENB_MASK;
	val |= enable << IRQ_ENB_POS;
	writel(I2C_DEVICE[channel] + ICCR_OFF, val);
}

static void i2c_set_ackGeneration(u32 channel, boolean enable)
{
	const u32 ACK_GEN_POS   = 7;
	const u32 ACK_GEN_MASK  = 1 << ACK_GEN_POS;

	int val;

	val = readl(I2C_DEVICE[channel] + ICCR_OFF);
	val &= ~ACK_GEN_MASK;
	val |= enable << ACK_GEN_POS;
	writel(I2C_DEVICE[channel] + ICCR_OFF, val);
}

static void i2c_set_clock_prescaler(u32 channel, u32 pclk_div, u32 prescaler)
{
	const u32 CLKSRC_POS    = 6;
	const u32 CLKSRC_MASK   = 1 << CLKSRC_POS;
	const u32 CLK_SCALER_MASK = 0xf;

	int val;
	int set_pclk_div = 0;

	if(pclk_div == 16)
		set_pclk_div = 0;
	else if(pclk_div == 256)
		set_pclk_div = 1;

	val = readl(I2C_DEVICE[channel] + ICCR_OFF);
	val &= ~(CLKSRC_MASK | CLK_SCALER_MASK);
	val |= ((set_pclk_div << CLKSRC_POS) | (prescaler - 1));
	writel(I2C_DEVICE[channel] + ICCR_OFF, val);
}

static inline void i2c_set_slaveAddress(u32 channel, u8 address)
{
	writel(I2C_DEVICE[channel] + IAR_OFF, address);
}

static void i2c_get_clock_prescaler(u32 channel, u32 *pclk_div, u32 *prescaler)
{
	const u32   CLKSRC_POS  = 6;
	const u32   CLKSRC_MASK = 1UL << CLKSRC_POS;
	const u32   CLK_SCALER_MASK = 0x0F;

	int val;

	val = readl(I2C_DEVICE[channel] + ICCR_OFF);
	if(val & CLKSRC_MASK)
	{
		*pclk_div = 256;
	}
	else
	{
		*pclk_div = 16;
	}

	*prescaler = (val & CLK_SCALER_MASK) + 1;
}

static inline boolean i2c_get_ack_generation_enable(u32 channel)
{
	const u32 ACK_GEN_POS   = 7;
	const u32 ACK_GEN_MASK  = 1UL << ACK_GEN_POS;

	return (readl(I2C_DEVICE[channel] + ICCR_OFF) & ACK_GEN_MASK) >> ACK_GEN_POS;
}

static inline boolean i2c_get_interrupt_enanle(u32 channel)
{
	const u32 IRQ_ENB_POS   = 5;
	const u32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

	return (readl(I2C_DEVICE[channel] + ICCR_OFF) & IRQ_ENB_MASK ) >> IRQ_ENB_POS;
}

static inline u32 i2c_get_slave_address(u32 channel)
{
	return readl(I2C_DEVICE[channel] + IAR_OFF) & 0x7f;
}

void I2C_IO_Init(I2C_CMD* CmdSet)
{
    u32 Channel          = CmdSet->Channel;
	u32 MSMode           = CmdSet->MSMode;
	u32 ClockDivider     = CmdSet->ClockDivider;
	u32 ClockPrescaler   = CmdSet->ClockPrescaler;
	u32 GetClkDivider    = 0;
	u32 GetClkPrescaler  = 0;
    I2C_Util_Init();

	/* I2C Deinitialize */
	i2c_set_interrupt(Channel, FALSE);
	i2c_set_ackGeneration(Channel, FALSE);
	i2c_bus_disable(Channel);

	/* I2C Initialize */
	i2c_set_clock_prescaler(Channel,ClockDivider,ClockPrescaler);
	if( (MSMode == I2C_TXRXMODE_SLAVE_TX) || (MSMode == I2C_TXRXMODE_SLAVE_RX))
	{
	    i2c_set_slaveAddress(Channel,CmdSet->SlaveAddress);
	}
	i2c_set_ackGeneration(Channel,TRUE);
	i2c_set_interrupt(Channel, TRUE);	 
	i2c_clear_interrupt_pending(Channel);
	i2c_get_clock_prescaler( Channel, &GetClkDivider, &GetClkPrescaler );
	
	printf("I2C%d Initialize  \r\n", Channel );
	printf("[Clock Divider      ]: %d \r\n", GetClkDivider   );
	printf("[Clock Prescaler    ]: %d \r\n", GetClkPrescaler );
	printf("[AckGeneration      ]: %8s \r\n", i2c_get_ack_generation_enable( Channel ) ? "ENABLE" : "DISABLE" );
	printf("[Tranmit Interrupt  ]: %8s \r\n", i2c_get_interrupt_enanle( Channel) ? "ENABLE" : "DISABLE" );
	if( (MSMode == I2C_TXRXMODE_SLAVE_TX) || (MSMode == I2C_TXRXMODE_SLAVE_RX) )
	{
	    printf(" Slave Address    : %8X \r\n", i2c_get_slave_address( Channel ) );
	}
}

static void I2C_RegisterChk(void)
{
	int val = readl(I2C1_RESET_BASE);
	
	val &= ~(1 << RESETINDEX_OF_I2C1_MODULE_PRESETn);
	writel(I2C1_RESET_BASE, val);
	mdelay(1);
	val |= (1 << RESETINDEX_OF_I2C1_MODULE_PRESETn);
	writel(I2C1_RESET_BASE, val);
}

int i2c_init( void )
{
   	I2C_CMD  MasterCmd;

    MasterCmd.Channel         = 1;
	MasterCmd.MSMode = I2C_TXRXMODE_MASTER_RX;//R
	MasterCmd.TransferSize   = 1;
	MasterCmd.SlaveAddress   = 0x70;//70
    MasterCmd.SubAddress     = 0x80;//80
	MasterCmd.ClockPrescaler = 6;
	MasterCmd.ClockDivider   = 256;

    printf("I2C_RegisterChk\r\n");
    I2C_RegisterChk();

    printf("I2C_IO_Init\r\n");
	I2C_IO_Init(&MasterCmd);

	return 0;
}

