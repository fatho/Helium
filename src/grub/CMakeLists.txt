cmake_minimum_required(VERSION 2.8)

project("grub")

configure_file(${PROJECT_SOURCE_DIR}/grub.cfg.in ${PROJECT_BINARY_DIR}/grub.cfg)  

install(FILES ${PROJECT_BINARY_DIR}/grub.cfg DESTINATION boot/grub)