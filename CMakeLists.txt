#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.19)

# specify cross compilers and tools
set(COMPILER_DIR $ENV{CH32_DEV}bin/riscv-none-embed-)
set(CMAKE_C_COMPILER ${COMPILER_DIR}gcc.exe)
set(CMAKE_CXX_COMPILER ${COMPILER_DIR}g++.exe)
set(CMAKE_ASM_COMPILER ${COMPILER_DIR}gcc.exe)
set(CMAKE_LINKER ${COMPILER_DIR}gcc.exe)
set(CMAKE_AR ${COMPILER_DIR}ar.exe)
set(CMAKE_OBJCOPY ${COMPILER_DIR}objcopy.exe)
set(CMAKE_OBJDUMP ${COMPILER_DIR}objdump.exe)
set(SIZE ${COMPILER_DIR}size.exe)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(CH32V307VC C ASM)
set(CMAKE_C_STANDARD 99)

add_definitions(-DTOOLCHAIN_PATH="${COMPILER_DIR}" -DPROJECT_PATH="${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")
add_compile_options(-march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmessage-length=0)
add_compile_options(-fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant -Wunused -Wuninitialized)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)
add_compile_options(-O2 -g)

add_link_options(-nostartfiles -Xlinker --gc-sections -Wl,--print-memory-usage,-Map,${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(--specs=nano.specs --specs=nosys.specs -march=rv32imafcxw -mabi=ilp32f -flto)

include_directories(system user library/inc freertos hardware)
file(GLOB_RECURSE SOURCES "system/startup_ch32v30x.s" "system/*.c" "library/src/*.c" "user/*.c" "freertos/*.c"
        "freertos/portasm.s" "hardware/*.c")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/system/linkscript.ld)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf m)

#add_custom_target(download_program
#        COMMAND $ENV{CH32_DEV}../openocd/bin/openocd.exe -f ${CMAKE_CURRENT_SOURCE_DIR}/wch-riscv.cfg
#        -c "tcl_port disabled" -c "gdb_port disabled" -c "tcl_port disabled" -c init -c halt
#        -c "program $<TARGET_FILE:${PROJECT_NAME}.elf>" -c wlink_reset_resume -c exit)
add_custom_target(download_program
        COMMAND E:/Desktop/Files/AutoInstall/pack/openocd/bin/openocd.exe -f ${CMAKE_CURRENT_SOURCE_DIR}/wch-riscv.cfg
        -c "tcl_port disabled" -c "gdb_port disabled" -c "tcl_port disabled" -c init -c halt -c
        "program $<TARGET_FILE:${PROJECT_NAME}.elf>" -c wlink_reset_resume -c exit
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/open_putty.bat COM9 115200)

add_custom_command(TARGET ${PROJECT_NAME}.elf download_program PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/system/system.c)

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
