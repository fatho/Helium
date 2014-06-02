/*
 * acpi.h
 *
 *  Created on: 30.05.2014
 *      Author: fabian
 */

#ifndef ACPI_H_
#define ACPI_H_

#define ACPI_V1 0
#define ACPI_V2 1

#include <stdint.h>
#include <stddef.h>

/// size of RSDP in ACPI 1.0
#define ACPI_V1_RSDP_SIZE 20

/**
 * @brief RSDP descriptor structure
 *
 * The fields after RsdtAddress are only valid, if ACPI version 2.0 or higher is supported.
 * Check the Revision field for that.
 */
typedef struct {
    /// "RSD PTR "
    char signature[8];
    /// This is the checksum of the fields defined in the ACPI 1.0 specification.
    /// This includes only the first 20 bytes of this table, bytes 0 to 19,
    /// including the checksum field. These bytes must sum to zero.
    uint8_t checksum;
    /// An OEM-supplied string that identifies the OEM.
    char oem_id[6];
    /// The revision of this structure. Larger revision numbers are backward
    /// compatible to lower revision numbers. The ACPI version 1.0
    /// revision number of this table is zero.
    uint8_t revision;
    /// 32 bit physical address of the RSDT.
    uint32_t rsdt_paddr;

    /// The length of the table, in bytes, including the header, starting from
    /// offset 0. This field is used to record the size of the entire table.
    uint32_t length;
    /// 64 bit physical address of the XSDT.
    uint64_t xsdt_paddr;
    /// This is a checksum of the entire table, including both checksum fields.
    uint8_t ext_checksum;
    uint8_t reserved[3];
}__attribute__((packed)) acpi_rsdp_t;

/**
 * @brief ACPI system description table header
 */
typedef struct {
    /// The ASCII string representation of the table identifier.
    char signature[4];
    /// The length of the table, in bytes, including the header, starting from
    /// offset 0. This field is used to record the size of the entire table.
    uint32_t length;
    /// The revision of the structure corresponding to the signature field for
    /// this table. Larger revision numbers are backward compatible to
    /// lower revision numbers with the same signature.
    uint8_t revision;
    /// The entire table, including the checksum field, must add to zero to
    /// be considered valid.
    uint8_t checksum;
    /// An OEM-supplied string that identifies the OEM.
    char oem_id[6];
    /// An OEM-supplied string that the OEM uses to identify the particular
    /// data table. This field is particularly useful when defining a definition
    /// block to distinguish definition block functions. The OEM assigns
    /// each dissimilar table a new OEM Table ID.
    char oem_table_id[8];
    /// An OEM-supplied revision number. Larger numbers are assumed to be newer revisions.
    uint32_t oem_revision;
    /// Vendor ID of utility that created the table. For tables containing
    /// Definition Blocks, this is the ID for the ASL Compiler.
    uint32_t creator_id;
    /// Revision of utility that created the table. For tables containing
    /// Definition Blocks, this is the revision for the ASL Compiler.
    uint32_t creator_revision;
}__attribute__((packed)) acpi_sdt_hdr_t;

/**
 * @brief Initializes the ACPI subsystem.
 *
 * @return one on success, zero on failure
 * @remark requires identity mapping for the lowest 1 MiB.
 */
void acpi_init();

/**
 * @brief Searches the designated memory area for the RSDP signature ("RSD PTR ").
 *
 * @param base the beginning of the memory area
 * @param size the size of the memory area in bytes
 *
 * @return The address of the RSDP or NULL, if it was not found.
 */
acpi_rsdp_t* acpi_search_rsdp(uintptr_t base, size_t size);

/**
 * @brief Validates the checksum of an ACPI table.
 *
 * @param table virtual address of the table
 * @param size size of the table in bytes
 *
 * @return One if the table is valid, zero otherwise.
 * @remark In a valid table, the sum over all bytes (including the checksum byte) must be zero.
 */
int acpi_verify_checksum(void* table, size_t size);

/**
 * @brief Writes debug information about ACPI to the screen.
 */
void acpi_debug_output();

#endif /* ACPI_H_ */
