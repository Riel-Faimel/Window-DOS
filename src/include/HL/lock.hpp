#ifndef __include_interface_TD_lock_hpp
#define __include_interface_TD_lock_hpp

template<typename T>
struct atomic;

template<>
struct atomic<bool>{
    bool data;
    
        atomic() noexcept = default;
        constexpr atomic(bool desired) noexcept;
        atomic(const atomic&) = delete;
        atomic& operator=(const atomic&) = delete;
        atomic& operator=(const atomic&) volatile = delete;

        constexpr atomic& operator=(bool desired) noexcept { store(desired); return *this; }
        constexpr atomic& operator=(bool desired) volatile noexcept { store(desired); return *this; }

        constexpr bool load(memory_order order = memory_order_seq_cst) const noexcept;
        constexpr bool load(memory_order order = memory_order_seq_cst) const volatile noexcept;
        
        constexpr void store(bool desired, memory_order order = memory_order_seq_cst) noexcept;
        constexpr void store(bool desired, memory_order order = memory_order_seq_cst) volatile noexcept;

        constexpr bool exchange(bool desired, memory_order order = memory_order_seq_cst) noexcept;
        constexpr bool exchange(bool desired, memory_order order = memory_order_seq_cst) volatile noexcept;

};

struct bus_lock {
    bus_lock() {}
    ~bus_lock() {}
};


#endif