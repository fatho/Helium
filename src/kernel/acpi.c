/**
 * @file
 *
 * @author Fabian Thorand
 * @date May 30, 2014
 *
 * @brief Contains basic functionality to find and parse ACPI tables.
 */

#include "kernel/acpi.h"
#include "kernel/debug.h"
#include "kernel/page.h"
#include "kernel/screen.h"
#include "kernel/string.h"
#include "kernel/util.h"

#include <stdint.h>

/// pointer to the RSDP, or NULL if it does not exist
static acpi_rsdp_t* rsdp = NULL;

/**
 * @brief Validates the checksum of an ACPI table.
 *
 * @param table virtual address of the table
 * @param size size of the table in bytes
 *
 * @return One if the table is valid, zero otherwise.
 * @remark In a valid table, the sum over all bytes (including the checksum byte) must be zero.
 */
int acpi_verify_checksum(void* table, size_t size) {
    uint8_t* mem = (uint8_t*)table;
    uint8_t checksum = 0;
    for(size_t i = 0; i < size; i++) {
        checksum += mem[i];
    }
    return (checksum == 0);
}

/**
 * @brief Searches the designated memory area for the RSDP signature ("RSD PTR ").
 *
 * @param base the beginning of the memory area
 * @param size the size of the memory area in bytes
 *
 * @return The address of the RSDP or NULL, if it was not found.
 */
acpi_rsdp_t* acpi_search_rsdp(uintptr_t base, size_t size) {
    const char* signature = "RSD PTR ";
    // align to 16 bytes
    base = (base + 0xF) & ~0xF;
    size = size & ~0xF;

    char* memory = (char*)base;
    for(size_t offset = 0; offset < size; offset += 16) {
        if(strncmp(signature, memory + offset, 8) == 0) {
            acpi_rsdp_t* candidate = (acpi_rsdp_t*)(memory + offset);
            // signature match, verify checksum
            if(!acpi_verify_checksum(candidate, ACPI_V1_RSDP_SIZE)) {
                continue;
            }

            // if ACPI 2.0 or higher, verify full table
            if(candidate->revision >= ACPI_V2) {
                if(!acpi_verify_checksum(candidate, candidate->length)) {
                    continue;
                }
            }

            return candidate;
        }
    }

    return NULL;
}

acpi_sdt_hdr_t* acpi_map_table(uintptr_t vaddr, uintptr_t table_paddr) {
    uintptr_t physpage = PAGE_FLOOR(table_paddr);
    vaddr &= ~0xFFF;
    kprintf("[acpi_map_table] %llx to %llx\n", vaddr, table_paddr);
    // identity map first page of XSDT (read only)
    paging_map(vaddr, physpage, 0, 0);
    acpi_sdt_hdr_t* table = (acpi_sdt_hdr_t*)(vaddr + (table_paddr - physpage));
    kprintf("[acpi_map_table] table mapped to %llx\n", (uintptr_t)table);
    kprintf("[acpi_map_table] verification: %llx\n", paging_get_physical_addr((uintptr_t)table));
    // map rest of XSDT if its larger than one page
    MAGIC_BREAK;
    size_t pagesize = PAGE_CEIL(table->length);
    for(size_t offset = 0x1000; offset < pagesize; offset += 0x1000) {
        kprintf("[acpi_map_table] mapping next page at offset %llx\n", (uint64_t)offset);
        paging_map(vaddr + offset, physpage + offset, 0, 0);
    }
    return table;
}

void acpi_unmap_table(acpi_sdt_hdr_t* table) {
    size_t freesize = PAGE_CEIL(table->length);
    uintptr_t vaddr = (uintptr_t)table;
    for(size_t offset = 0; offset < freesize; offset += 0x1000) {
        paging_unmap(vaddr + offset);
    }
}

void acpi_parse_rsdp() {
    kputs("[acpi_parse_rsdp] enter\n");
    if(rsdp->revision >= ACPI_V2) {
        kputs("[acpi_parse_rsdp] ACPI 2.0 available\n");
        // XSDT is available
        acpi_sdt_hdr_t* xsdt = acpi_map_table(rsdp->xsdt_paddr, rsdp->xsdt_paddr);
        if(acpi_verify_checksum(xsdt, xsdt->length)) {
            // TODO: Parse XSDT
            return;
        }
    }
    kputs("[acpi_parse_rsdp] fallback to RSDT\n");
    // fallback to RSDT

    // identity map RSDT (read only)
    acpi_sdt_hdr_t* rsdt = acpi_map_table(rsdp->rsdt_paddr, rsdp->rsdt_paddr);
    kputs("[acpi_parse_rsdp] RSDT mapped\n");
    if(!acpi_verify_checksum(rsdt, rsdt->length)) {
        kpanic("[ACPI] RSDT has invalid checksum and XSDT is invalid or not present");
    }

    uint32_t* rsdt_data = (rsdp->rsdt_paddr + sizeof(acpi_sdt_hdr_t));
    for(int i = 0; i < (rsdt->length - sizeof(acpi_sdt_hdr_t)) / 4; i++) {
        kprintf("%x ", rsdt_data[i]);
        if((i & 0x3) == 3) {
            kputchar('\n');
        }
    }
    kputchar('\n');
}

acpi_sdt_hdr_t* acpi_search_table(char* signature) {
    return NULL;
}

/**
 * @brief Initializes the ACPI subsystem.
 *
 * @remark requires identity mapping for the lowest 1 MiB.
 */
void acpi_init() {
    // get EBDA address
    uintptr_t ebda_addr = *((uint16_t*)0x040E) << 4;
    // search first 1 KiB of EBDA
    rsdp = acpi_search_rsdp(ebda_addr, 0x400);
    // search main BIOS area
    if(rsdp == NULL) {
        rsdp = acpi_search_rsdp(0x000E0000, 0x00100000 - 0x000E0000);
    }
    if(rsdp == NULL) {
        kpanic("[ACPI] RSDP not found");
    }
    acpi_parse_rsdp();
}


/**
 * @brief Writes debug information about ACPI to the screen.
 */
void acpi_debug_output() {
    screen_color = SCREENCOLOR(BLACK, BROWN | LIGHT_OR_BLINK);
    if(rsdp) {
        kprintf("ACPI rev %d ", (uint32_t)rsdp->revision);
        kputsn(rsdp->oem_id, sizeof(rsdp->oem_id));
        kputs("\n");
        kprintf("RSDT: %x\n", rsdp->rsdt_paddr);
        if(rsdp->revision >= ACPI_V2) {
            kprintf("XSDT: %llx\n", rsdp->xsdt_paddr);
        }
    } else {
        kprintf("No ACPI tables detected. Call acpi_init first!");
    }
    screen_color = SCREENCOLOR(BLACK, WHITE);
}

