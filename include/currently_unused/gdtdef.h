/*
 * gdt.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef GDT_H_
#define GDT_H_

#include <stdint.h>

#define GDT_ACCESS_PRESENT   (1<<7)
#define GDT_ACCESS_EXECUTE   (1<<3)
#define GDT_ACCESS_READWRITE (1<<1)
#define GDT_ACCESS_PRIVL(r)  (r << 5)

#define GDT_FLAGS_GRANULARITY (1<<3)
#define GDT_FLAGS_SIZE        (1<<2)
#define GDT_FLAGS_LONGMODE    (1<<1)

#endif /* GDT_H_ */
