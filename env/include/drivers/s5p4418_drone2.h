/*
 * (C) Copyright 2009 Nexell Co.,
 * jung hyun kim<jhkim@nexell.co.kr>
 *
 * Configuation settings for the Nexell board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ENV_INCLUDE_DRIVERS_S5P4418_CONFIG_H_
#define _ENV_INCLUDE_DRIVERS_S5P4418_CONFIG_H_

#define HZ                              1000
#define INITIAL_JIFFIES					((unsigned long)(unsigned int) (-0*HZ))
#define NSEC_PER_SEC					1000000		//1MHZ
#define CLOCKHZ                         1000000     //1MHZ

#define SDHC_CLK                        100000000   //100MHZ

#define KERNEL_TIMERS                   20
/*
 * ABOUT MMU
 */
#define PAGE_SIZE                       4096
#define CONFIG_MMU_ENABLE
#define CONFIG_MACH_S5P4418
#define CACHE_LINE_SIZE                 32

/*
 *  TASK MACROS
 */
#define MAX_TASKS                       20
#define MAX_TASK_LEVELS                 10 //level : 0-9
#define MAIN_TASK_LEVEL                 8 // main task level 8
#define TASK_STACK_SIZE                 0x4000
#define MAX_SHEET_SIZE_PER_TASK         10

#define TASK_SWTICH_TIME                0.01 // 定时器任务切换最小时间间隔，单位秒

#define CONFIG_MACH_S5P4418
/*
 *  LCD DISPLAY MARCOS
 */
#define SCREEN_HORIZONTAL
#ifdef SCREEN_HORIZONTAL
#define CFG_DISP_PRI_RESOL_WIDTH        1280
#define CFG_DISP_PRI_RESOL_HEIGHT       800
#else
#define CFG_DISP_PRI_RESOL_WIDTH        800
#define CFG_DISP_PRI_RESOL_HEIGHT       1280
#endif

#define CFG_DISP_PRI_SCREEN_PIXEL_BYTE  4
/*
 *      PHOTO CONFIG
 */
#define MAX_ZOOMED 1.5 //最大缩放为sheet的比例
#define MIN_ZOOMED 0.5 //最小缩放为图片的比例
#define ZOOM_COUNT 10  //双击放大次数
#define MOVE_COUNT 10  //移动次数
#define ZOOM_CYC   0.5 //放大周期
#define MOVE_CYC   0.5 //图片移动周期

#define UNUSED(x)   (void)(x)
/*-----------------------------------------------------------------------
 *  System memory Configuration
 */
#define CONFIG_RELOC_TO_TEXT_BASE                                               /* Relocate u-boot code to TEXT_BASE */

//#define CONFIG_SYS_TEXT_BASE            0x42C00000
#define CONFIG_SYS_INIT_SP_ADDR         CONFIG_SYS_TEXT_BASE                    /* init and run stack pointer */

/* when CONFIG_LCD */
#define CONFIG_FB_ADDR                  0x46000000
#define CONFIG_BMP_ADDR                 0x47000000

/* Download OFFSET */
#define CONFIG_MEM_LOAD_ADDR            0x48000000

/*-----------------------------------------------------------------------
 *  High Level System Configuration
 */
#undef  CONFIG_USE_IRQ                                                          /* Not used: not need IRQ/FIQ stuff */
#define CONFIG_SYS_HZ                   1000                                    /* decrementer freq: 1ms ticks */

#define CONFIG_SYS_SDRAM_BASE           CFG_MEM_PHY_SYSTEM_BASE                 /* board_init_f */
#define CONFIG_SYS_SDRAM_SIZE           CFG_MEM_PHY_SYSTEM_SIZE

#define CONFIG_NR_DRAM_BANKS            1                                       /* dram 1 bank num */

#define CONFIG_SYS_MALLOC_END           (CONFIG_MEM_MALLOC_START + CONFIG_MEM_MALLOC_LENGTH)    /* relocate_code and  board_init_r */
#define CONFIG_SYS_MALLOC_LEN           (CONFIG_MEM_MALLOC_LENGTH - 0x8000)                     /* board_init_f, more than 2M for ubifs */

#define CONFIG_SYS_LOAD_ADDR            CONFIG_MEM_LOAD_ADDR                    /* kernel load address */

#define CONFIG_SYS_MEMTEST_START        CONFIG_SYS_MALLOC_END                   /* memtest works on */
#define CONFIG_SYS_MEMTEST_END          (CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE)

/*-----------------------------------------------------------------------
 *  System initialize options (board_init_f)
 */
#define CONFIG_ARCH_CPU_INIT                                                    /* board_init_f->init_sequence, call arch_cpu_init */
#define CONFIG_BOARD_EARLY_INIT_F                                               /* board_init_f->init_sequence, call board_early_init_f */
#define CONFIG_BOARD_LATE_INIT                                                  /* board_init_r, call board_early_init_f */
#define CONFIG_DISPLAY_CPUINFO                                                  /* board_init_f->init_sequence, call print_cpuinfo */
#define CONFIG_SYS_DCACHE_OFF                                                   /* board_init_f, CONFIG_SYS_ICACHE_OFF */
#define CONFIG_ARCH_MISC_INIT                                                   /* board_init_r, call arch_misc_init */
//#define   CONFIG_SYS_ICACHE_OFF

#define CONFIG_MMU_ENABLE
#ifdef  CONFIG_MMU_ENABLE
#undef  CONFIG_SYS_DCACHE_OFF
#endif

/*-----------------------------------------------------------------------
 *  U-Boot default cmd
 */
#define CONFIG_CMD_MEMORY   /* md mm nm mw cp cmp crc base loop mtest */
/* remove by cym 20150908 */
#if 1
#define CONFIG_CMD_NET      /* bootp, tftpboot, rarpboot    */
#endif
/* end remove */
#define CONFIG_CMD_RUN      /* run command in env variable  */
#define CONFIG_CMD_SAVEENV  /* saveenv          */
#define CONFIG_CMD_SOURCE   /* "source" command support */
#define CONFIG_CMD_BOOTD    /* "boot" command support */
/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
/* modify by cym 20151127 */
#if 0
#define CONFIG_SYS_PROMPT               "s5p4418# "                                             /* Monitor Command Prompt   */
#else
#define CONFIG_SYS_PROMPT               "iTOP-4418# "
#endif
/* end modify */
#define CONFIG_SYS_LONGHELP                                                                 /* undef to save memory    */
#define CONFIG_SYS_CBSIZE               1024                                                    /* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)    /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS              16                                                  /* max number of command args   */
#define CONFIG_SYS_BARGSIZE             CONFIG_SYS_CBSIZE                                   /* Boot Argument Buffer Size    */

/*-----------------------------------------------------------------------
 * allow to overwrite serial and ethaddr
 */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_SYS_HUSH_PARSER          /* use "hush" command parser    */
#ifdef  CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2  "> "
#endif

/*-----------------------------------------------------------------------
 * Etc Command definition
 */
#define CONFIG_CMD_BDI                  /* board info   */
#define CONFIG_CMD_IMI                  /* image info   */
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_RUN                  /* run commands in an environment variable  */
#define CONFIG_CMDLINE_EDITING          /* add command line history */
#define CONFIG_CMDLINE_TAG              /* use bootargs commandline */
//#define CONFIG_SETUP_MEMORY_TAGS
//#define CONFIG_INITRD_TAG
//#define CONFIG_SERIAL_TAG
//#define CONFIG_REVISION_TAG

#undef  CONFIG_BOOTM_RTEMS

/*-----------------------------------------------------------------------
 * Ethernet configuration
 * depend on CONFIG_CMD_NET
 */
/* remove by cym 20150908 */
#if 0
#define CONFIG_DRIVER_DM9000            1
#endif
#define CONFIG_PHY_REALTEK
#define CONFIG_PHYLIB
/* end remove */

#if defined(CONFIG_CMD_NET)
    /* DM9000 Ethernet device */
    #if defined(CONFIG_DRIVER_DM9000)
    #define CONFIG_DM9000_BASE          CFG_ETHER_EXT_PHY_BASEADDR      /* DM9000: 0x04000000(CS1) */
    #define DM9000_IO                   CONFIG_DM9000_BASE
    #define DM9000_DATA                 (CONFIG_DM9000_BASE + 0x4)
//  #define CONFIG_DM9000_DEBUG
    #endif
#endif

/*-----------------------------------------------------------------------
 * NAND FLASH
 */

#if defined(CONFIG_NAND_FTL) && defined(CONFIG_NAND_MTD)
#error "Duplicated config for NAND Driver!!!"
#endif

#if defined(CONFIG_NAND_FTL)
#define HAVE_BLOCK_DEVICE
#endif

#if defined(CONFIG_CMD_NAND)
    #if !defined(CONFIG_NAND_FTL) && !defined(CONFIG_NAND_MTD)
    #error "Select FTL or MTD for NAND Driver!!!"
    #endif

    #define CONFIG_SYS_MAX_NAND_DEVICE      (1)
    #define CONFIG_SYS_NAND_MAX_CHIPS       (1)
    #define CONFIG_SYS_NAND_BASE            PHY_BASEADDR_CS_NAND                            /* Nand data register, nand->IO_ADDR_R/_W */

    #if defined(CONFIG_ENV_IS_IN_NAND)
        #define CONFIG_ENV_OFFSET           (0x1000000)                                 /* 4MB */
        #define CONFIG_ENV_SIZE             (0x100000)                                  /* 1 block size */
        #define CONFIG_ENV_RANGE            (0x400000)                                  /* avoid bad block */
    #endif
#endif

#if defined(CONFIG_NAND_MTD)
    #define CONFIG_SYS_NAND_ONFI_DETECTION
    #define CONFIG_CMD_NAND_TRIMFFS

    #define CONFIG_MTD_NAND_NXP
//  #define CONFIG_MTD_NAND_ECC_BCH                                                         /* sync kernel config */
    #define CONFIG_MTD_NAND_ECC_HW
//  #define CONFIG_MTD_NAND_VERIFY_WRITE
//  #define CONFIG_MTD_NAND_BMT_FIRST_LAST                                                  /* Samsumg 8192 page nand write bad mark on 1st and last block */

    #define CONFIG_CMD_UPDATE_NAND

    #if defined (CONFIG_MTD_NAND_ECC_BCH)
        #define CONFIG_BCH
        #define CONFIG_NAND_ECC_BCH
    #endif


    #undef  CONFIG_CMD_IMLS

    #define CONFIG_CMD_MTDPARTS
    #if defined(CONFIG_CMD_MTDPARTS)
        #define CONFIG_MTD_DEVICE
        #define CONFIG_MTD_PARTITIONS
        #define MTDIDS_DEFAULT              "nand0=mtd-nand"
        #define MTDPARTS_DEFAULT            "mtdparts=mtd-nand:2m(u-boot),4m(kernel),8m(ramdisk),-(extra)"
    #endif

//  #define CONFIG_MTD_DEBUG
    #ifdef  CONFIG_MTD_DEBUG
        #define CONFIG_MTD_DEBUG_VERBOSE    0   /* For nand debug message = 0 ~ 3 *//* list all images found in flash   */
    #endif
#endif  /* CONFIG_CMD_NAND */


/*-----------------------------------------------------------------------
 * I2C
 *
 * probe
 *  #> i2c probe
 *
 * speed
 *  #> i2c speed xxxxxx
 *
 * select bus
 *  #> i2c dev n
 *
 * write
 *  #> i2c mw 0x30 0xRR 0xDD 1
 *  - 0x30 = slave, 0xRR = register, 0xDD = data, 1 = write length
 *
 * read
 *  #> i2c md 0x30 0xRR 1
 *  - 0x30 = slave, 0xRR = register, 1 = read length
 *
 */
#define CONFIG_CMD_I2C
#if defined(CONFIG_CMD_I2C)
    #define CONFIG_HARD_I2C
    #define CONFIG_I2C_MULTI_BUS

    #define CONFIG_I2C_GPIO_MODE                            /* gpio i2c */
    #define CONFIG_SYS_I2C_SPEED        100000              /* default speed, 100 khz */

    #define CONFIG_I2C0_NEXELL                              /* 0 = i2c 0 */
    #define CONFIG_I2C0_NO_STOP             1               /* when tx end, 0= generate stop signal , 1: skip stop signal */

    #define CONFIG_I2C1_NEXELL                              /* 1 = i2c 1 */
    #define CONFIG_I2C1_NO_STOP             0               /* when tx end, 0= generate stop signal , 1: skip stop signal */

    #define CONFIG_I2C2_NEXELL                              /* 1 = i2c 1 */
    #define CONFIG_I2C2_NO_STOP             0               /* when tx end, 0= generate stop signal , 1: skip stop signal */

    #define CONFIG_I2C3_NEXELL                              /* 1 = i2c 1 */
    #define CONFIG_I2C3_NO_STOP             1               /* when tx end, 0= generate stop signal , 1: skip stop signal */
#endif

/*-----------------------------------------------------------------------
 * SD/MMC
 *  #> mmcinfo          -> get current device mmc info or detect mmc device
 *  #> mmc rescan       -> rescan mmc device
 *  #> mmc dev 'num'    -> set current sdhc device for mmcinfo or mmc rescan
 *                        (ex. "mmc dev 0" or "mmc dev 1")
 *
 * #> fatls   mmc 0 "directory"
 * #> fatload mmc 0  0x.....    "file"
 *
 */
#define CONFIG_CMD_MMC
#define CONFIG_ENV_IS_IN_MMC

#if defined(CONFIG_CMD_MMC)

    #define CONFIG_MMC
    #define CONFIG_GENERIC_MMC
    #define HAVE_BLOCK_DEVICE

    #define CONFIG_MMC0_ATTACH          TRUE    /* 0 = MMC0 : BOOT */
    #define CONFIG_MMC1_ATTACH          FALSE   /* 1 = MMC1 : External */
    #define CONFIG_MMC2_ATTACH          TRUE    /* 2 = MMC2 */

    #define CONFIG_MMC0_CLOCK           50000000
    #define CONFIG_MMC0_CLK_DELAY       DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0x1c) | DW_MMC_DRIVE_PHASE(2)| DW_MMC_SAMPLE_PHASE(1)

    #define CONFIG_MMC2_CLOCK           50000000
    #define CONFIG_MMC2_CLK_DELAY       DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0x1c) | DW_MMC_DRIVE_PHASE(2)| DW_MMC_SAMPLE_PHASE(1)

    #define CONFIG_DWMMC
    #define CONFIG_NXP_DWMMC
    #define CONFIG_MMC_PARTITIONS
    #define CONFIG_CMD_MMC_UPDATE
    #define CONFIG_SYS_MMC_BOOT_DEV     (2)     /* BOOT MMC DEVICE NUM */

    #if defined(CONFIG_ENV_IS_IN_MMC)
    #define CONFIG_ENV_OFFSET           512*1024                /* 0x00080000 */
    #define CONFIG_ENV_SIZE             32*1024                 /* N block size (512Byte Per Block)  */
    #define CONFIG_ENV_RANGE            CONFIG_ENV_SIZE * 2     /* avoid bad block */
    #define CONFIG_SYS_MMC_ENV_DEV      CONFIG_SYS_MMC_BOOT_DEV
    #endif
#endif

/*-----------------------------------------------------------------------
 *  GPIO LIBRARY
 */
#define CONFIG_GPIOLIB_NXP

/*-----------------------------------------------------------------------
 * Default environment organization
 */
#if !defined(CONFIG_ENV_IS_IN_MMC) && !defined(CONFIG_ENV_IS_IN_NAND) &&    \
    !defined(CONFIG_ENV_IS_IN_FLASH) && !defined(CONFIG_ENV_IS_IN_EEPROM)
    #define CONFIG_ENV_IS_NOWHERE                       /* default: CONFIG_ENV_IS_NOWHERE */
    #define CONFIG_ENV_OFFSET                 1024
    #define CONFIG_ENV_SIZE                 4*1024      /* env size */
    #undef  CONFIG_CMD_IMLS                             /* imls - list all images found in flash, default enable so disable */
#endif

/*-----------------------------------------------------------------------
 * FAT Partition
 */
#if defined(CONFIG_MMC) || defined(CONFIG_CMD_USB)
    #define CONFIG_DOS_PARTITION

    #define CONFIG_CMD_FAT
    #define CONFIG_FS_FAT
    #define CONFIG_FAT_WRITE

    #define CONFIG_CMD_EXT4
    #define CONFIG_CMD_EXT4_WRITE
    #define CONFIG_FS_EXT4
    #define CONFIG_EXT4_WRITE
#endif

#endif /* __CONFIG_H__ */

