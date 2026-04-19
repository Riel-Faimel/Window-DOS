class example {
public:

    example(unsigned, void *){
        ;
    }

    void print(char ch){
        asm volatile (
            "call print_call"
            :
            : "r"(ch)
            : "edx"
        );
    }
};
example ex{0, 0};

void print(char *str){
    for (unsigned i = 0;str[i];i++){
        ex.print(str[i]);
    }
}

int main(){
    print("hello from example application\n");
    return 0;
}