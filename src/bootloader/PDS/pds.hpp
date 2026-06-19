#ifndef __bootloader_PDS_pds_hpp
#define __bootloader_PDS_pds_hpp

#include <interface/LDisk.hpp>
#include <TL/container>

class PDS {
public:
    struct Info {
        unsigned long long device_id;
        unsigned long long type_id;
    };
    struct Storage {
        Info info;
        using drive = LogicalDisk;
        drive* d;
    };
    struct Network {
        Info info;
        class drive {
            virtual unsigned recv() = 0;
            virtual unsigned send() = 0;
        }* d;
    };
    struct Bridge {
        Info info;
        class drive {
            virtual void probe() = 0;
        }* d;
    };
private:
    struct {
        rtl::array<Storage> s{};
        rtl::array<Network> n{};
        rtl::array<Bridge> b{};        
    } device_space;
public:
    PDS() = default;

    void append(Info , Storage::drive* );
    void append(Info , Network::drive* );
    void append(Info , Bridge::drive* );
    void remove(unsigned long long);

    inline rtl::array<Storage>& get_stg_list() { return device_space.s; }
    inline rtl::array<Network>& get_net_list() { return device_space.n; }
    inline rtl::array<Bridge>& get_brg_list() { return device_space.b; }
};

#endif