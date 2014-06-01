/*
 * gdt.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef GDT_H_
#define GDT_H_

#include <stdint.h>

void set_gdt_entry(uint64_t* gdt_data, int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

#endif /* GDT_H_ */
