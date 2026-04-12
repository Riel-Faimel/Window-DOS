extern "C" {
    void __attribute__((regparm(1)))change_stack(void *);
    void change_stack_back();
}