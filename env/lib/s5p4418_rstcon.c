/*
 * s5p4418-rstcon.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <types.h>
#include <io.h>
#include <s5p4418_rstcon.h>
#include <s5p4418_tick_delay.h>

static const unsigned int BIT_MASK = 0x1;

extern void __udelay(u32 us);

static void s5p4418_ip_setrst(int id, int reset)
{
    physical_addr_t addr = S5P4418_SYS_IP_RSTCON0;
    u32_t val;

    if(id < IP_RESET_REGISTER_0_32)
        addr = S5P4418_SYS_IP_RSTCON0;
    else if(id < IP_RESET_REGISTER_32_64)
        addr = S5P4418_SYS_IP_RSTCON1;
    else if(id < IP_RESET_REGISTER_64_96)
        addr = S5P4418_SYS_IP_RSTCON2;
    else
        return;

    val = read32(phys_to_virt(addr));
    val &= ~(BIT_SET << (id & RESET_ID_MASK));
    val |= (reset ? BIT_SET : BIT_CLEAR) << (id & RESET_ID_MASK);
    write32(phys_to_virt(addr), val);
}

static int s5p4418_ip_getrst(int id)
{
    physical_addr_t addr = S5P4418_SYS_IP_RSTCON0;
    u32_t val;

    if(id < IP_RESET_REGISTER_0_32)
        addr = S5P4418_SYS_IP_RSTCON0;
    else if(id < IP_RESET_REGISTER_32_64)
        addr = S5P4418_SYS_IP_RSTCON1;
    else if(id < IP_RESET_REGISTER_64_96)
        addr = S5P4418_SYS_IP_RSTCON2;
    else
        return 1;

    val = read32(phys_to_virt(addr));
    return (val >> (id & RESET_ID_MASK)) & BIT_MASK;
}

void s5p4418_ip_reset(int id, int force)
{
    const unsigned int enable_reset = 0;
    const unsigned int no_reset = 1;

    if(force || !s5p4418_ip_getrst(id))
    {
        s5p4418_ip_setrst(id, enable_reset);
        __udelay(10);
        s5p4418_ip_setrst(id, no_reset);
    }
}

