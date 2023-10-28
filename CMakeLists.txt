set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.19)

# specify cross compilers and tools
message(STATUS "Operation system is ${CMAKE_HOST_SYSTEM_NAME}")
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(COMPILER_DIR $ENV{CH32_DEV}bin/riscv-none-embed-)
    set(CMAKE_C_COMPILER ${COMPILER_DIR}gcc)
    set(CMAKE_CXX_COMPILER ${COMPILER_DIR}g++)
    set(CMAKE_ASM_COMPILER ${COMPILER_DIR}gcc)
    set(CMAKE_LINKER ${COMPILER_DIR}gcc)
    set(CMAKE_AR ${COMPILER_DIR}ar)
    set(CMAKE_OBJCOPY ${COMPILER_DIR}objcopy)
    set(CMAKE_OBJDUMP ${COMPILER_DIR}objdump)
    set(SIZE ${COMPILER_DIR}size.exe)
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(COMPILER_DIR $ENV{CH32_DEV}bin/riscv-none-elf-)
    set(CMAKE_C_COMPILER ${COMPILER_DIR}gcc.exe)
    set(CMAKE_CXX_COMPILER ${COMPILER_DIR}g++.exe)
    set(CMAKE_ASM_COMPILER ${COMPILER_DIR}gcc.exe)
    set(CMAKE_LINKER ${COMPILER_DIR}gcc.exe)
    set(CMAKE_AR ${COMPILER_DIR}ar.exe)
    set(CMAKE_OBJCOPY ${COMPILER_DIR}objcopy.exe)
    set(CMAKE_OBJDUMP ${COMPILER_DIR}objdump.exe)
    set(SIZE ${COMPILER_DIR}size.exe)
endif ()

# project settings
project(CH32V307VC C ASM)
set(CMAKE_C_STANDARD 99)

# compile and running settings
add_compile_options(-march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmessage-length=0)
add_compile_options(-fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant -Wunused -Wuninitialized)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)
add_compile_options(-Og -g)

add_link_options(-nostartfiles -Xlinker --gc-sections -Wl,--print-memory-usage,-Map,${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(--specs=nano.specs --specs=nosys.specs -march=rv32imafcxw -mabi=ilp32f -flto)

include_directories(system user library/inc freertos hardware device fatfs)
file(GLOB_RECURSE SOURCES "system/startup_ch32v30x.s" "system/*.c" "library/src/*.c" "user/*.c" "freertos/*.c"
        "freertos/portasm.s" "hardware/*.c" "device/*.c" "fatfs/*.c")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/system/linkscript.ld)
add_link_options(-T ${LINKER_SCRIPT})

add_custom_target(version_h BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/../system/version.h"
        COMMAND ${CMAKE_COMMAND} -D SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/gitversion.cmake
        COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/system/system.c)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf m)
add_dependencies(${PROJECT_NAME}.elf version_h)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

set(LIST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble -M xw $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LIST_FILE}
        COMMENT "Building ${LIST_FILE}")
