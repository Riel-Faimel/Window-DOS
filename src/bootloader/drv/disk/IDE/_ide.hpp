#include "ide.hpp"
#include <lib/hardlib/x86/port.h>
#include <drv/screen/screen_srv.hpp>
#include <TL/idlib>
#include <registry/registry.hpp>

class IDE_Channal {
public:
    enum class Channal{
        Master_Channel = 0x1F0,
        Slave_Channel = 0x170
    };
    IDE_Channal(IDE_DISK& master, IDE_DISK& slave, Channal chan); //use strange

private:
    Channal chan;
    unsigned read(unsigned short *buf, unsigned LBA, unsigned count, DISK_INFO *info);
    unsigned read_PIO_LBA(unsigned short *buf, unsigned LBA, unsigned count, u8 dev);
    unsigned read_PIO_CHS(unsigned short *buf, unsigned LBA, unsigned count, u8 dev);

    unsigned write(unsigned short *buf, unsigned LBA, unsigned char count, DISK_INFO *info);
    unsigned write_PIO_LBA(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);
    unsigned write_PIO_CHS(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);

    struct ctlpkg{
        unsigned long	flags;		/* TFLAG_xxx */
        u8			protocol;	/* PROT_xxx */
        u8			ctl;		/* control reg */
        u8			feature;
        u8			nsect;
		u8			l;
    	u8			m;
    	u8			h;
		struct {
        u8			feature;
        u8			nsect;
		u8			l;
    	u8			m;
    	u8			h;
		} hob;

        u8			device;
        u8			cmd_stat;	/* IO operation */
    };
    void get_ctlpkg(ctlpkg&);
    void send_ctlpkg(ctlpkg &);
    void effect_ctlpkg(ctlpkg &);
    friend class IDE_DISK;
};


/**
 * from Linux2.6.32.1
 * include/linux/ata.h
 */
enum class ATA : u32{
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

	/* SError bits */
	SERR_DRECOVERED	= (1 << 0), /* recovered data error */
	SERR_COMM_RECOVERED	= (1 << 1), /* recovered comm failure */
	SERR_DATA		= (1 << 8), /* unrecovered data error */
	SERR_PERSISTENT		= (1 << 9), /* persistent data/comm error */
	SERR_PROTOCOL		= (1 << 10), /* protocol violation */
	SERR_INTERNAL		= (1 << 11), /* host internal error */
	SERR_PHYRDY_CHG		= (1 << 16), /* PHY RDY changed */
	SERR_PHY_INT_ERR	= (1 << 17), /* PHY internal error */
	SERR_COMM_WAKE		= (1 << 18), /* Comm wake */
	SERR_10B_8B_ERR		= (1 << 19), /* 10b to 8b decode error */
	SERR_DISPARITY		= (1 << 20), /* Disparity */
	SERR_CRC		= (1 << 21), /* CRC error */
	SERR_HANDSHAKE		= (1 << 22), /* Handshake error */
	SERR_LINK_SEQ_ERR	= (1 << 23), /* Link sequence error */
	SERR_TRANS_ST_ERROR	= (1 << 24), /* Transport state trans. error */
	SERR_UNRECOG_FIS	= (1 << 25), /* Unrecognized FIS */
	SERR_DEV_XCHG		= (1 << 26), /* device exchanged */

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
