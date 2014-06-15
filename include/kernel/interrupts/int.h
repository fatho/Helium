/**
 * @file int.h
 *
 * @author fabian
 * @date   15.06.2014
 *
 * @brief 
 */
#ifndef INT_H_
#define INT_H_

/**
 * @brief Enables interrupts.
 */
#define INT_ENABLE() asm volatile ("sti")

/**
 * @brief Disables interrupts.
 */
#define INT_DISABLE() asm volatile ("cli")


#endif /* INT_H_ */
