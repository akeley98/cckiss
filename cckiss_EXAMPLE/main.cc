#include <iostream>
#include "hello.h"
#include "gadget.hpp"

int main()
{
    std::cout << get_hello() << '\n';
    Gadget g { 3 , "Robot" };
    std::cout << g << '\n';
    return 0;
}
