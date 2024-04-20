// SPDX-License-Identifier: MIT
// SPDX-SnippetCopyrightText: 2024 Mateusz Karabela <mateusz.karabela@gmail.com>

#ifndef UTIMER_H
#define UTIMER_H

#include <stddef.h>
#include <stdbool.h>

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef UTIMER_MAX_TIMERS
#define UTIMER_MAX_TIMERS   (20)
#endif

#define UTIMER_OK               (0)
#define UTIMER_NO_RESOURCES     (-1)
#define UTIMER_INSTANCE_ERROR   (-2)
#define UTIMER_STATE_ERROR      (-3)

typedef int utimer_handler;
typedef int (*get_timestamp_fcn)();

struct utimer_callback {
    void (*timeout)(const struct utimer_callback * const handler);
};

void utimer_init(get_timestamp_fcn get_timestamp);
void utimer_process();

utimer_handler utimer_create_timer(int timeout_ms, const struct utimer_callback * const clbk, bool periodical);
int utimer_start(utimer_handler instance);
int utimer_set_timeout_ms(utimer_handler instance, int timeout_ms);
int utimer_cancel(utimer_handler instance);
int utimer_pause(utimer_handler instance);
int utimer_free(utimer_handler instance);


#endif // UTIMER_H
