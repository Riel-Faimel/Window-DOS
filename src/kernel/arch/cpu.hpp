#ifndef __kernel_arch_cpu_hpp
#define __kernel_arch_cpu_hpp

#include <global/type.hpp>
#include <HL/atomic>
#include <HL/generator>

namespace arch {

struct CPUInfo {
    enum class ArchType : u8 {
        x86, x86_64,
    } arch_type;
    u8 cpu_nums;
    u8 bit_num;
    u8 Segmentation_support : 1;
    u8 paging_support : 1;
    u8 Segmentation_mode : 1; // non-flat model
    u8 paging_mode : 1;
    u8 FPU_suppport : 1;
};

class CPUIndex {
    u16 id;
public:
    CPUIndex(): id{0} {}
    explicit CPUIndex(u16 id_): id{id_} {}
    explicit operator u16 () const { return id; }

    bool operator == (const CPUIndex &oth) const { return id == oth.id; }
    CPUIndex &operator++ () { id++; return *this; }
};

class CPU {
    CPUIndex cpuid{0};
    CPU *next_{nullptr};
public:
    CPU();
    CPU(const CPU &) = delete;
    CPU &operator= (const CPU &) = delete;
    CPU *&next_node(){ return next_; }
    CPUIndex get_id(){ return cpuid; }

    virtual CPUInfo get_info() = 0;

    // scheduler control
    virtual void preempt() = 0;
    virtual void collabora() = 0;
    virtual void run(
        void (*)(size_t, void *), size_t, void *, size_t time = 1, u8 ring = 3,
        size_t cs = 0, size_t ds = 0, size_t gs = 0, size_t fs = 0
    ) = 0;

    // interrupt regist
    enum class AccessControl : u8 {
        Public, Private, HardOnly
    };
    virtual bool regist_int_handler(
        void (*handler)(void), unsigned internum, AccessControl access
    ) = 0;
};
    
} // namespace arch 

class CPUList {
    atomic<bool> countorlock{false};
    arch::CPUIndex cpuidcountor{0};
    arch::CPU *root{nullptr};
public:
    void append(arch::CPU *cpu, arch::CPUIndex &cpuid) {
        while(!countorlock.try_lock()){} {
            if (root == nullptr) {
                root = cpu;
                cpuid = cpuidcountor;
            } else for (auto cpup = root;cpup != nullptr;cpup = cpup->next_node()) {
                if (cpup->next_node() == nullptr) {
                    cpup->next_node() = cpu;
                    cpuid = cpuidcountor;
                    break;
                }
            }
            ++cpuidcountor;
        }
        countorlock.unlock();
    }

    auto begin() const { return rtl::list_tranveser{root}.begin(); }
    auto end() const { return rtl::list_tranveser{root}.end(); }
};

extern CPUList cpu_list;

inline arch::CPU::CPU() { cpu_list.append(this, cpuid); }

#ifdef ArchitectureDetail
#ifdef _x86
#include <arch/x86/cpu/cpu.hpp>
#endif
#endif

#endif