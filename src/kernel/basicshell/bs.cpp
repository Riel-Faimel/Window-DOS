#include "bs.hpp"
#include "shell_in.hpp"
#include "shell_vm.hpp"
#include <stdio>

[[noreturn]]
void BasicShell::main() {
    kout << " - renew DOS Basic Shell -\n";

    ShellIn io;
    if (!io.ready()) kout << "[shell] no input stream, \"kbd\" is not registered in SDM\n";

    ShellVM vm{io};

    for (;;) {
        kout << "dbg> ";
        ShellNode node;
        if (io.read(node)) vm.run(node);
    }
}
