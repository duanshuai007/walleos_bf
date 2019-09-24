#ifndef _ENV_INCLUDE_S5P4418_GPIO_H_
#define _ENV_INCLUDE_S5P4418_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>

/*------------------------------------------------------------------------------
  *  GPIO function config type
  */
#define PAD_MODE_SHIFT          0
#define PAD_FUNC_SHIFT          8
#define PAD_LEVEL_SHIFT         12
#define PAD_PULLUP_SHIFT        16
#define PAD_STRENGTH_SHIFT      20

#define PAD_GET_GROUP(pad)      ((pad >> 0x5) & 0x07)       /* Divide 32 */
#define PAD_GET_BITNO(pad)      ((pad & 0x1F) >> 0x0)
#define PAD_GET_FUNC(pad)       (0xF & (pad >> PAD_FUNC_SHIFT))
#define PAD_GET_MODE(pad)       (0xF & (pad >> PAD_MODE_SHIFT))
#define PAD_GET_LEVEL(pad)      (0xF & (pad >> PAD_LEVEL_SHIFT))
#define PAD_GET_PULLUP(pad)     (0xF & (pad >> PAD_PULLUP_SHIFT))
#define PAD_GET_STRENGTH(pad)   (0xF & (pad >> PAD_STRENGTH_SHIFT))
#define PAD_GET_BUSPAD(pad)     (pad & 0xFF)


/* altfunction, refer to NX_GPIO_PADFUNC in nx_gpio.h */
enum {
	PAD_FUNC_ALT0   = (0 << PAD_FUNC_SHIFT),
	PAD_FUNC_ALT1   = (1 << PAD_FUNC_SHIFT),
	PAD_FUNC_ALT2   = (2 << PAD_FUNC_SHIFT),
	PAD_FUNC_ALT3   = (3 << PAD_FUNC_SHIFT),
};

/* gpio mode, altfunction, gpio in/out or interrput */
enum {
	PAD_MODE_ALT    = (0 << PAD_MODE_SHIFT),
	PAD_MODE_IN     = (1 << PAD_MODE_SHIFT),
	PAD_MODE_OUT    = (2 << PAD_MODE_SHIFT),
	PAD_MODE_INT    = (3 << PAD_MODE_SHIFT),
};

/* ouput level or interrupt detect mode, refer to NX_GPIO_INTMODE in nx_gpio.h */
enum {
	PAD_LEVEL_LOW         = (0 << PAD_LEVEL_SHIFT),     /* if alive, async lowlevel */
	PAD_LEVEL_HIGH        = (1 << PAD_LEVEL_SHIFT),     /* if alive, async highlevel */
	PAD_LEVEL_FALLINGEDGE = (2 << PAD_LEVEL_SHIFT),     /* if alive, async fallingedge */
	PAD_LEVEL_RISINGEDGE  = (3 << PAD_LEVEL_SHIFT),     /* if alive, async eisingedge */
	PAD_LEVEL_LOW_SYNC    = (4 << PAD_LEVEL_SHIFT),     /* if gpio , not support */
	PAD_LEVEL_HIGH_SYNC   = (5 << PAD_LEVEL_SHIFT),     /* if gpio , not support */
	PAD_LEVEL_BOTHEDGE    = (4 << PAD_LEVEL_SHIFT),     /* if alive, not support */
	PAD_LEVEL_ALT         = (6 << PAD_LEVEL_SHIFT),     /* if pad function is alt, not set */
};

enum {
	PAD_PULL_DN     = (0 << PAD_PULLUP_SHIFT),          /* Do not support Alive-GPIO */
	PAD_PULL_UP     = (1 << PAD_PULLUP_SHIFT),
	PAD_PULL_OFF    = (2 << PAD_PULLUP_SHIFT),
};

enum {
	PAD_STRENGTH_0 = (0 << PAD_STRENGTH_SHIFT),
	PAD_STRENGTH_1 = (1 << PAD_STRENGTH_SHIFT),
	PAD_STRENGTH_2 = (2 << PAD_STRENGTH_SHIFT),
	PAD_STRENGTH_3 = (3 << PAD_STRENGTH_SHIFT),
};

/* alive wakeup detect mode */
enum {
	PAD_WAKEUP_ASYNC_LOWLEVEL   = (0),
	PAD_WAKEUP_ASYNC_HIGHLEVEL  = (1),
	PAD_WAKEUP_FALLINGEDGE      = (2),
	PAD_WAKEUP_RISINGEDGE       = (3),
	PAD_WAKEUP_SYNC_LOWLEVEL    = (4),
	PAD_WAKEUP_SYNC_HIGHLEVEL   = (5),
};


enum gpio_base{
    S5P4418_GPIOA_BASE = 0xC001A000,
    S5P4418_GPIOB_BASE = 0xC001B000,
    S5P4418_GPIOC_BASE = 0xC001C000,
    S5P4418_GPIOD_BASE = 0xC001D000,
    S5P4418_GPIOE_BASE = 0xC001E000,    
};

enum gpio_reg{
    GPIO_OUT                      =  0x00,
    GPIO_OUTENB                   =  0x04,
    GPIO_DETMODE0                 =  0x08,
    GPIO_DETMODE1                 =  0x0C,
    GPIO_INTENB                   =  0x10,
    GPIO_DET                      =  0x14,
    GPIO_PAD                      =  0x18,
    GPIO_ALTFN0                   =  0x20,
    GPIO_ALTFN1                   =  0x24,
    GPIO_DETMODEEX                =  0x28,
    GPIO_DETENB                   =  0x3C,
    GPIO_SLEW                     =  0x40,
    GPIO_SLEW_DISABLE_DEFAULT     =  0x44,
    GPIO_DRV1                     =  0x48,
    GPIO_DRV1_DISABLE_DEFAULT     =  0x4C,
    GPIO_DRV0                     =  0x50,
    GPIO_DRV0_DISABLE_DEFAULT     =  0x54,
    GPIO_PULLSEL                  =  0x58,
    GPIO_PULLSEL_DISABLE_DEFAULT  =  0x5C,
    GPIO_PULLENB                  =  0x60,
    GPIO_PULLENB_DISABLE_DEFAULT  =  0x64,
};

#define S5P4418_GPIOA(x)                (x)
#define S5P4418_GPIOB(x)                (x + 32)
#define S5P4418_GPIOC(x)                (x + 64)
#define S5P4418_GPIOD(x)                (x + 96)
#define S5P4418_GPIOE(x)                (x + 128)
#define S5P4418_GPIOALV(x)              (x + 160)

/*  gpio group pad start num. */
enum {
	PAD_GPIO_A      = (0 * 32),
	PAD_GPIO_B      = (1 * 32),
	PAD_GPIO_C      = (2 * 32),
	PAD_GPIO_D      = (3 * 32),
	PAD_GPIO_E      = (4 * 32),
	PAD_GPIO_ALV    = (5 * 32),
};

enum gpio_ctrl{
    GPIO_PINS_NUMBER             =   32,
    GPIO_DET_MODE_RESERVED       =   0xf,
    GPIO_DET_MODE_LOW_LEVEL      =   0x0,
    GPIO_DET_MODE_HIGH_LEVEL     =   0x1,
    GPIO_DET_MODE_FALLING_EDGE   =   0x2,
    GPIO_DET_MODE_RISING_EDGE    =   0x3,
    GPIO_DET_MODE_BOTH_EDGE      =   0x4,
    GPIO_DET_MODE_MASK           =   0x3,
    GPIO_DET_MODE_NO_MASK        =   0x4,
    GPIO_LOW_SIXTEEN             =   16,
    GPIO_HIGH_SIXTEEN            =   32,
    BIT_ENABLE                   =   1,
    CLEAR_INTERRUPT              =   1,
};

enum{
    ALTFH0 = 0,
    ALTFH1 = 1,
    ALTFH2 = 2,
    ALTFH3 = 3,
    ALTFH_MASK = 0x3,
};

struct gpiochip_list_t
{
    struct gpiochip_t * chip;
    struct list_head entry;
};

struct s5p4418_gpiochip_data_t
{
    const char * name;
    int base;
    int ngpio;
    physical_addr_t regbase;
};

enum gpio_pull_t {
    GPIO_PULL_UP            = 0,
    GPIO_PULL_DOWN          = 1,
    GPIO_PULL_NONE          = 2,
};

enum gpio_drv_t {
    GPIO_DRV_LOW            = 0,
    GPIO_DRV_MEDIAN         = 1,
    GPIO_DRV_HIGH           = 2,
};

enum gpio_rate_t {
    GPIO_RATE_SLOW          = 0,
    GPIO_RATE_FAST          = 1,
};

enum gpio_direction_t {
    GPIO_DIRECTION_INPUT    = 0,
    GPIO_DIRECTION_OUTPUT   = 1,
    GPIO_DIRECTION_UNKOWN   = 2,
};

struct gpiochip_t
{
    const char * name;
    int base;
    int ngpio;

    void (*set_cfg)(struct gpiochip_t * chip, int offset, int cfg);
    int  (*get_cfg)(struct gpiochip_t * chip, int offset);
    void (*set_pull)(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull);
    enum gpio_pull_t (*get_pull)(struct gpiochip_t * chip, int offset);
    void (*set_drv)(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv);
    enum gpio_drv_t (*get_drv)(struct gpiochip_t * chip, int offset);
    void (*set_rate)(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate);
    enum gpio_rate_t (*get_rate)(struct gpiochip_t * chip, int offset);
    void (*set_dir)(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir);
    enum gpio_direction_t (*get_dir)(struct gpiochip_t * chip, int offset);
    void (*set_value)(struct gpiochip_t * chip, int offset, int value);
    int  (*get_value)(struct gpiochip_t * chip, int offset);

    void * priv;
};

struct gpiochip_t * search_gpiochip(const char * name);
boolean register_gpiochip(struct gpiochip_t * chip); 
boolean unregister_gpiochip(struct gpiochip_t * chip);

int gpio_is_valid(int no);
void gpio_set_cfg(int no, int cfg);
int gpio_get_cfg(int no); 
void gpio_set_pull(int no, enum gpio_pull_t pull);
enum gpio_pull_t gpio_get_pull(int no);
void gpio_set_drv(int no, enum gpio_drv_t drv);
enum gpio_drv_t gpio_get_drv(int no);
void gpio_set_rate(int no, enum gpio_rate_t rate);
enum gpio_rate_t gpio_get_rate(int no);
void gpio_set_direction(int no, enum gpio_direction_t dir);
enum gpio_direction_t gpio_get_direction(int no);
void gpio_set_value(int no, int value);
int gpio_get_value(int no); 
void gpio_direction_output(int no, int value);
void gpio_direction_input(int no);

void s5p4418_gpiochip_init(void);
void gpio_direction_input(int no);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */

