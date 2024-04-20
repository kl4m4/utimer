#include <stdio.h>
#include "utimer/utimer.h"

int timestamp = 0;

int fake_get_timestamp() {
    return timestamp;
}

void _clbk(const struct utimer_callback * const handler) {
    printf("timer callback\n");
}

struct utimer_callback clbk = {
    .timeout = _clbk
};

int main(){

    utimer_init(fake_get_timestamp);

    int uut = utimer_create_timer(10, &clbk, true);

    utimer_process();
    timestamp++;

    utimer_start(uut);
    utimer_process();
    timestamp += 9;
    utimer_process();
    timestamp++;
    utimer_process();
    timestamp++;
    utimer_process();   // !
    utimer_process();
    timestamp += 10;
    utimer_process();   // !
    timestamp += 10;
    utimer_process();// !

    utimer_cancel(uut);
    timestamp += 10;
    utimer_process();

    utimer_free(uut);

    return 0;
}