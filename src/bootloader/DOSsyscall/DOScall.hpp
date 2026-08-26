class IDT;

class DOScall {
    IDT &idt;
public:
    DOScall(IDT&);
    ~DOScall() = default;
};