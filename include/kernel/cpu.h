/**
 * @file cpu.h
 *
 * @author fabian
 * @date   05.06.2014
 *
 * @brief 
 */
#ifndef CPU_H_
#define CPU_H_

#include <stdint.h>

#define CPUID_1_EDX_MSR (1<<5)

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpu_id_t;


/**
 * @brief Executes the \c cpuid instruction using the function \c code
 *
 * @param code The \c cpuid function code.
 * @param result Pointer to a #cpu_id_t structure receiving \c eax, \c ebx, \c ecx and \c edx.
 */
void cpuid(uint32_t code, cpu_id_t* result);

/**
 * @brief Writes the CPU vendor into the supplied buffer.
 *
 * @param vendor Buffer that receives the vendor string. Must be at least 12 bytes long.
 * This function does not write a null terminator.
 */
void cpu_get_vendor(char* vendor);

/**
 * @brief Checks if this CPU has support for MSRs.
 */
int cpu_has_msr();

/**
 * @brief Reads a 64 bit value from a MSR.
 * @param msr MSR index
 */
uint64_t cpu_msr_read(uint32_t msr);

/**
 * @brief Writes a 64 bit value to a MSR.
 * @param msr MSR index
 */
void cpu_msr_write(uint32_t msr, uint64_t value);

/**
 * @brief Writes \c value to the specified \c port.
 *
 * @param port Index of the output port.
 * @param value 8 bit value to write
 */
void cpu_outb(uint16_t port, uint8_t value);

/**
 * @brief Reads one byte from the specified port.
 *
 * @param port Index of the input port.
 * @return The byte that was read.
 */
uint8_t cpu_inb(uint16_t port);

#endif /* CPU_H_ */
