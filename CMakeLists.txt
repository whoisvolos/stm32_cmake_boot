cmake_minimum_required(VERSION 3.16)
project(stm32_cmake_boot LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MCU_FAMILY STM32F4)
set(MCU_SERIES STM32F401xC)
set(MCU_SERIES STM32F401CCU6)

add_definitions(
        # MCU family
        -DSTM32F4
        # Concrete MCU
        -DSTM32F401xC
        # HSE (crystal resonator) freq, MHz, useless for now
        -DHSE_VALUE=25000000
        # HSI (internal RC oscillator) freq, MHz, useless for now
        -DHSI_VALUE=16000000
        # Which one to use (HSE/HSI), useless for now
        -DCLOCK_SOURCE=HSE
)

include_directories(src/include src/include/cmsis src/include/device)
file(GLOB_RECURSE USER_SOURCES src/*.c src/*.cpp src/include/*.c)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")
set(HEX_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>)