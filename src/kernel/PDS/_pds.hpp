#include "pds.hpp"
#include <interface/dev_cls/device.hpp>
#include <global/macro.hpp>
#include <drv/screen/screen_srv.hpp>

#include "ISA/isa.hpp"
#include "PCI/pci.hpp"
#include "USB/usb.hpp"

using DDR = Device_DriverRecored;

template<typename C>
class DDR::Iterator {
protected:
    using Pit_t = DeviceSpace_t::Iterator;
    using Dit_t = class rtl::list<DDR::DeviceNode>::Iterator;
    bool match(const PifNode &prog) const 
    { return static_cast<const C *>(this)->match(prog); }
private:
    DeviceSpace_t::Iterator pif_this;
public:
    Iterator(const Pit_t &pit_): pif_this{pit_} {}
    bool operator != (const Iterator &oth) const 
    { return (pif_this != oth.pif_this); }
    PifNode &operator * () const { return *pif_this; }
    Iterator &operator ++ () { 
        match(*pif_this); // not yet
        return *this;
    }
};

// === conditions ===

template<u32 N, DevAcCon<PifNode> AC>
class DDR::CondiView : CondiView<N-1, AC> {
    AC ac;
    class it;
public:
    constexpr CondiView(CondiView<N-1, AC> oth, AC ac_):
    CondiView<N-1, AC>{oth}, ac{ac_} {}

    CondiView<N+1, AC> filter(AC ac) { return {*this, ac}; }
    bool match(const PifNode &prog) const
    { return ac(prog) && CondiView<N-1, AC>::match(prog); }

    it begin() const;
    it end() const;
};

template<DevAcCon<PifNode> AC>
class DDR::CondiView<0u, AC> {
protected:
    using PifNode = DDR::PifNode;
    using DevNode = DDR::DevNode;
    DeviceSpace_t &space;

    auto get_begin() const { return space.begin(); }
    auto get_end() const { return space.end(); }
private:
    AC ac;
    class it;
public:
    CondiView(DeviceSpace_t &s, AC ac_): space{s}, ac{ac_} {}

    CondiView<1u, AC> filter(AC ac) { return {*this, ac}; }
    bool match(const PifNode &pid) const { return ac(pid); }

    it begin() const;
    it end() const;
};

// === Iterators ===

class DDR::it : public DDR::Iterator<it> {
protected:
    bool match(const PifNode &, const DevNode &) const { return true; }
public:
    it(const Pit_t &pit_): Iterator<it>{pit_} {}
};

template<u32 N, DevAcCon<PifNode> AC>
class DDR::CondiView<N, AC>::it : public DDR::Iterator<it> {
    CondiView<N, AC> condition;
    friend class DDR::Iterator<it>;
    bool match(const PifNode &pid) const { return condition.match(pid); }
public:
    it(const Iterator<it>::Pit_t &pit_, const CondiView<N, AC> &condition_):
    Iterator<it>{pit_}, condition{condition_} {}
};

template<DevAcCon<PifNode> AC>
class DDR::CondiView<0U, AC>::it : public DDR::Iterator<it> {
    CondiView<0, AC> condition;
    friend class DDR::Iterator<it>;
    bool match(const PifNode &pid) const { return condition.match(pid); }
public:
    it(const Iterator<it>::Pit_t &pit_, const CondiView<0, AC> &condition_):
    Iterator<it>{pit_}, condition{condition_} {}
};

DDR::it DDR::begin() const { return {storage.begin()}; }
DDR::it DDR::end() const { return {storage.end()}; }
template<DevAcCon<PifNode> AC> DDR::CondiView<0U, AC> 
DDR::filter(AC ac) { return {storage, ac}; }
template<u32 N, DevAcCon<PifNode> AC> DDR::CondiView<N, AC>::it 
DDR::CondiView<N, AC>::begin() const { return {CondiView<0u, AC>::space.begin(), *this}; }
template<u32 N, DevAcCon<PifNode> AC> DDR::CondiView<N, AC>::it 
DDR::CondiView<N, AC>::end() const { return {CondiView<0u, AC>::space.end(), *this}; }
template<DevAcCon<PifNode> AC> DDR::CondiView<0u, AC>::it 
DDR::CondiView<0u, AC>::begin() const { return {space.begin(), *this}; }
template<DevAcCon<PifNode> AC> DDR::CondiView<0u, AC>::it 
DDR::CondiView<0u, AC>::end() const { return {space.end(), *this}; }

bool always_true(const PifNode &) { return true; }