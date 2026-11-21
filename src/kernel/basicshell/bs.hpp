#ifndef __kernel_basicshell_bs_hpp
#define __kernel_basicshell_bs_hpp

/**
 * 调试环境的入口：只负责把前端和执行器接起来。
 *   ShellIn   —— 从 SDM 打开输入流、行编辑、语法树
 *   ShellVM   —— 执行（以后是字节码解释）
 */
class BasicShell {
public:
    [[noreturn]] void main();
};

#endif
