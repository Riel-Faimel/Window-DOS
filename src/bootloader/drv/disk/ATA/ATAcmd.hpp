#include <global/type.hpp>

/**
 * from Linux2.6.32.1
 * include/linux/ata.h
 */
enum class ATA : u32 {
	/* various global constants */
	MAX_DEVICES		= 2,	/* per bus/port */
	MAX_PRD		= 256,	/* we could make these 256/256 */
	SECT_SIZE		= 512,
	MAX_SECTORS_128	= 128,
	MAX_SECTORS		= 256,
	MAX_SECTORS_LBA48	= 65535,/* TODO: 65536? */
    /**
     * I mean, 65535 forced
     * if it is confused, just throw away
     */
	MAX_SECTORS_TAPE	= 65535,

	ID_WORDS		= 256,
	ID_CONFIG		= 0,
	ID_CYLS		= 1,
	ID_HEADS		= 3,
	ID_SECTORS		= 6,
	ID_SERNO		= 10,
	ID_BUF_SIZE		= 21,
	ID_FW_REV		= 23,
	ID_PROD		= 27,
	ID_MAX_MULTSECT	= 47,
	ID_DWORD_IO		= 48,
	ID_CAPABILITY	= 49,
	ID_OLD_PIO_MODES	= 51,
	ID_OLD_DMA_MODES	= 52,
	ID_FIELD_VALID	= 53,
	ID_CUR_CYLS		= 54,
	ID_CUR_HEADS	= 55,
	ID_CUR_SECTORS	= 56,
	ID_MULTSECT		= 59,
	ID_LBA_CAPACITY	= 60,
	ID_SWDMA_MODES	= 62,
	ID_MWDMA_MODES	= 63,
	ID_PIO_MODES	= 64,
	ID_EIDE_DMA_MIN	= 65,
	ID_EIDE_DMA_TIME	= 66,
	ID_EIDE_PIO		= 67,
	ID_EIDE_PIO_IORDY	= 68,
	ID_QUEUE_DEPTH	= 75,
	ID_MAJOR_VER	= 80,
	ID_COMMAND_SET_1	= 82,
	ID_COMMAND_SET_2	= 83,
	ID_CFSSE		= 84,
	ID_CFS_ENABLE_1	= 85,
	ID_CFS_ENABLE_2	= 86,
	ID_CSF_DEFAULT	= 87,
	ID_UDMA_MODES	= 88,
	ID_HW_CONFIG	= 93,
	ID_SPG		= 98,
	ID_LBA_CAPACITY_2	= 100,
	ID_DLST_LUN		= 126,
	ID_DLF		= 128,
	ID_CSFO		= 129,
	ID_CFA_POWER	= 160,
	ID_CFA_KEY_MGMT	= 162,
	ID_CFA_MODES	= 163,
	ID_DSET_MGMT	= 169,
	ID_ROT_SPEED	= 217,
	ID_PIO4		= (1 << 1),

	ID_SERNO_LEN	= 20,
	ID_FW_REV_LEN	= 8,
	ID_PROD_LEN		= 40,

	PCI_CTL_OFS		= 2,

	PIO0		= (1 << 0),
	PIO1		= PIO0 | (1 << 1),
	PIO2		= PIO1 | (1 << 2),
	PIO3		= PIO2 | (1 << 3),
	PIO4		= PIO3 | (1 << 4),
	PIO5		= PIO4 | (1 << 5),
	PIO6		= PIO5 | (1 << 6),

	PIO4_ONLY		= (1 << 4),

	SWDMA0		= (1 << 0),
	SWDMA1		= SWDMA0 | (1 << 1),
	SWDMA2		= SWDMA1 | (1 << 2),

	SWDMA2_ONLY		= (1 << 2),

	MWDMA0		= (1 << 0),
	MWDMA1		= MWDMA0 | (1 << 1),
	MWDMA2		= MWDMA1 | (1 << 2),
	MWDMA3		= MWDMA2 | (1 << 3),
	MWDMA4		= MWDMA3 | (1 << 4),

	MWDMA12_ONLY	= (1 << 1) | (1 << 2),
	MWDMA2_ONLY		= (1 << 2),

	UDMA0		= (1 << 0),
	UDMA1		= UDMA0 | (1 << 1),
	UDMA2		= UDMA1 | (1 << 2),
	UDMA3		= UDMA2 | (1 << 3),
	UDMA4		= UDMA3 | (1 << 4),
	UDMA5		= UDMA4 | (1 << 5),
	UDMA6		= UDMA5 | (1 << 6),
	UDMA7		= UDMA6 | (1 << 7),
	/* UDMA7 is just for completeness... doesn't exist (yet?).  */

	UDMA24_ONLY		= (1 << 2) | (1 << 4),

	UDMA_MASK_40C	= UDMA2,	/* udma0-2 */

	/* DMA-related */
	PRD_SZ		= 8,
	PRD_TBL_SZ		= (MAX_PRD * PRD_SZ),
	PRD_EOT		= (1U << 31),	/* end-of-table flag */

	DMA_TABLE_OFS	= 4,
	DMA_STATUS		= 2,
	DMA_CMD		= 0,
	DMA_WR		= (1 << 3),
	DMA_START		= (1 << 0),
	DMA_INTR		= (1 << 2),
	DMA_ERR		= (1 << 1),
	DMA_ACTIVE		= (1 << 0),

	/* bits in ATA command block registers */
	HOB			= (1 << 7),	/* LBA48 selector */
	NIEN		= (1 << 1),	/* disable-irq flag */
	LBA			= (1 << 6),	/* LBA28 selector */
	DEV1		= (1 << 4),	/* Select Device 1 (slave) */
	DEVICE_OBS		= (1 << 7) | (1 << 5), /* obs bits in dev reg */
	DEVCTL_OBS		= (1 << 3),	/* obsolete bit in devctl reg */
	BUSY		= (1 << 7),	/* BSY status bit */
	DRDY		= (1 << 6),	/* device ready */
	DF			= (1 << 5),	/* device fault */
	DSC			= (1 << 4),	/* drive seek complete */
	DRQ			= (1 << 3),	/* data request i/o */
	CORR		= (1 << 2),	/* corrected data error */
	IDX			= (1 << 1),	/* index */
	ERR			= (1 << 0),	/* have an error */
	SRST		= (1 << 2),	/* software reset */
	ICRC		= (1 << 7),	/* interface CRC error */
	BBK			= ICRC,	/* pre-EIDE: block marked bad */
	UNC			= (1 << 6),	/* uncorrectable media error */
	MC			= (1 << 5),	/* media changed */
	IDNF		= (1 << 4),	/* ID not found */
	MCR			= (1 << 3),	/* media change requested */
	ABORTED		= (1 << 2),	/* command aborted */
	TRK0NF		= (1 << 1),	/* track 0 not found */
	AMNF		= (1 << 0),	/* address mark not found */
	ATAPI_LFS		= 0xF0,		/* last failed sense */
	ATAPI_EOM		= TRK0NF,	/* end of media */
	ATAPI_ILI		= AMNF,	/* illegal length indication */
	ATAPI_IO		= (1 << 1),
	ATAPI_COD		= (1 << 0),

	/* ATA command block registers */
	REG_DATA		= 0x00,
	REG_ERR		= 0x01,
	REG_NSECT		= 0x02,
	REG_LBAL		= 0x03,
	REG_LBAM		= 0x04,
	REG_LBAH		= 0x05,
	REG_DEVICE		= 0x06,
	REG_STATUS		= 0x07,

	REG_FEATURE		= REG_ERR, /* and their aliases */
	REG_CMD		= REG_STATUS,
	REG_BYTEL		= REG_LBAM,
	REG_BYTEH		= REG_LBAH,
	REG_DEVSEL		= REG_DEVICE,
	REG_IRQ		= REG_NSECT,

	/* ATA device commands */
	CMD_DEV_RESET	= 0x08, /* ATAPI device reset */
	CMD_CHK_POWER	= 0xE5, /* check power mode */
	CMD_STANDBY		= 0xE2, /* place in standby power mode */
	CMD_IDLE		= 0xE3, /* place in idle power mode */
	CMD_EDD		= 0x90,	/* execute device diagnostic */
	CMD_DOWNLOAD_MICRO  = 0x92,
	CMD_NOP		= 0x00,
	CMD_FLUSH		= 0xE7,
	CMD_FLUSH_EXT	= 0xEA,
	CMD_ID_ATA		= 0xEC,
	CMD_ID_ATAPI	= 0xA1,
	CMD_SERVICE		= 0xA2,
	CMD_READ		= 0xC8,
	CMD_READ_EXT	= 0x25,
	CMD_READ_QUEUED	= 0x26,
	CMD_READ_STREAM_EXT	= 0x2B,
	CMD_READ_STREAM_DMA_EXT = 0x2A,
	CMD_WRITE		= 0xCA,
	CMD_WRITE_EXT	= 0x35,
	CMD_WRITE_QUEUED	= 0x36,
	CMD_WRITE_STREAM_EXT = 0x3B,
	CMD_WRITE_STREAM_DMA_EXT = 0x3A,
	CMD_WRITE_FUA_EXT	= 0x3D,
	CMD_WRITE_QUEUED_FUA_EXT = 0x3E,
	CMD_FPDMA_READ	= 0x60,
	CMD_FPDMA_WRITE	= 0x61,
	CMD_PIO_READ	= 0x20,
	CMD_PIO_READ_EXT	= 0x24,
	CMD_PIO_WRITE	= 0x30,
	CMD_PIO_WRITE_EXT	= 0x34,
	CMD_READ_MULTI	= 0xC4,
	CMD_READ_MULTI_EXT	= 0x29,
	CMD_WRITE_MULTI	= 0xC5,
	CMD_WRITE_MULTI_EXT	= 0x39,
	CMD_WRITE_MULTI_FUA_EXT = 0xCE,
	CMD_SET_FEATURES	= 0xEF,
	CMD_SET_MULTI	= 0xC6,
	CMD_PACKET		= 0xA0,
	CMD_VERIFY		= 0x40,
	CMD_VERIFY_EXT	= 0x42,
	CMD_WRITE_UNCORR_EXT = 0x45,
	CMD_STANDBYNOW1	= 0xE0,
	CMD_IDLEIMMEDIATE	= 0xE1,
	CMD_SLEEP		= 0xE6,
	CMD_INIT_DEV_PARAMS	= 0x91,
	CMD_READ_NATIVE_MAX	= 0xF8,
	CMD_READ_NATIVE_MAX_EXT = 0x27,
	CMD_SET_MAX		= 0xF9,
	CMD_SET_MAX_EXT	= 0x37,
	CMD_READ_LOG_EXT	= 0x2F,
	CMD_WRITE_LOG_EXT	= 0x3F,
	CMD_READ_LOG_DMA_EXT = 0x47,
	CMD_WRITE_LOG_DMA_EXT = 0x57,
	CMD_TRUSTED_RCV	= 0x5C,
	CMD_TRUSTED_RCV_DMA = 0x5D,
	CMD_TRUSTED_SND	= 0x5E,
	CMD_TRUSTED_SND_DMA = 0x5F,
	CMD_PMP_READ	= 0xE4,
	CMD_PMP_WRITE	= 0xE8,
	CMD_CONF_OVERLAY	= 0xB1,
	CMD_SEC_SET_PASS	= 0xF1,
	CMD_SEC_UNLOCK	= 0xF2,
	CMD_SEC_ERASE_PREP	= 0xF3,
	CMD_SEC_ERASE_UNIT	= 0xF4,
	CMD_SEC_FREEZE_LOCK	= 0xF5,
	CMD_SEC_DISABLE_PASS = 0xF6,
	CMD_CONFIG_STREAM	= 0x51,
	CMD_SMART		= 0xB0,
	CMD_MEDIA_LOCK	= 0xDE,
	CMD_MEDIA_UNLOCK	= 0xDF,
	CMD_DSM		= 0x06,
	CMD_CHK_MED_CRD_TYP = 0xD1,
	CMD_CFA_REQ_EXT_ERR = 0x03,
	CMD_CFA_WRITE_NE	= 0x38,
	CMD_CFA_TRANS_SECT	= 0x87,
	CMD_CFA_ERASE	= 0xC0,
	CMD_CFA_WRITE_MULT_NE = 0xCD,
	/* marked obsolete in the ATA/ATAPI-7 spec */
	CMD_RESTORE		= 0x10,

	/* READ_LOG_EXT pages */
	LOG_SNCQ	= 0x10,

	/* READ/WRITE LONG (obsolete) */
	CMD_READ_LONG	= 0x22,
	CMD_READ_LONG_ONCE	= 0x23,
	CMD_WRITE_LONG	= 0x32,
	CMD_WRITE_LONG_ONCE	= 0x33,

	/* SETFEATURES stuff */
	SETFEATURES_XFER	= 0x03,
	XFER_UDMA_7		= 0x47,
	XFER_UDMA_6		= 0x46,
	XFER_UDMA_5		= 0x45,
	XFER_UDMA_4		= 0x44,
	XFER_UDMA_3		= 0x43,
	XFER_UDMA_2		= 0x42,
	XFER_UDMA_1		= 0x41,
	XFER_UDMA_0		= 0x40,
	XFER_MW_DMA_4		= 0x24,	/* CFA only */
	XFER_MW_DMA_3		= 0x23,	/* CFA only */
	XFER_MW_DMA_2		= 0x22,
	XFER_MW_DMA_1		= 0x21,
	XFER_MW_DMA_0		= 0x20,
	XFER_SW_DMA_2		= 0x12,
	XFER_SW_DMA_1		= 0x11,
	XFER_SW_DMA_0		= 0x10,
	XFER_PIO_6		= 0x0E,	/* CFA only */
	XFER_PIO_5		= 0x0D,	/* CFA only */
	XFER_PIO_4		= 0x0C,
	XFER_PIO_3		= 0x0B,
	XFER_PIO_2		= 0x0A,
	XFER_PIO_1		= 0x09,
	XFER_PIO_0		= 0x08,
	XFER_PIO_SLOW		= 0x00,

	SETFEATURES_WC_ON	= 0x02, /* Enable write cache */
	SETFEATURES_WC_OFF	= 0x82, /* Disable write cache */

	/* Enable/Disable Automatic Acoustic Management */
	SETFEATURES_AAM_ON	= 0x42,
	SETFEATURES_AAM_OFF	= 0xC2,

	SETFEATURES_SPINUP	= 0x07, /* Spin-up drive */

	SETFEATURES_SENABLE = 0x10, /* Enable use of SATA feature */
	SETFEATURES_SDISABLE = 0x90, /* Disable use of SATA feature */

	/* SETFEATURE Sector counts for SATA features */
	SFPDMA_OFFSET	= 0x01,	/* FPDMA non-zero buffer offsets */
	SFPDMA_AA		= 0x02, /* FPDMA Setup FIS Auto-Activate */
	SDIPM		= 0x03,	/* Device Initiated Power Management */
	SFPDMA_IN_ORDER	= 0x04,	/* FPDMA in-order data delivery */
	SAN			= 0x05,	/* Asynchronous Notification */
	SSSP		= 0x06,	/* Software Settings Preservation */

	/* feature values for SET_MAX */
	SET_MAX_ADDR	= 0x00,
	SET_MAX_PASSWD	= 0x01,
	SET_MAX_LOCK	= 0x02,
	SET_MAX_UNLOCK	= 0x03,
	SET_MAX_FREEZE_LOCK	= 0x04,

	/* feature values for DEVICE CONFIGURATION OVERLAY */
	DCO_RESTORE		= 0xC0,
	DCO_FREEZE_LOCK	= 0xC1,
	DCO_IDENTIFY	= 0xC2,
	DCO_SET		= 0xC3,

	/* feature values for SMART */
	SMART_ENABLE	= 0xD8,
	SMART_READ_VALUES	= 0xD0,
	SMART_READ_THRESHOLDS = 0xD1,

	/* feature values for Data Set Management */
	DSM_TRIM		= 0x01,

	/* password used in LBA Mid / LBA High for executing SMART commands */
	SMART_LBAM_PASS	= 0x4F,
	SMART_LBAH_PASS	= 0xC2,

	/* ATAPI stuff */
	ATAPI_PKT_DMA		= (1 << 0),
	ATAPI_DMADIR		= (1 << 2),	/* ATAPI data dir:
						   0=to device, 1=to host */
	ATAPI_CDB_LEN		= 16,

	/* PMP stuff */
	SPMP_MAX_PORTS	= 15,
	SPMP_CTRL_PORT	= 15,

	SPMP_GSCR_DWORDS	= 128,
	SPMP_GSCR_PROD_ID	= 0,
	SPMP_GSCR_REV	= 1,
	SPMP_GSCR_PORT_INFO	= 2,
	SPMP_GSCR_ERROR	= 32,
	SPMP_GSCR_ERROR_EN	= 33,
	SPMP_GSCR_FEAT	= 64,
	SPMP_GSCR_FEAT_EN	= 96,

	SPMP_PSCR_STATUS	= 0,
	SPMP_PSCR_ERROR	= 1,
	SPMP_PSCR_CONTROL	= 2,

	SPMP_FEAT_BIST	= (1 << 0),
	SPMP_FEAT_PMREQ	= (1 << 1),
	SPMP_FEAT_DYNSSC	= (1 << 2),
	SPMP_FEAT_NOTIFY	= (1 << 3),

	/* cable types */
	CBL_NONE		= 0,
	CBL_PATA40		= 1,
	CBL_PATA80		= 2,
	CBL_PATA40_SHORT	= 3,	/* 40 wire cable to high UDMA spec */
	CBL_PUNK	= 4,	/* don't know, maybe 80c? */
	CBL_PIGN	= 5,	/* don't know, ignore cable handling */
	CBL_SATA		= 6,

	/* SATA Status and Control Registers */
	SCR_STATUS		= 0,
	SCR_ERROR		= 1,
	SCR_CONTROL		= 2,
	SCR_ACTIVE		= 3,
	SCR_NOTIFICATION	= 4,


	/* struct taskfile flags */
	TFLAG_LBA48		= (1 << 0), /* enable 48-bit LBA and "HOB" */
	TFLAG_ISADDR	= (1 << 1), /* enable r/w to nsect/lba regs */
	TFLAG_DEVICE	= (1 << 2), /* enable r/w to device reg */
	TFLAG_WRITE		= (1 << 3), /* data dir: host->dev==1 (write) */
	TFLAG_LBA		= (1 << 4), /* enable LBA */
	TFLAG_FUA		= (1 << 5), /* enable FUA */
	TFLAG_POLLING	= (1 << 6), /* set nIEN to 1 and use polling */

	/* protocol flags */
	PROT_FLAG_PIO	= (1 << 0), /* is PIO */
	PROT_FLAG_DMA	= (1 << 1), /* is DMA */
	PROT_FLAG_DATA	= PROT_FLAG_PIO | PROT_FLAG_DMA,
	PROT_FLAG_NCQ	= (1 << 2), /* is NCQ */
	PROT_FLAG_ATAPI	= (1 << 3), /* is ATAPI */
};

/**
 * Thanks for Deepseek
 */
enum class ATAcmd : u8 {
    // ========== 核心读写命令 ==========
    read_pio           = 0x20,   // PIO模式读扇区（慢但兼容性最好）
    read_pio_ext       = 0x24,   // PIO模式读扇区（48位LBA）
    read_dma           = 0x25,   // DMA模式读扇区（主力命令，推荐）
    read_dma_queued    = 0x26,   // 带队列的DMA读（NCQ）
    read_stream        = 0x2B,   // 流式读（音视频专用）
    read_stream_dma    = 0x2A,   // 流式DMA读（音视频专用）
    read_multi         = 0xC4,   // 多扇区PIO读（一次读多个扇区）
    read_multi_ext     = 0x29,   // 多扇区PIO读（48位LBA）
    
    write_pio          = 0x30,   // PIO模式写扇区（慢但兼容性最好）
    write_pio_ext      = 0x34,   // PIO模式写扇区（48位LBA）
    write_dma          = 0x35,   // DMA模式写扇区（主力命令，推荐）
    write_dma_queued   = 0x36,   // 带队列的DMA写（NCQ）
    write_stream       = 0x3B,   // 流式写（音视频专用）
    write_stream_dma   = 0x3A,   // 流式DMA写（音视频专用）
    write_multi        = 0xC5,   // 多扇区PIO写
    write_multi_ext    = 0x39,   // 多扇区PIO写（48位LBA）
    write_multi_fua    = 0xCE,   // 多扇区PIO写（强制单元访问）
    write_uncorrectable= 0x45,   // 标记坏块（写入不可校正的数据）
    write_fua_ext      = 0x3D,   // DMA写（强制单元访问，绕过缓存）
    write_queued_fua   = 0x3E,   // 排队写（强制单元访问）
    
    // ========== 设备识别命令 ==========
    identify_ata       = 0xEC,   // 获取ATA硬盘身份证（型号/容量/特性）
    identify_atapi     = 0xA1,   // 获取ATAPI设备身份证（光驱/磁带机）
    read_native_max    = 0xF8,   // 读取原生最大地址（28位LBA）
    read_native_max_ext= 0x27,   // 读取原生最大地址（48位LBA）
    set_max            = 0xF9,   // 设置最大地址（28位LBA，限制容量）
    set_max_ext        = 0x37,   // 设置最大地址（48位LBA，限制容量）
    verify             = 0x40,   // 校验扇区（不读数据，只检查）
    verify_ext         = 0x42,   // 校验扇区（48位LBA）
    
    // ========== 缓存与电源管理 ==========
    flush_cache        = 0xE7,   // 刷新写缓存（把数据真正写到盘片）
    flush_cache_ext    = 0xEA,   // 刷新写缓存（扩展版，必须用这个）
    set_features       = 0xEF,   // 设置硬盘特性（写缓存/电源管理/传输模式）
    set_multiple       = 0xC6,   // 设置多扇区读写块大小
    standby            = 0xE2,   // 进入待机模式（停转）
    standby_now        = 0xE0,   // 立即进入待机模式（不等待缓存刷新）
    idle               = 0xE3,   // 进入空闲模式（低功耗）
    idle_immediate     = 0xE1,   // 立即进入空闲模式
    sleep              = 0xE6,   // 进入休眠模式（最深睡眠）
    check_power        = 0xE5,   // 检查当前电源模式状态
    
    // ========== 日志与诊断 ==========
    read_log_ext       = 0x2F,   // 读取日志页（SMART日志/错误日志）
    write_log_ext      = 0x3F,   // 写入日志页
    read_log_dma       = 0x47,   // DMA方式读取日志页
    write_log_dma      = 0x57,   // DMA方式写入日志页
    smart              = 0xB0,   // SMART命令（需配合子命令使用）
    execute_diagnostic = 0x90,   // 执行设备自检（EDD）
    device_reset       = 0x08,   // ATAPI设备复位（谨慎使用）
    nop                = 0x00,   // 空操作（用于忙等待）
    download_microcode = 0x92,   // 更新硬盘固件（危险操作）
    
    // ========== 高级/专用命令 ==========
    dsm                = 0x06,   // 数据管理（TRIM命令，用于SSD回收空间）
    fpdma_read         = 0x60,   // FPDMA读取（NCQ，带优先级）
    fpdma_write        = 0x61,   // FPDMA写入（NCQ，带优先级）
    packet             = 0xA0,   // ATAPI包命令（光驱/磁带机的入口）
    service            = 0xA2,   // ATAPI服务命令（配合PACKET使用）
    
    // ========== 安全相关（密码锁） ==========
    security_set_password   = 0xF1,   // 设置硬盘密码
    security_unlock         = 0xF2,   // 解锁硬盘（需要密码）
    security_erase_prepare  = 0xF3,   // 准备安全擦除
    security_erase_unit     = 0xF4,   // 执行安全擦除（数据销毁）
    security_freeze_lock    = 0xF5,   // 冻结安全状态（防暴力破解）
    security_disable_password=0xF6,   // 禁用密码（需验证旧密码）
    
    // ========== 端口复用（PMP） ==========
    pmp_read          = 0xE4,   // 读端口复用器寄存器
    pmp_write         = 0xE8,   // 写端口复用器寄存器
    
    // ========== 配置与信任计算 ==========
    config_overlay    = 0xB1,   // 配置覆盖（设备特定）
    config_stream     = 0x51,   // 配置流（音视频流控制）
    trusted_receive   = 0x5C,   // 可信接收（TCG/Opal安全命令）
    trusted_receive_dma=0x5D,   // 可信接收（DMA方式）
    trusted_send      = 0x5E,   // 可信发送
    trusted_send_dma  = 0x5F,   // 可信发送（DMA方式）
    
    // ========== 媒体控制（可移动设备） ==========
    media_lock        = 0xDE,   // 锁定介质弹出（防止误弹出）
    media_unlock      = 0xDF,   // 解锁介质弹出
    check_media_card_type = 0xD1, // 检查CF卡类型（老式设备专用）
    
    // ========== CFA专用（CompactFlash） ==========
    cfa_request_ext_error = 0x03,   // 请求扩展错误信息
    cfa_write_without_erase = 0x38, // 不擦除直接写（闪存优化）
    cfa_translate_sector = 0x87,    // 扇区地址转换
    cfa_erase          = 0xC0,      // 擦除扇区（闪存专用）
    cfa_write_multi_no_erase = 0xCD,// 多扇区写（不擦除）
    
    // ========== 遗留/废弃命令 ==========
    restore           = 0x10,   // 恢复（ATA-1遗留命令，已废弃）
    initialize_params = 0x91,   // 初始化设备参数（用于老式CHS寻址）
};

#pragma pack(push, 1)
union ATAstatus {
	struct {
		u8 error : 1;
		u8 index : 1;
		u8 correct_data : 1;
		u8 data_ready : 1;
		u8 service : 1;
		u8 driver_fault : 1;
		u8 ready : 1;
		u8 busy : 1;
	};
	u8 buf;
};
#pragma pack(pop)

enum class ATAerror : u32 {
	// ========== 恢复的错误类型 ==========
	SERR_DRECOVERED	= (1 << 0), // data error
	SERR_COMM_RECOVERED	= (1 << 1), // communic error

	// ========== 不可恢复错误 ==========
	SERR_DATA		= (1 << 8), // data error
	SERR_PERSISTENT		= (1 << 9), // communic error
	SERR_PROTOCOL		= (1 << 10), // protocol violation
	SERR_INTERNAL		= (1 << 11), // in-host error

	// ========== PHY与链路层错误 ==========
	SERR_PHYRDY_CHG		= (1 << 16), /* PHY RDY changed */
	SERR_PHY_INT_ERR	= (1 << 17), /* PHY internal error */
	SERR_COMM_WAKE		= (1 << 18), /* Comm wake */
	SERR_10B_8B_ERR		= (1 << 19), /* 10b to 8b decode error */
	SERR_DISPARITY		= (1 << 20), /* Disparity */
	SERR_CRC		= (1 << 21), /* CRC error */
	SERR_HANDSHAKE		= (1 << 22), /* Handshake error */
	SERR_LINK_SEQ_ERR	= (1 << 23), /* Link sequence error */

	// ========== 传输层错误 ==========
	SERR_TRANS_ST_ERROR	= (1 << 24), /* Transport state trans. error */
	SERR_UNRECOG_FIS	= (1 << 25), /* Unrecognized FIS */
	SERR_DEV_XCHG		= (1 << 26), /* device exchanged */
};

enum class CMDBlock : u16 {
	data = 0x00,
	error = 0x01,
	feature = error,
	sector_num = 0x02,
	LBAl = 0x03,
	LBAm = 0x04,
	LBAh = 0x05,
	device = 0x06,
	status = 0x07,
	CMD = status,

	sector_index = LBAl,
	cylinder_l = LBAm,
	cylinder_h = LBAh,
	head = device,
	IRQ = sector_num,
};

inline u16 operator +(u16 u, CMDBlock c) {
	return u + static_cast<u16>(c);
}

