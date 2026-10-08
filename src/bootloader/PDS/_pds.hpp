#include "pds.hpp"
#include <interface/dev_cls/device.hpp>

#include "ISA/isa.hpp"
#include "PCI/pci.hpp"
#include "USB/usb.hpp"

using AccordCondition = bool (*) (const ProgramInFace &, const DeviceNode &);
using PIterator_t = DeviceSpace_t::Iterator;
using DIterator_t = rtl::list<DeviceNode>::Iterator;

template<u32 N>
class condi_it {
    PIterator_t this_prog;
    PIterator_t end_prog;

    DIterator_t this_dev;
    DIterator_t end_dev;

    const condi_tranv<N> &condition;

    void prev() {
        for (;this_prog != end_prog;++this_prog,
            this_dev = (*this_prog).dev.begin(),
            end_dev = (*this_prog).dev.end()
        ) {
            for (;this_dev != end_dev;++this_dev) {
                if (condition.match(*this_prog, *this_dev)) return; // point at match device
            }
        }
        // point at end
    }
public:
    condi_it(PIterator_t begin, PIterator_t end, const condi_tranv<N> &con):
    this_prog{begin}, end_prog{end}, this_dev{(*this_prog).dev.begin()},
    end_dev{(*this_prog).dev.end()}, condition{con} { prev(); }

    bool operator !=(const condi_it &) { return (this_prog != end_prog) && (this_dev != end_dev); }
    DeviceNode &operator *() { return *this_dev; }
    condi_it &operator ++() {
        ++this_prog;
        prev();
        return *this;
    }
};

template<u32 N>
class condi_tranv : condi_tranv<N-1> {
    AccordCondition ac;
public:
    constexpr condi_tranv(condi_tranv<N-1> oth, const AccordCondition &ac_):
    condi_tranv<N-1>{oth}, ac{ac_} {}

    condi_tranv<N+1> filter(const AccordCondition &ac) { return {*this, ac}; }
    bool match(const ProgramInFace &prog, const DeviceNode &dev) const
    { return ac(prog, dev) && condi_tranv<N-1>::match(prog, dev); }

    condi_it<N> begin() { return {this->get_begin(), this->get_end(), *this}; }
    condi_it<N> end() { return {this->get_end(), this->get_end(), *this}; }
};

template<>
class condi_tranv<0u> {
    DeviceSpace_t &space;
public:
    condi_tranv(DeviceSpace_t &s): space{s} {}

    condi_tranv<1u> filter(const AccordCondition &ac) { return {*this, ac}; }
    bool match(const ProgramInFace &, const DeviceNode &) const { return true; }

    auto get_begin() { return space.begin(); }
    auto get_end() { return space.end(); }
};

static bool always_true(const ProgramInFace &, const DeviceNode &) { return true; }

//#include <TL/condi_tranv>