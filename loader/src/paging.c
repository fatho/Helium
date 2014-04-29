#include <stdint.h>

/**
 * Implements basic paging functionality for the loader.
 * Sets up the Page Map Level 4 Table at 0x1000 and identity maps
 * the first 2 MiBs.
 */

// Page Map Level 4 Table
uint64_t* pml4t = 0x1000;


void setup_identity_paging() {

}
