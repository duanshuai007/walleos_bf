#ifndef _ENV_INCLUDE_I2C_I2C_UTIL_H_
#define _ENV_INCLUDE_I2C_I2C_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <nxp/nx_chip.h>

enum i2c_misc{
    I2C_UNUSED  = 0,
    I2C_MODULE_INDEX    = 0,
    INTC_I2C_INFO       = 0,
    I2C_NO_HANDLER      = 0,
    I2C_NO_SUBADDR      = 1,
    I2C_M_RD            = 0x0001,
};

enum{
	ICCR_OFF	= 0x0,
	ICSR_OFF	= 0x4,
	IAR_OFF		= 0x8,
	IDSR_OFF	= 0xc,
	STOPCON_OFF	= 0x10,
};

enum i2c{
    I2C1_CLK_BASE       = 0xC00AF000,
    I2C1_RESET_BASE     = 0xC0012000,
};

struct i2c_state
{
	unsigned int channel;
	unsigned int datacnt;
	unsigned int addrcnt;
	unsigned int ackcnt;
	unsigned int stopflag;
	unsigned int failmsg;
};

typedef enum
{
	I2C_TXRXMODE_SLAVE_RX    = 0,    ///< Slave Receive Mode
	I2C_TXRXMODE_SLAVE_TX    = 1,    ///< Slave Transmit Mode
	I2C_TXRXMODE_MASTER_RX   = 2,    ///< Master Receive Mode
	I2C_TXRXMODE_MASTER_TX   = 3,     ///< Master Transmit Mode
}I2C_TXRXMODE;

typedef enum
{
    REALTEK_SPEAKER_OUTPUT_CONTROL   		= 0x02,
    REALTEK_HEADPHONE_OUTPUT_CONTROL 		= 0x04,
    REALTEK_AX0X_OUTPUT_CONTROL      		= 0x06,

	REALTEK_SPEAKER_MIXER_CONTROL	 		= 0x28,
    REALTEK_SPEAKER_MONO_OUTPUT_CONTROL 	= 0x2A,
    REALTEK_SPE_MONO_HP_OUTPUT_CONTROL		= 0x2C,
    REALTEK_STEREO_SERIAL_DATA_PORT_CONTROL = 0x34,
    REALTEK_STEREO_ADC_DAC_CLOCK_CONTROL	= 0x38,
    REALTEK_POWER_MANAGEMENT1				= 0x3A,
    REALTEK_POWER_MANAGEMENT2				= 0x3B,
    REALTEK_POWER_MANAGEMENT4				= 0x3E,
    REALTEK_GLOBAL_CLOCK_CONTROL			= 0x42,
    REALTEK_PLL_CONTROL						= 0x44,
    
} REALTEK_REG_CMD;

enum
{
	I2C_CH0_SLAVEADDRESS  	 = 0x0C,
	I2C_CH1_SLAVEADDRESS  	 = 0x04,
	I2C_CH2_SLAVEADDRESS  	 = 0x08,

	I2S_CODEC_SLAVEADDRESS   = 0x34,
	PMIC_SLAVEADDRESS		 = 0x64,

	LYNX_TOUCH_SLAVEADDR  	 = 0x70,
	PYSIX_TOUCH_SLAVEADDR	 = 0xE0,
	SMAVD_TOUCH_SLAVEADDR	 = 0x4A,
	SAMVD_TOUCH_SUBADDR	 	 = 0x6B,
};

enum i2c_int{
    I2C_INT_PENDING_BIT = 4,
    I2C_INT_ENABLE_BIT  = 5,
};

enum i2c_gpiod_alt{
    I2C0_SDA_ALT_OFFSET    = 3<<1,
    I2C0_SCL_ALT_OFFSET    = 2<<1,

    I2C1_SDA_ALT_OFFSET    = 5<<1,
    I2C1_SCL_ALT_OFFSET    = 4<<1,
    
    I2C2_SDA_ALT_OFFSET    = 7<<1,
    I2C2_SCL_ALT_OFFSET    = 6<<1,
};

typedef enum
{
	I2C_TRANS_SUBADDRESS = 0,
	I2C_TRANS_READSTART  = 1,

} I2C_TRANS_SEQUENCE;

typedef enum
{
	I2C_SIGNAL_STOP	 = 0,		
	I2C_SIGNAL_START = 1, 			
	I2C_WRITE_DATA   = 2,
	I2C_READ_DATA    = 3
} I2C_SIGNAL;

typedef enum
{
	LCD_INIT_LYNX	 = 1,
	LCD_INIT_PYSIX	 = 2,
	LCD_INIT_SAMVD	 = 3,
} I2C_LCD_INIT;

typedef enum
{	
	I2C_NOT_BUSY 		   = 1,
	I2C_ARBITRATION_FAILED = 2,
	I2C_NOT_ACKED          = 3,
	I2C_STOPBIT_ERR		   = 4
} I2C_ERR;

typedef struct
{
    u32     Channel;
    boolean   ClockDivider;
    u32     ClockPrescaler;
    u32     MSMode;
    u8      SlaveAddress;
    u8      SubAddress;
    u32     TransferSize;
	// Separation
	u32		DataCnt;
	u32		AddrCnt;
	u32		AckCnt;
	u32		ReadSel;
	u32		StopFlag;
	u32		FailedFlag;
} I2C_CMD;

typedef enum
{
    I2C_IO_WRITE_TEST   = 0,
    I2C_IO_READ_TEST    = 1,
    I2C_IO_RECEIVE_TEST = 2,
    I2C_IO_ONBOARD_TEST = 3

} I2C_IO_CHK_OPT;

typedef enum
{
    GPIO_I2C_SCL_CH0  = 3,
    GPIO_I2C_SDA_CH0  = 3,

    GPIO_I2C_SCL_CH1  = 3,
    GPIO_I2C_SDA_CH1  = 3,

    GPIO_I2C_SCL_CH2  = 3,
    GPIO_I2C_SDA_CH2  = 3,
} GPIO_I2C_CH;

typedef enum
{
    GPIO_I2C_CH0_SCL  = 2,
    GPIO_I2C_CH0_SDA  = 3,

    GPIO_I2C_CH1_SCL  = 4,
    GPIO_I2C_CH1_SDA  = 5,

    GPIO_I2C_CH2_SCL  = 6,
    GPIO_I2C_CH2_SDA  = 7,
} GPIO_I2C_CH_PAD;

typedef enum // SDAD = SCLD EQUAL
{
    ALT_I2C_CH0 = 1,
    ALT_I2C_CH1 = 1,
    ALT_I2C_CH2 = 1,
} ALT_I2C_CH;

typedef enum
{
    I2C_CH0 = 0,
    I2C_CH1 = 1,
    I2C_CH2 = 2,
} I2C_CH;

typedef enum
{
    RSTIDX_I2C_CH0 = RESETINDEX_OF_I2C0_MODULE_PRESETn,
    RSTIDX_I2C_CH1 = RESETINDEX_OF_I2C1_MODULE_PRESETn,
    RSTIDX_I2C_CH2 = RESETINDEX_OF_I2C2_MODULE_PRESETn,
} RSTIDX_I2C_CH;

typedef enum
{
    CLKIDX_I2C_CH0 = CLOCKINDEX_OF_I2C0_MODULE,
    CLKIDX_I2C_CH1 = CLOCKINDEX_OF_I2C1_MODULE,
    CLKIDX_I2C_CH2 = CLOCKINDEX_OF_I2C2_MODULE,
} CLKIDX_I2C_CH;

typedef enum
{
    I2C_INT_BITNUM00 = 15,
    I2C_INT_BITNUM01 = 16,
    I2C_INT_BITNUM02 = 17,
} I2C_INT_BITNUM;

//#define FT5X0X_NAME             "ft5x0x_ts"
//#define FT5X0X_PT_MAX           5
//struct ft5x0x_event 
//{
//    int touch_point;
//	  
//	u16 x[FT5X0X_PT_MAX];
//	u16 y[FT5X0X_PT_MAX];
//	    
//	u16 pressure;
//};

//struct ft5x0x_ts_data 
//{
//	struct ft5x0x_event event;
//	    
//	u32 gpio_irq;
//	u32 gpio_wakeup;
//    u32 gpio_reset;
//	   
//	int screen_max_x;
//	int screen_max_y;
//	int pressure_max;
//};


void I2C_Util_Init( void );
void I2C_Util_DeInit( void );

void I2C_IO_Init( I2C_CMD* CmdSet );
//void I2C_IO_DeInit( I2C_CMD* CmdSet );

//int i2c_master_write( u32 channel, u8 slaveaddr, u8 subaddr, u32 size, u8* buffer );
int i2c_master_read( u32 channel, u8 slaveaddr, u8 subaddr, u32 size, u8* buffer );
//int I2C_MasterWrite( I2C_CMD* CmdSet, u8* WriteData );
int I2C_MasterRead( I2C_CMD* CmdSet, u8* ReadData );

int i2c_init(void);

#ifdef __cplusplus
}
#endif

#endif
