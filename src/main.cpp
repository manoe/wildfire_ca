#include <iostream>
#include "wf.h"

int main() {
    WildFireParams param = { 0.58, 0.045, 0.131, 0.078, 0, 4, 100 };
    WildFireCA ca(20,20,param);
    ca.step();
    std::cout<<ca<<std::endl;
    ca.step();
    std::cout<<ca<<std::endl;
    return 0;
}
