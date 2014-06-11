/**
 * @file config.h
 * @date 11.06.2014
 * @author fabian
 *
 * @brief The placeholders in \c config.h.in are subsituted by CMAKE 
 * and the resulting header file is written to \c config.h.
 * 
 * Therefore you should not modify \c config.h itself.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define OS_NAME "Helium"
#define OS_VERSION_MAJOR 0
#define OS_VERSION_MINOR 1
#define OS_TARGET_TRIPLE "x86_64-helium-elf"
#define OS_COPYRIGHT "Copyright 2014 (c) Fabian Thorand"

// Definitions for Idris-Runtime
#define IDRIS_TARGET_OS "Helium"
#define IDRIS_TARGET_TRIPLE "x86_64-helium-elf"

#endif /* CONFIG_H_ */
