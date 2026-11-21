#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "_pds.hpp"

/**
 * PeripheralDeviceSpace:
 *
 *   ProgramInFace
 *   ├── drv
 *   └── dev_l   DeviceNode
 *               ├── drv
 *               └── in_l   Instance
 *                           ├── config
 *                           └── drvInstance
 */


namespace {

enum : int {
    PdsOk = 0,
    PdsBadArg = -1,
    PdsConflict = -2, // 一个根只允许一个通用驱动、一个叉只允许一个特化驱动
};

} // namespace

PeripheralDeviceSpace *pdevspace;

PeripheralDeviceSpace::PeripheralDeviceSpace():
busspace{}, storage{}, bc{} {
    pdevspace = this;
}

/**
 * 登记一条总线：分配 BusIndex、登记，然后立刻 probe。
 * probe 期间提交的 DeviceInfo 会带上这条总线的号。
 */
Enumerator::EnumerIndex PeripheralDeviceSpace::append(Enumerator *enumer) {
    auto id = bc++;
    enumer->set_id(id);
    busspace.append(EnumerNode{id, enumer});
    enumer->probe();
    return id;
}

/**
 * 登记一台设备（实例形式）。
 * PDS 接管 dev 的所有权：搬出需要的字段后 delete。
 */
int PeripheralDeviceSpace::append(DeviceInfo *dev) {
    if (!dev) return PdsBadArg;

    // 根不存在时，用这台设备的属性建根（bus/cls 只是记录，不参与匹配）
    ProgramInFace *root = nullptr;
    for (auto &pif : rtl::Filter<ProgramInFace, rtl::list>{storage}
        .with([dev](const ProgramInFace &p) { return p.prog_t == dev->progif; })
    ) {
        root = &pif;
        break;
    }
    if (!root) {
        storage.append(ProgramInFace{dev->bus, dev->cls, dev->progif, {}, nullptr});
    }

    int re = append(dev->progif, dev->device, dev);

    // dev 交给 Instance 持有：驱动后到的补绑路径还要读它，所以不能在这里释放。
    return re;
}

/**
 * 设备到达：挂到 (progif, device) 的叉上，成为该叉上的一个实例。
 * 如果驱动已经在等它，立刻绑定；否则挂在叉上等驱动注册。
 */
int PeripheralDeviceSpace::append(ProgIFIndex pid, DeviceInfo::DeviceType did, DeviceInfo *info) {
    ProgramInFace *root = nullptr;
    for (auto &pif : rtl::Filter<ProgramInFace, rtl::list>{storage}
        .with([pid](const ProgramInFace &p) { return p.prog_t == pid; })
    ) {
        root = &pif;
        break;
    }
    if (!root) {
        storage.append(ProgramInFace{{}, DeviceInfo::ClassType::nul, pid, {}, nullptr});
        root = &storage.back();
    }

    DeviceNode *node = nullptr;
    for (auto &dev : root->dev_l) if (dev.dev_t == did) {
        node = &dev;
        break;
    }
    if (!node) {
        root->dev_l.append(DeviceNode{did, {}, nullptr});
        node = &root->dev_l.back();
    }

    auto &instance = node->in_l.emplace_back(info);

    // 特化优先于通用；都没有就先挂着，等驱动来绑
    if (node->drv) {
        instance.drvInstance = node->drv(info); // 特化
        instance.drvInstance->work();
    } else if (root->drv) {
        instance.drvInstance = root->drv(info); // 通用
        instance.drvInstance->work();
    }

    LocalDebugInfo(
        kout << "[PDS] +device ";
        if ((u32)pid)
        kout << "progif=" << (u32)pid
             << " bus=" << (u16)root->bus_id
             << (instance.drvInstance ? " bound\n" : " waiting\n");
        else
        kout << "unknow\n";
    )
    return PdsOk;
}

/**
 * 通用驱动到达：挂在根上，负责这个 ProgIF 的所有设备类型。
 * 已经挂着的设备立刻绑定；之后到达的设备在 append(pid, did, cfg) 里绑。
 */
int PeripheralDeviceSpace::append(ProgIFIndex pid, DriverConstr dcon) {
    if (!dcon) return PdsBadArg;

    ProgramInFace *root = nullptr;
    for (auto &pif : rtl::Filter<ProgramInFace, rtl::list>{storage}.
        with([pid](const ProgramInFace &p) { return p.prog_t == pid; })
    ) {
        root = &pif;
        break;
    }
    if (!root) {
        storage.append(ProgramInFace{{}, DeviceInfo::ClassType::nul, pid, {}, nullptr});
        root = &storage.back();
    }

    if (root->drv) return PdsConflict;
    root->drv = dcon;

    // 已经挂着、还没绑的设备现在绑上；有特化驱动的叉不抢
    for (auto &node : root->dev_l) {
        if (node.drv) continue;
        for (auto &instance : node.in_l) {
            if (!instance.drvInstance) {
                instance.drvInstance = dcon(instance.info);
                instance.drvInstance->work();
            }
        }
    }

    LocalDebugInfo(kout << "[PDS] +driver progif=" << (u32)pid << '\n';)
    return PdsOk;
}

/**
 * 特化驱动到达：挂在 (progif, device) 的叉上，只负责这一种设备类型。
 * 已经绑在通用驱动上的实例不重绑（没有释放路径），只补绑还没绑的实例。
 */
int PeripheralDeviceSpace::append(
    ProgIFIndex pid, DeviceInfo::DeviceType did, DriverConstr dcon
) {
    if (!dcon) return PdsBadArg;

    ProgramInFace *root = nullptr;
    for (auto &pif : rtl::Filter<ProgramInFace, rtl::list>{storage}.
        with([pid](const ProgramInFace &p) { return p.prog_t == pid; })
    ) {
        root = &pif;
        break;
    }
    if (!root) {
        storage.append(ProgramInFace{{}, DeviceInfo::ClassType::nul, pid, {}, nullptr});
        root = &storage.back();
    }

    DeviceNode *node = nullptr;
    for (auto &dev : root->dev_l) if (dev.dev_t == did) {
        node = &dev;
        break;
    }
    if (!node) {
        root->dev_l.append(DeviceNode{did, {}, nullptr});
        node = &root->dev_l.back();
    }

    if (node->drv) return PdsConflict;
    node->drv = dcon;

    for (auto &instance : node->in_l) {
        if (!instance.drvInstance) {
            instance.drvInstance = dcon(instance.info);
            instance.drvInstance->work();
        }
    }

    LocalDebugInfo(kout << "[PDS] +driver progif=" << (u32)pid << "device=" << (u32)did << '\n';)
    return PdsOk;
}
