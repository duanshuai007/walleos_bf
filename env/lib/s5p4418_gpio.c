#include <errno.h>
#include <types.h>
#include <s5p4418_gpio.h>
#include <io.h>
#include <alloc.h>
#include <sizes.h>
#include <list.h>
#include <string.h>

struct gpiochip_list_t __gpiochip_list = { 
    .entry = { 
        .next   = &(__gpiochip_list.entry),
        .prev   = &(__gpiochip_list.entry),
    },  
};

static inline boolean __GET_BOOL_VALUE(u32 x)
{
    return !!x;
}

static struct gpiochip_t * search_gpiochip_with_no(int no)
{
    struct gpiochip_list_t * pos, * n;

    list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
    {
        if( (no >= pos->chip->base) && (no < (pos->chip->base + pos->chip->ngpio)) )
            return pos->chip;
    }

    return NULL;
}

struct gpiochip_t * search_gpiochip(const char * name)
{
    struct gpiochip_list_t * pos, * n;

    if(!name)
        return NULL;

    list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
    {
        if(strcmp(pos->chip->name, name) == 0)
            return pos->chip;
    }

    return NULL;
}

boolean register_gpiochip(struct gpiochip_t * chip)
{
    struct gpiochip_list_t * gl;

    if(!chip || !chip->name)
        return FALSE;

    if(search_gpiochip(chip->name))
        return FALSE;

    gl = malloc(sizeof(struct gpiochip_list_t));
    if(!gl)
        return FALSE;

    gl->chip = chip;
    list_add_tail(&gl->entry, &(__gpiochip_list.entry));

    return TRUE;
}

boolean unregister_gpiochip(struct gpiochip_t * chip)
{
    struct gpiochip_list_t * pos, * n;

    if(!chip || !chip->name)
        return FALSE;

    list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
    {
        if(pos->chip == chip)
        {
            list_del(&(pos->entry));
            free(pos);
            return TRUE;
        }
    }

    return FALSE;
}

int gpio_is_valid(int no)
{
    return search_gpiochip_with_no(no) ? 1 : 0;
}

void gpio_set_cfg(int no, int cfg)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_cfg)
        chip->set_cfg(chip, no - chip->base, cfg);
}

int gpio_get_cfg(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_cfg)
        return chip->get_cfg(chip, no - chip->base);
    return 0;
}

void gpio_set_pull(int no, enum gpio_pull_t pull)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_pull)
        chip->set_pull(chip, no - chip->base, pull);
}

enum gpio_pull_t gpio_get_pull(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_pull)
        return chip->get_pull(chip, no - chip->base);
    return GPIO_PULL_NONE;
}

void gpio_set_drv(int no, enum gpio_drv_t drv)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_drv)
        chip->set_drv(chip, no - chip->base, drv);
}

enum gpio_drv_t gpio_get_drv(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_drv)
        return chip->get_drv(chip, no - chip->base);
    return GPIO_DRV_LOW;
}

void gpio_set_rate(int no, enum gpio_rate_t rate)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_rate)
        chip->set_rate(chip, no - chip->base, rate);
}

enum gpio_rate_t gpio_get_rate(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_rate)
        return chip->get_rate(chip, no - chip->base);
    return GPIO_RATE_SLOW;
}

void gpio_set_direction(int no, enum gpio_direction_t dir)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_dir)
        chip->set_dir(chip, no - chip->base, dir);
}

enum gpio_direction_t gpio_get_direction(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_dir)
        return chip->get_dir(chip, no - chip->base);
    return GPIO_DIRECTION_UNKOWN;
}

void gpio_set_value(int no, int value)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_value)
        chip->set_value(chip, no - chip->base, value);
}

int gpio_get_value(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->get_value)
        return chip->get_value(chip, no - chip->base);
    return 0;
}

void gpio_direction_output(int no, int value)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(!chip)
        return;

    if(chip->set_dir)
        chip->set_dir(chip, no - chip->base, GPIO_DIRECTION_OUTPUT);

    if(chip->set_value)
        chip->set_value(chip, no - chip->base, value);
}

void gpio_direction_input(int no)
{
    struct gpiochip_t * chip = search_gpiochip_with_no(no);

    if(chip && chip->set_dir)
        chip->set_dir(chip, no - chip->base, GPIO_DIRECTION_INPUT);
}

static void s5p4418_gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    if(offset < GPIO_LOW_SIXTEEN)
    {
        offset = offset << 0x1;
        val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN0));
        val &= ~(GPIO_DET_MODE_MASK << offset);
        val |= cfg << offset;
        write32(phys_to_virt(dat->regbase + GPIO_ALTFN0), val);
    }
    else if(offset < GPIO_HIGH_SIXTEEN)
    {
        offset = (offset - GPIO_LOW_SIXTEEN) << 0x1;
        val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN1));
        val &= ~(GPIO_DET_MODE_MASK << offset);
        val |= cfg << offset;
        write32(phys_to_virt(dat->regbase + GPIO_ALTFN1), val);
    }
}

static int s5p4418_gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return 0;

    if(offset < GPIO_LOW_SIXTEEN)
    {
        offset = offset << 0x1;
        val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN0));
        return ((val >> offset) & GPIO_DET_MODE_MASK);
    }
    else if(offset < GPIO_HIGH_SIXTEEN)
    {
        offset = (offset - GPIO_LOW_SIXTEEN) << 0x1;
        val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN1));
        return ((val >> offset) & GPIO_DET_MODE_MASK);
    }

    return 0;
}

static void s5p4418_gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    switch(pull)
    {
        case GPIO_PULL_UP:
            val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_PULLSEL), val);
            val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
            break;

        case GPIO_PULL_DOWN:
            val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_PULLSEL), val);
            val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
            break;

        case GPIO_PULL_NONE:
            val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
            break;

        default:
            break;
    }

    val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL_DISABLE_DEFAULT));
    val |= 1 << offset;
    write32(phys_to_virt(dat->regbase + GPIO_PULLSEL_DISABLE_DEFAULT), val);
    val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB_DISABLE_DEFAULT));
    val |= 1 << offset;
    write32(phys_to_virt(dat->regbase + GPIO_PULLENB_DISABLE_DEFAULT), val);
}

static enum gpio_pull_t s5p4418_gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return GPIO_PULL_NONE;

    val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
    if(!((val >> offset) & 0x1))
    {
        val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
        if((val >> offset) & 0x1)
            return GPIO_PULL_UP;
        else
            return GPIO_PULL_DOWN;
    }
    return GPIO_PULL_NONE;
}

static void s5p4418_gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    switch(drv)
    {
        case GPIO_DRV_LOW:
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
            break;

        case GPIO_DRV_MEDIAN:
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
            break;

        case GPIO_DRV_HIGH:
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
            val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
            break;

        default:
            break;
    }

    val = read32(phys_to_virt(dat->regbase + GPIO_DRV0_DISABLE_DEFAULT));
    val |= 1 << offset;
    write32(phys_to_virt(dat->regbase + GPIO_DRV0_DISABLE_DEFAULT), val);
    val = read32(phys_to_virt(dat->regbase + GPIO_DRV1_DISABLE_DEFAULT));
    val |= 1 << offset;
    write32(phys_to_virt(dat->regbase + GPIO_DRV1_DISABLE_DEFAULT), val);
}

static enum gpio_drv_t s5p4418_gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val, d;

    if(offset >= chip->ngpio)
        return GPIO_DRV_LOW;

    val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
    d = (val >> offset) & 0x1;
    val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
    d |= ((val >> offset) & 0x1) << 1;

    switch(d)
    {
        case 0x0:
            return GPIO_DRV_LOW;
        case 0x1:
            return GPIO_DRV_MEDIAN;
        case 0x2:
            return GPIO_DRV_MEDIAN;
        case 0x3:
            return GPIO_DRV_HIGH;
        default:
            break;
    }
    return GPIO_DRV_LOW;
}

static void s5p4418_gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    switch(rate)
    {
        case GPIO_RATE_SLOW:
            val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
            val |= 1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_SLEW), val);
            break;

        case GPIO_RATE_FAST:
            val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
            val &= ~(1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_SLEW), val);
            break;

        default:
            break;
    }

    val = read32(phys_to_virt(dat->regbase + GPIO_SLEW_DISABLE_DEFAULT));
    val |= 1 << offset;
    write32(phys_to_virt(dat->regbase + GPIO_SLEW_DISABLE_DEFAULT), val);
}

static enum gpio_rate_t s5p4418_gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return GPIO_RATE_SLOW;

    val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
    if((val >> offset) & 0x1)
        return GPIO_RATE_SLOW;
    else
        return GPIO_RATE_FAST;
}

static void s5p4418_gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    switch(dir)
    {
        case GPIO_DIRECTION_INPUT:
            val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
            val &= ~(0x1 << offset);
            write32(phys_to_virt(dat->regbase + GPIO_OUTENB), val);
            break;

        case GPIO_DIRECTION_OUTPUT:
            val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
            val |= 0x1 << offset;
            write32(phys_to_virt(dat->regbase + GPIO_OUTENB), val);
            break;

        default:
            break;
    }
}

static enum gpio_direction_t s5p4418_gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val, d;

    if(offset >= chip->ngpio)
        return GPIO_DIRECTION_UNKOWN;

    val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
    d = (val >> offset) & 0x1;
    switch(d)
    {
        case GPIO_DIRECTION_INPUT:
            return GPIO_DIRECTION_INPUT;
        case GPIO_DIRECTION_OUTPUT:
            return GPIO_DIRECTION_OUTPUT;
        default:
            break;
    }
    return GPIO_DIRECTION_UNKOWN;
}

static void s5p4418_gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return;

    val = read32(phys_to_virt(dat->regbase + GPIO_OUT));
    val &= ~(1 << offset);
    val |= (__GET_BOOL_VALUE(value)) << offset;
    write32(phys_to_virt(dat->regbase + GPIO_OUT), val);
}

static int s5p4418_gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
    struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
    u32 val;

    if(offset >= chip->ngpio)
        return 0;

    val = read32(phys_to_virt(dat->regbase + GPIO_PAD));
    return __GET_BOOL_VALUE(val & (1 << offset));
}

static struct s5p4418_gpiochip_data_t gpiochip_datas[] = {
    {
        .name       = "GPIOA",
        .base       = S5P4418_GPIOA(0),
        .ngpio      = GPIO_PINS_NUMBER,
        .regbase    = S5P4418_GPIOA_BASE,
    }, {
        .name       = "GPIOB",
        .base       = S5P4418_GPIOB(0),
        .ngpio      = GPIO_PINS_NUMBER,
        .regbase    = S5P4418_GPIOB_BASE,
    }, {
        .name       = "GPIOC",
        .base       = S5P4418_GPIOC(0),
        .ngpio      = GPIO_PINS_NUMBER,
        .regbase    = S5P4418_GPIOC_BASE,
    }, {
        .name       = "GPIOD",
        .base       = S5P4418_GPIOD(0),
        .ngpio      = GPIO_PINS_NUMBER,
        .regbase    = S5P4418_GPIOD_BASE,
    }, {
        .name       = "GPIOE",
        .base       = S5P4418_GPIOE(0),
        .ngpio      = GPIO_PINS_NUMBER,
        .regbase    = S5P4418_GPIOE_BASE,
    }
};

void s5p4418_gpiochip_init(void)
{
    struct gpiochip_t * chip;
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(gpiochip_datas); i++)
    {
        chip = malloc(sizeof(struct gpiochip_t));
        if(!chip)
            continue;

        chip->name = gpiochip_datas[i].name;
        chip->base = gpiochip_datas[i].base;
        chip->ngpio = gpiochip_datas[i].ngpio;
        chip->set_cfg = s5p4418_gpiochip_set_cfg;
        chip->get_cfg = s5p4418_gpiochip_get_cfg;
        chip->set_pull = s5p4418_gpiochip_set_pull;
        chip->get_pull = s5p4418_gpiochip_get_pull;
        chip->set_drv = s5p4418_gpiochip_set_drv;
        chip->get_drv = s5p4418_gpiochip_get_drv;
        chip->set_rate = s5p4418_gpiochip_set_rate;
        chip->get_rate = s5p4418_gpiochip_get_rate;
        chip->set_dir = s5p4418_gpiochip_set_dir;
        chip->get_dir = s5p4418_gpiochip_get_dir;
        chip->set_value = s5p4418_gpiochip_set_value;
        chip->get_value = s5p4418_gpiochip_get_value;
        chip->priv = &gpiochip_datas[i];

        register_gpiochip(chip);
    }
}

void s5p4418_gpiochip_exit(void)
{
    struct gpiochip_t * chip;
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(gpiochip_datas); i++)
    {
        chip = search_gpiochip(gpiochip_datas[i].name);
        if(!chip)
            continue;
        unregister_gpiochip(chip);
        free(chip);
    }
}
