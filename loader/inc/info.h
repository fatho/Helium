/*
 * info.h
 *
 *  Created on: 07.05.2014
 *      Author: fabian
 */

#ifndef INFO_H_
#define INFO_H_

typedef struct {
    uint64_t reserved;
}__attribute__((packed)) helium_module_t;

typedef struct {
    uint64_t free_addr; // contains address of first free page
}__attribute__((packed)) helium_info_t;

#endif /* INFO_H_ */
