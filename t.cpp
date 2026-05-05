using size_t = unsigned long long;

#define __independent_lib_Using_pointer_system
#include "src/include/TL/pointer"

#define __independent_lib_Using_template_container
#include "src/include/TL/container"

#include <iostream>

//#include "t.hpp"

void p(unsigned a) {
    std::cout << "T" << a;
}

rtl::dummy_ptr<int> sp;
int main() {
    int a = 1;
    rtl::vested_ptr<int, rtl::no_delete> p_{a};

    rtl::amorphous_ptr<void> p_faddr {&p};
    //p_faddr(&a, 3, std::cout, std::add_cv_t<int>{}, 1, 2, 3, 4, 9);

    new(sp) rtl::aeonian_ptr<int> {a};
    reinterpret_cast<rtl::aeonian_ptr<int>&>(sp);

    auto t = make_tuple(1, "23", 'c', p_faddr);
    rtl::tuple_type<2, decltype(t)>::type bb{};
    std::cout << rtl::tuple_size<decltype(t)>::value;

    auto re = get<2>(t);
    std::cout << re;
    return 0;
}