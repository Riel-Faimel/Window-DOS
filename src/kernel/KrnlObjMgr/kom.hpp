#ifndef __kernel_KOM_kom_hpp
#define __kernel_KOM_kom_hpp

#include <interface/knlobj/knlobj.hpp>
#include <interface/knlobj/converter.hpp>
#include <HL/container>

class KernelObjectManager {
public:
    class ObjID {
        u32 id;
    public:
        explicit ObjID(u32 id_ = 0): id{id_} {}
        bool valid() const { return id != 0; }
        bool operator == (ObjID oth) const { return id == oth.id; }
    };
private:
    struct Entry {
        ObjID id;
        KernelObj *obj;

        Entry(ObjID id_, KernelObj *obj_): id{id_}, obj{obj_} {}

        Entry(const Entry &) = delete;
        Entry &operator = (const Entry &) = delete;

        Entry(Entry &&oth) noexcept: id{oth.id}, obj{oth.obj} {
            oth.obj = nullptr;
        }

        Entry &operator = (Entry &&oth) noexcept {
            if (this != &oth) {
                if (obj) delete obj;
                id = oth.id;
                obj = oth.obj;
                oth.obj = nullptr;
            }
            return *this;
        }

        ~Entry() { if (obj) delete obj; }
    };

    rtl::array<Converter *> conv_l;
    rtl::list<Entry> obj_l;
    u32 next_id = 1;
public:
    KernelObjectManager();
    void append(Converter *);
    ObjID append(KernelObj *);
    bool delet_(ObjID);
};

extern KernelObjectManager *kom;

#endif
