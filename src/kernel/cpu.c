/**
 * @file cpu.c
 *
 * @author Fabian Thorand
 * @date   05.06.2014
 *
 * @brief Prodides functions for accessing CPU features.
 */

#include "kernel/cpu.h"
#include "kernel/klibc/string.h"

/**
 * @brief Executes the \c cpuid instruction using the function \c code
 *
 * @param code The \c cpuid function code.
 * @param result Pointer to a #cpu_id_t structure receiving \c eax, \c ebx, \c ecx and \c edx.
 */
void cpuid(uint32_t code, cpu_id_t* result) {
    asm volatile (
            "movl %0,%%eax;\n"
            "cpuid;"
                : "=a"(result->eax), "=b"(result->ebx)
                  , "=c"(result->ecx), "=d"(result->edx)
                : "r"(code));
}


/**
 * @brief Writes the CPU vendor into the supplied buffer.
 *
 * @param vendor Buffer that receives the vendor string. Must be at least 12 bytes long.
 * This function does not write a null terminator.
 */
void cpu_get_vendor(char* vendor) {
    cpu_id_t result;
    cpuid(0, &result);
    // copy into vendor string
    memcpy(vendor, &result.ebx, 4);
    memcpy(vendor+4, &result.edx, 4);
    memcpy(vendor+8, &result.ecx, 4);
    vendor[13] = 0;
}

/**
 * @brief Checks if this CPU has support for MSRs.
 */
int cpu_has_msr() {
    cpu_id_t result;
    cpuid(1, &result);
    return (result.edx & CPUID_1_EDX_MSR);
}

/**
 * @brief Reads a 64 bit value from a MSR.
 * @param msr MSR index
 */
uint64_t cpu_msr_read(uint32_t msr) {
    uint32_t lo, hi;
    asm volatile("rdmsr"
            : "=a"(lo), "=d"(hi)
            : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

/**
 * @brief Writes a 64 bit value to a MSR.
 *
 * @param msr MSR index
 * @param value Value to write into the MSR.
 */
void cpu_msr_write(uint32_t msr, uint64_t value) {
    asm volatile("wrmsr"
            : : "c"(msr), "a"(value & 0xFFFFFFFF), "d"(value >> 32));
}

/**
 * @brief Writes \c value to the specified \c port.
 *
 * @param port Index of the output port.
 * @param value 8 bit value to write
 */
void cpu_outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN"(port), "a"(value));
}

/**
 * @brief Reads one byte from the specified port.
 *
 * @param port Index of the input port.
 * @return The byte that was read.
 */
uint8_t cpu_inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}
