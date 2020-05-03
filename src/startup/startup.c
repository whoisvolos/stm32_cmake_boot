#include "aux.h"
#include "init_hardware.h"

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern void
(*__preinit_array_start[])(void) __attribute__((weak));
extern void
(*__preinit_array_end[])(void) __attribute__((weak));
extern void
(*__init_array_start[])(void) __attribute__((weak));
extern void
(*__init_array_end[])(void) __attribute__((weak));
extern void
(*__fini_array_start[])(void) __attribute__((weak));
extern void
(*__fini_array_end[])(void) __attribute__((weak));

extern int main();

void zero_bss();
void fill_data();
void init_array();
void fini_array();

__attribute__((noreturn, weak)) void Reset_Handler() {
    // TODO: Move after hardware init?
    zero_bss();
    fill_data();
    init_array();

    // Hardware/clock init
    core_init();
    set_flash_latency();
    clock_reset();
    clock_init();

    main();

    fini_array();
    //_exit(main code)?

    for(;;) {}
}

inline void __attribute__((always_inline)) zero_bss()
{
    uint32_t *p = &_sbss;
    uint32_t *e = &_ebss;
    while (p < e)
        *p++ = 0;
}

inline void __attribute__((always_inline)) fill_data()
{
    uint32_t *from = &_sidata;
    uint32_t *e = &_edata;
    uint32_t *p = &_sdata;
    while (p < e)
        *p++ = *from++;
}

inline void __attribute__((always_inline)) init_array() {
    uint32_t count;
    uint32_t i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i] ();

    //_init ();

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
        __init_array_start[i] ();
}

inline void __attribute__((always_inline)) fini_array() {
    uint32_t count;
    uint32_t i;

    count = __fini_array_end - __fini_array_start;
    for (i = count - 1; i >= 0; i--)
        __fini_array_start[i] ();

    //_fini ();
}