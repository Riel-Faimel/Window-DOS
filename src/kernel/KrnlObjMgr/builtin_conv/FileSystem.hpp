#ifndef __kernel_KOM_builtin_conv_FileSystem_hpp
#define __kernel_KOM_builtin_conv_FileSystem_hpp

#include <interface/knlobj/converter.hpp>

namespace builtin_converter {

class FileSystem : public Converter {
public:
    MatchDegree touch(KernelObj *kobj) override;
    void conv(KernelObj *kobj) override;
};

} // namespace builtin_converter

#endif
