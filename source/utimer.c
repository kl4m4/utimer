// SPDX-License-Identifier: MIT
// SPDX-SnippetCopyrightText: 2024 Mateusz Karabela <mateusz.karabela@gmail.com>

#include "utimer/utimer.h"

#define UTIMER_INSTANCE_CHECK(i) \
    if(i >= UTIMER_MAX_TIMERS) return UTIMER_INSTANCE_ERROR;

enum utimer_slot_state {
    UTIMER_STATE_INACTIVE = 0,
    UTIMER_STATE_CONFIGURED,
    UTIMER_STATE_RUNNING,
    UTIMER_STATE_DONE
};

struct utimer_slot {
    int pending_timestamp;
    int interval_ms;
    enum utimer_slot_state state;
    struct utimer_callback * callback;
    bool is_periodical;
};

struct utimer_context {
    struct utimer_slot timers[UTIMER_MAX_TIMERS];
    get_timestamp_fcn get_timestamp;
};

static struct utimer_context h;

void utimer_init(get_timestamp_fcn get_timestamp) {
    for(size_t idx = 0; idx < UTIMER_MAX_TIMERS; idx++) {
        struct utimer_slot *timer = &h.timers[idx];
        timer->state = UTIMER_STATE_INACTIVE;
        timer->pending_timestamp = 0;
        timer->interval_ms = 0;
        timer->callback = NULL;
        timer->is_periodical = false;
    }
    h.get_timestamp = get_timestamp;
}

void utimer_process()
{
    int current_timestamp = h.get_timestamp();
    for(size_t idx = 0; idx < UTIMER_MAX_TIMERS; idx++) {
        struct utimer_slot *timer = &h.timers[idx];
        if(timer->state != UTIMER_STATE_RUNNING) continue;
        if(current_timestamp > timer->pending_timestamp) {
            if(timer->callback) {timer->callback->timeout(timer->callback);}
            if(timer->is_periodical) {
                timer->pending_timestamp += timer->interval_ms;
            }else{
                timer->state = UTIMER_STATE_DONE;
            }
        }

    }
}

utimer_handler utimer_create_timer(int timeout_ms, const struct utimer_callback *const clbk, bool periodical) {
    // find first UTIMER_STATE_INACTIVE
    int found_idx = -1;
    for(size_t idx = 0; idx < UTIMER_MAX_TIMERS; idx++) {
        if(h.timers[idx].state == UTIMER_STATE_INACTIVE) {
            found_idx = idx;
            break;
        }
    }

    if(found_idx < 0) return UTIMER_NO_RESOURCES;

    h.timers[found_idx].callback = (struct utimer_callback*)clbk;
    h.timers[found_idx].interval_ms = timeout_ms;
    h.timers[found_idx].state = UTIMER_STATE_CONFIGURED;
    h.timers[found_idx].is_periodical = periodical;

    return found_idx;
}

int utimer_start(utimer_handler instance)
{
    UTIMER_INSTANCE_CHECK(instance);
    struct utimer_slot *timer = &h.timers[instance];
    if(timer->state == UTIMER_STATE_INACTIVE) return UTIMER_INSTANCE_ERROR;
    if(timer->state != UTIMER_STATE_CONFIGURED) return UTIMER_STATE_ERROR;
    timer->pending_timestamp = h.get_timestamp() + timer->interval_ms;
    timer->state = UTIMER_STATE_RUNNING;
    return UTIMER_OK;
}

int utimer_cancel(utimer_handler instance)
{
    UTIMER_INSTANCE_CHECK(instance);
    struct utimer_slot *timer = &h.timers[instance];
    if(timer->state != UTIMER_STATE_RUNNING) return UTIMER_STATE_ERROR;
    timer->state = UTIMER_STATE_CONFIGURED;
    return UTIMER_OK;
}

int utimer_pause(utimer_handler instance) {
    UTIMER_INSTANCE_CHECK(instance);
    struct utimer_slot *timer = &h.timers[instance];
    if(timer->state == UTIMER_STATE_DONE) return UTIMER_STATE_ERROR;
    if(timer->state == UTIMER_STATE_CONFIGURED) return UTIMER_OK;
    /*
    TBD
    */
    return UTIMER_OK;
}

int utimer_free(utimer_handler instance)
{
    UTIMER_INSTANCE_CHECK(instance);
    struct utimer_slot *timer = &h.timers[instance];
    if(timer->state == UTIMER_STATE_INACTIVE) return UTIMER_INSTANCE_ERROR;
    timer->state = UTIMER_STATE_INACTIVE;
    timer->pending_timestamp = 0;
    timer->callback = NULL;
    return UTIMER_OK;
}
