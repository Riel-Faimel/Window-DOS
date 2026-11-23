#include "_pds.hpp"

using namespace pds;

PeripheralDeviceSpace *pdevspace;

PeripheralDeviceSpace::PeripheralDeviceSpace():
busspace{}, storage{}, bc{} { pdevspace = this; }

// alloc BusIndex、append into busspace，then probe.
Enumerator::EnumerIndex PeripheralDeviceSpace::append(Enumerator *enumer) {
    auto id = bc++;
    enumer->set_id(id);
    busspace.append(enumer);
    enumer->probe();
    return id;
}

// translate DeviceInfo to {pid, did, info}
Result PeripheralDeviceSpace::append(DeviceInfo *dev) {
    if (!dev) return Result::BadArgs;
    return append(dev->progif, dev->bus, dev->device, dev);
}

/**
 * 设备到达：挂到 (progif, device) 的叉上，成为该叉上的一个实例。
 * 如果驱动已经在等它，立刻绑定；否则挂在叉上等驱动注册。
 */
Result PeripheralDeviceSpace::append(ProgIFIndex pid, Enumerator::EnumerIndex eid, DeviceInfo::DeviceType did, DeviceInfo *info) {
    ProgramInFace *root = nullptr;
    for (auto &pif : storage) if (pif.prog_t == pid) {
        root = &pif;
        goto found_pid;
    }
    storage.append(ProgramInFace{eid, DeviceInfo::ClassType::nul, pid, {}, nullptr});
    root = &storage.back();

    found_pid:
    if (root->bus_id == Enumerator::EnumerIndex{}) root->bus_id = eid;
    DeviceNode *node = nullptr;
    for (auto &dev : root->dev_l) if (dev.dev_t == did) {
        node = &dev;
        goto found_did;
    }
    root->dev_l.append(DeviceNode{did, {}, nullptr});
    node = &root->dev_l.back();
    
    found_did:
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
        // progif=0 的设备还没有对应的接口分类，记下来只是噪声；
        // VGA 一屏内优先保留能绑定/等待驱动的设备。
        if ((u32)pid) {
            kout << "[PDS] +device "
                 << "progif=" << (u32)pid
                 << " bus=" << (u16)root->bus_id
                 << (instance.drvInstance ? " bound\n" : " waiting\n");
        }
    )
    return Result::NoError;
}

// append driver and use it
Result PeripheralDeviceSpace::append(ProgIFIndex pid, DriverConstr dcon) {
    if (!dcon) return Result::BadArgs;

    ProgramInFace *root = nullptr;
    for (auto &pif : storage) if (pif.prog_t == pid) {
        root = &pif;
        goto found_pid;
    }
    storage.append(ProgramInFace{{}, DeviceInfo::ClassType::nul, pid, {}, nullptr});
    root = &storage.back();

    found_pid:
    if (root->drv) return Result::NoSpace;
    root->drv = dcon;

    // use driver
    for (auto &node : root->dev_l) {
        if (node.drv) continue; // skip
        for (auto &instance : node.in_l) {
            if (!instance.drvInstance) {
                instance.drvInstance = dcon(instance.info);
                instance.drvInstance->work();
            }
        }
    }

    LocalDebugInfo(kout << "[PDS] +driver progif=" << (u32)pid << '\n';)
    return Result::NoError;
}

//
Result PeripheralDeviceSpace::append(
    ProgIFIndex pid, DeviceInfo::DeviceType did, DriverConstr dcon
) {
    if (!dcon) return Result::BadArgs;

    ProgramInFace *root = nullptr;
    for (auto &pif : storage) if (pif.prog_t == pid) {
        root = &pif;
        goto found_pid;
    }
    storage.append(ProgramInFace{{}, DeviceInfo::ClassType::nul, pid, {}, nullptr});
    root = &storage.back();

    found_pid:
    DeviceNode *node = nullptr;
    for (auto &dev : root->dev_l) if (dev.dev_t == did) {
        node = &dev;
        goto found_did;
    }
    root->dev_l.append(DeviceNode{did, {}, nullptr});
    node = &root->dev_l.back();

    found_did:
    if (node->drv) return Result::NoSpace;
    node->drv = dcon;

    for (auto &instance : node->in_l) {
        if (!instance.drvInstance) {
            instance.drvInstance = dcon(instance.info);
            instance.drvInstance->work();
        } else {
            instance.drvInstance->off();
            delete instance.drvInstance;
            instance.drvInstance = dcon(instance.info);
            instance.drvInstance->work();
        }
    }

    LocalDebugInfo(kout << "[PDS] +driver progif=" << (u32)pid << "device=" << (u32)did << '\n';)
    return Result::NoError;
}