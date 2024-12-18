# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set FreeRTOS port before project
set(FREERTOS_PORT GCC_ARM_CM0 CACHE STRING "")
set(FREERTOS_HEAP_IMPLEMENTATION heap_4 CACHE STRING "")

# Define project
project(Anti-Theft C CXX ASM)

# Initialize pico-sdk
pico_sdk_init()

# Add FreeRTOS kernel library
add_library(FreeRTOS-Kernel STATIC
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/tasks.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/queue.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/list.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/timers.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/MemMang/${FREERTOS_HEAP_IMPLEMENTATION}.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c
)

target_include_directories(FreeRTOS-Kernel PUBLIC
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${CMAKE_SOURCE_DIR}/config
    ${PICO_SDK_PATH}/src/rp2_common/cmsis/include
    ${PICO_SDK_PATH}/src/rp2040/hardware_regs/include
)

target_link_libraries(FreeRTOS-Kernel
    freertos_config
    pico_platform
    hardware_exception
)

# Create FreeRTOS config
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
    INTERFACE
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${PICO_SDK_PATH}/src/common/pico_stdlib/include
    ${PICO_SDK_PATH}/src/common/pico_base/include
    ${PICO_SDK_PATH}/src/common/pico_sync/include
    ${PICO_SDK_PATH}/src/common/pico_time/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_platform/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_base/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_irq/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_watchdog/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_boot_lock/include
    ${PICO_SDK_PATH}/src/rp2_common/boot_stage2/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_bootrom/include
    ${PICO_SDK_PATH}/src/rp2040/hardware_regs/include
    ${PICO_SDK_PATH}/src/rp2040/hardware_structs/include
    ${PICO_SDK_PATH}/src/boards/include
)


target_compile_definitions(freertos_config
    INTERFACE
    projCOVERAGE_TEST=0
    PICO_SDK_VERSION_MAJOR=1
    PICO_SDK_VERSION_MINOR=5
    PICO_SDK_VERSION_REVISION=1
)


add_executable(${PROJECT_NAME} 
    main.c
    Drivers/GPS/gps.c
    Drivers/LED/led.c
    Drivers/GSM/gsm.c
    Drivers/Buzzer/buzzer.c
    Drivers/Vibration/vibration.c
    Drivers/Motor/motor.c
    Wifi/wifi.c
)

add_dependencies(${PROJECT_NAME} FreeRTOS-Kernel)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${CMAKE_SOURCE_DIR}/config
)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_adc
    hardware_gpio
    hardware_uart
    hardware_pwm
    hardware_clocks
    hardware_rtc
    hardware_uart
    hardware_irq
    hardware_watchdog
    FreeRTOS-Kernel
    freertos_config
    pico_time
    pico_sync
    pico_base
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)