# utimer

Lightweight, simple, straightforward C timer library. Designed with
microcontrollers in mind - no dynamic allocation, static only.
Works with limited timing resolution

# Include in your code

Just copy `utimer.c` and `utimer.h` file to your source tree, and add to your build system.
Alternatively, using `cmake`, clone this repo as your project submodule, and:

```
# in your CMakeLists.txt
add_subdirectory(path_to_this_repo)
target_link_libraries(YOUR_TARGET PUBLIC utimer)
```

# Configure it

Only configurable thing is number of statically allocated timer objects - macro `UTIMER_MAX_TIMERS`.
By default it is set to 20. You can set it just before include library's header file.

# Using it in your code

Start with calling `utimer_init` function to setup how library fetches current time:

```C
#define UTIMER_MAX_TIMERS 30
#include "utimer/utimer.h"

int some_function_returning_timestamp() {
    return timestamp;
}

// main.c
utimer_init(some_function_returning_timestamp);
```

To actually feed the library with CPU resource, call `utimer_process();`. Library assumes milisecond
resolution, so there is no point calling it quicker than with 1 [ms] period. But still, simplest
way is to just put it in main loop (`while(1) {...}`) and call it a day.

You can also call it from 1 [ms] timer interrupt service routine, but bear in mind that all
timers callbacks will also be called from this ISR context.

To setup a timer:

```C
int timer_instance = utimer_create_timer(10, &clbk, true); // 10ms periodical timer
if(timer_instance == 0){
    // timer created successfully
}
```

`clbk` is a `struct utimer_callback` object, as below:

```C
void _clbk_fcn(const struct utimer_callback * const handler) {
    printf("timer callback\n");
}

struct utimer_callback clbk = {
    .timeout = _clbk_fcn
};
```

When everything is set up:

```C
int retval = utimer_start(timer_instance);
```

From now on every 10 [ms] a `_clbk_fcn` should be called.

For additional functionality - see `./tests/test.cpp`.

# Similar libraries

[stimer](https://github.com/odurc/stimer/) - more feature rich, and better documented