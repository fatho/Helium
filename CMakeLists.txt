cmake_minimum_required(VERSION 2.8)
project("Helium")

# global defintions
set(OS_NAME "Helium")
set(KERNEL_NAME "kernel")
set(KERNEL_EXECUTABLE "${KERNEL_NAME}.sys")


# default prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist" CACHE PATH "Helium install prefix")

# build kernel
add_subdirectory(src/grub)
add_subdirectory(src/kernel)

# options
option(BUILD_DOC "Use Doxygen to create the HTML based API documentation" OFF)

FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
    "Doxygen is needed to build the documentation. Please install it correctly")
endif()
#-- Configure the Template Doxyfile for our specific project
configure_file(Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)

if(BUILD_DOC)
    #-- Add a custom target to run Doxygen when ever the project is built
    add_custom_target (doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
    #-- Install rule for documentation
    install(DIRECTORY ${PROJECT_BINARY_DIR}/doc DESTINATION .)
else()
    #-- Add a custom target to run Doxygen when ever the project is built
    add_custom_target (doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
endif()