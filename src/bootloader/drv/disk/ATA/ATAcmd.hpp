#include "ATAdef.hpp"
#include <global/type.hpp>
#include <TL/tuple>
#include <TL/utility>

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

class IdentifyAnalysis {
	enum {
		WORDS		= 256,
		CONFIG		= 0,
		CYLS		= 1,
		HEADS		= 3,
		SECTORS		= 6,
		SERNO		= 10,
		BUF_SIZE		= 21,
		FW_REV		= 23,
		PROD		= 27,
		MAX_MULTSECT	= 47,
		DWORD_IO		= 48,
		CAPABILITY	= 49,
		OLD_PIO_MODES	= 51,
		OLD_DMA_MODES	= 52,
		FIELD_VALID	= 53,
		CUR_CYLS		= 54,
		CUR_HEADS	= 55,
		CUR_SECTORS	= 56,
		MULTSECT		= 59,
		LBA_CAPACITY	= 60,
		SWDMA_MODES	= 62,
		MWDMA_MODES	= 63,
		PIO_MODES	= 64,
		EIDE_DMA_MIN	= 65,
		EIDE_DMA_TIME	= 66,
		EIDE_PIO		= 67,
		EIDE_PIO_IORDY	= 68,
		QUEUE_DEPTH	= 75,
		MAJOR_VER	= 80,
		COMMAND_SET_1	= 82,
		COMMAND_SET_2	= 83,
		CFSSE		= 84,
		CFS_ENABLE_1	= 85,
		CFS_ENABLE_2	= 86,
		CSF_DEFAULT	= 87,
		UDMA_MODES	= 88,
		HW_CONFIG	= 93,
		SPG		= 98,
		LBA_CAPACITY_2	= 100,
		DLST_LUN		= 126,
		DLF		= 128,
		CSFO		= 129,
		CFA_POWER	= 160,
		CFA_KEY_MGMT	= 162,
		CFA_MODES	= 163,
		DSET_MGMT	= 169,
		ROT_SPEED	= 217,
		PIO4		= (1 << 1),
		SERNO_LEN	= 20,
		FW_REV_LEN	= 8,
		PROD_LEN		= 40,
	};
	u16 buf[256];
public:
	bool is_ata() { return ((buf[CONFIG] & (1 << 15))) == 0; }

	rtl::tuple<bool, u16, u16, u16> chs_param() {
		auto cur = (buf[FIELD_VALID] & 1) != 0;
		if (cur) {
			auto c = buf[CUR_CYLS], h = buf[CUR_HEADS], s = buf[CUR_SECTORS];
			if (c == 0 || h == 0 || s == 0) {
				return {
					false, 0, 0, 0
				};
			}
			return {
				true, c, h, s
			};
		} else {
			return {
				true, buf[CYLS], buf[HEADS], buf[SECTORS]
			};
		}
	}
	rtl::pair<bool, u32> lba_param() {
		if (!((buf[CAPABILITY] >> 9) & 1)) return { false, 0 };
		u32 size = as_u32(LBA_CAPACITY) & 0x0FFFFFFF;
		return { true, size };
	}
	rtl::pair<bool, u64> lba48_param() {
		if ((buf[COMMAND_SET_2] & 0xC000) != 0x4000) return { false, u64(0) };
		u64 size = as_u64(LBA_CAPACITY_2);
		if (!size) return { false, u64(0) };
		if (!(buf[COMMAND_SET_2] & (1 << 10))) return { false, u64(0) };
		return { true, size };
	}

	u32 lba28_sec_count() { return as_u32(LBA_CAPACITY) & 0x0FFFFFFF; }
	u64 lba48_sec_count() { return as_u64(LBA_CAPACITY_2); }

	bool has_dma(){ return (buf[CAPABILITY] & (1 << 8)) != 0; }
	bool has_ncq(){ return (buf[76] & (1 << 8)) != 0; }
	u32 as_u32(size_t n) { return ((u32)buf[n + 1] << 16) | ((u32)buf[n]); }
	u64 as_u64(size_t n) { return u64{as_u32(n + 2), as_u32(n)}; }
	bool has_fua() 
	{ if ((buf[CFSSE] & 0xC000) != 0x4000) return false;
	return (buf[CFSSE] & (1 << 6)) != 0; }
	bool has_flush(){
		if ((buf[COMMAND_SET_2] & 0xC000) != 0x4000)return false;
		return (buf[COMMAND_SET_2] & (1 << 12)) != 0;
	}
	bool pio_valid() {
		return (buf[PIO_MODES] & 0x8000) != 0;
	}

	u16 &operator [] (size_t id) { return buf[id]; }
	operator bufAddr () { return bufAddr{(void *)buf}; }
};
