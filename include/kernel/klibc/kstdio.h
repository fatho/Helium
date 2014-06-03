#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/// maximum resulting length of the format string in one call of printf
#define KPRINTF_LINE_MAX 256

/**
 * @brief this structure defines a backend for kernel print statements.
 * This is used for the switch from direct screen output to virtual terminals.
 */
typedef struct {
    /// minimal complete definition that writes one char at a time
    void (*kputchar)(int);
    /// function to write a string, if NULL, default implementation is used.
    void (*kputs)(const char*);
    /// function to write a string with maximum length, if NULL, default implementation is used.
    void (*kputsn)(const char*, size_t length);
} kstdio_backend_t;

/**
 * @brief Works like #snprintf, but accepts an explicit argument list.
 */
int vsnprintf(char* str, size_t strn, const char* format, va_list arglist);

/**
 * @brief Writes formatted text to the supplied buffer.
 * @param str the output buffer
 * @param strn size of the output buffer
 * @param format the format string which supports the usual format specifiers except for floating point values.
 * @return
 *   * if the return value is negative, an error occured
 *   * if the return value greater than or equal to zero,
 *     it denotes the number of bytes that would have been written to \c str.
 *     When this number is greater than \c strn, the string returned in \c str is incomplete.
 *
 *  @todo implement floating point formatting
 */
int snprintf(char* str, size_t strn, const char* format, ...);

/**
 * @brief Sets the current backend for kstdio.
 *
 * @param new_backend The new backend that should be used by output functions.
 * @return The old backend.
 */
kstdio_backend_t* kstdio_set_backend(kstdio_backend_t* new_backend);

/**
 * @brief writes a null terminated string to the VGA buffer.
 */
void kputs(const char* msg);

/**
 * @brief writes a null terminated string to the VGA buffer, but at most \c size characters.
 */
void kputsn(const char* msg, size_t size);

/**
 * @brief Writes a character to the screen.
 *
 * @param chr the character
 */
void kputchar(int chr);

/**
 * @brief Kernel level printf. Uses #snprintf internally for formatting.
 *
 */
void kprintf(const char*, ...);

/**
 * @brief same as \c kprintf(const char*, ...), but with explicit variable argument list.
 */
void kvprintf(const char* format, va_list vl);



#endif
