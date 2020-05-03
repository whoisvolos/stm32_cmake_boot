INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
SET(CMAKE_SIZE arm-none-eabi-size)

SET(LINKER_SCRIPT_DIRECTORY ${PROJECT_SOURCE_DIR}/linker_scripts)
SET(LINKER_SCRIPT ${LINKER_SCRIPT_DIRECTORY}/STM32F401CCUx_FLASH.ld)
SET(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -ffunction-sections -fdata-sections -g -ggdb3 -fno-common -fmessage-length=0")

SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++2a -fno-exceptions")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")

SET(CMAKE_EXE_LINKER_FLAGS "-flto -Wl,-gc-sections -nostartfiles -nostdlib -T ${LINKER_SCRIPT}")
# -specs=nosys.specs -specs=nano.specs