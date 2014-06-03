/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains a basic screen output system.
 */

#include "kernel/klibc/kstdio.h"
#include "kernel/klibc/string.h"
#include "kernel/klibc/kstdio_screen.h"
#include <stdarg.h>
#include <stdint.h>

static kstdio_backend_t* backend = &kstdio_screen_direct;

/**
 * @brief Sets the current backend for kstdio.
 *
 * @param new_backend The new backend that should be used by output functions.
 * @return The old backend.
 */
kstdio_backend_t* kstdio_set_backend(kstdio_backend_t* new_backend) {
    kstdio_backend_t* old = backend;
    backend = new_backend;
    return old;
}

void kputchar(int ch) {
    if(backend && backend->kputchar) {
        backend->kputchar(ch);
    }
}

/**
 * @brief writes a null terminated string to the VGA buffer.
 */
void kputs(const char* message) {
    if(backend && backend->kputsn) {
        backend->kputs(message);
    } else if(backend && backend->kputchar) {
        // output until '\0' terminator or length
        while (*message) {
            backend->kputchar(*message);
            message += 1;
        }
    }
}

/**
 * @brief writes a null terminated string to the VGA buffer, but at most \c size characters.
 */
void kputsn(const char* message, size_t length) {
    if(backend && backend->kputsn) {
        backend->kputsn(message, length);
    } else if(backend && backend->kputchar) {
        // output until '\0' terminator or length
        while (*message && length--) {
            backend->kputchar(*message);
            message += 1;
        }
    }
}

/**
 * @brief Kernel mode printf, uses #snprintf.
 */
void kprintf(const char* format, ...) {
    va_list vl;
    va_start(vl, format);

    kvprintf(format, vl);

    va_end(vl);
}


/**
 * @brief same as \c kvprintf(const char*, ...), but with explicit variable argument list.
 */
void kvprintf(const char* format, va_list vl) {
    char linebuf[KPRINTF_LINE_MAX];
    vsnprintf(linebuf, sizeof(linebuf), format, vl);
    kputs(linebuf);
}
