#include <xc.h>
#include "HAL.h"

int main(void) {
    halInit();

    for (;;) {
        halTask();
    }
    return 0;
}