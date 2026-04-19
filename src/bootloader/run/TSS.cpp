#include "_TSS.hpp"

TSS::TSS(GDT &gdt):
tss{}{
    gdt.create_tss(&tss, );
}
