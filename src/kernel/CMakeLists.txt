cmake_minimum_required(VERSION 2.8)
include(CMakeForceCompiler)

project("kernel")

# additional files
set(LDFILE "${PROJECT_SOURCE_DIR}/linker.ld")

# include files
include_directories(${PROJECT_SOURCE_DIR}/../../include)

# compiler flags
set(ASM_FLAGS "-f elf64")
set(C_FLAGS "-std=gnu99 -ffreestanding -Wall -Wextra")
set(LD_FLAGS "-T ${LDFILE} -z max-page-size=0x1000")

# compilers
set(CMAKE_SYSTEM_NAME "Helium")
CMAKE_FORCE_C_COMPILER(x86_64-elf-gcc GNU)
set(CMAKE_C_LINK_EXECUTABLE "x86_64-elf-ld <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_COMPILER "yasm")

# gather files
file(GLOB_RECURSE CFILES *.c)
file(GLOB_RECURSE ASMFILES *.asm)

# define target
add_executable(${KERNEL_EXECUTABLE} ${CFILES} ${ASMFILES})

# installation
install(TARGETS ${KERNEL_EXECUTABLE} DESTINATION boot)

# set flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_C_FLAGS "${C_FLAGS}")
set(CMAKE_ASM_FLAGS "${ASM_FLAGS}")
set_target_properties(${KERNEL_EXECUTABLE} PROPERTIES LINK_FLAGS "${LD_FLAGS}")