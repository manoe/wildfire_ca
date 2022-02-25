#include "wf.h"

int main() {
    WildFireParams param = { 0.58, 0.045, 0.131, 0.078, 0, 4, 100 };
    WildFireCA ca(1000,1000,param);
    ca.step();
    return 0;
}
