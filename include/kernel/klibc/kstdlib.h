/**
 * @file kstdlib.h
 * @date 11.06.2014
 * @author fabian
 *
 * @brief
 */

#ifndef KSTDLIB_H_
#define KSTDLIB_H_

/**
 * @brief Allocates a block of kernel memory.
 * @return currently, #kmalloc always returns 0.
 * @todo Implement #kmalloc
 */
#define kmalloc(x) 0
/**
 * @brief Frees a block of kernel memory previously allocated with #kmalloc.
 * @todo Implement #kfree
 */
#define kfree(x)

/**
 * @brief Converts string to double
 *
 * @remark Who needs floating point numbers in kernel mode anyway?
 * @todo Implement #strtod
 */
double strtod (const char* str, char** endptr);

/**
 * @brief Converts string to long integer.
 */
long int strtol (const char* str, char** endptr, int base);


#endif /* KSTDLIB_H_ */
