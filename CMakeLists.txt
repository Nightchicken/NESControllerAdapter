# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "../../pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(Controller C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()


add_executable(Controller Controller.cpp usb_descriptors.c)

pico_set_program_name(Controller "Controller")
pico_set_program_version(Controller "0.1")

pico_enable_stdio_uart(Controller 0)
pico_enable_stdio_usb(Controller 0)

# Add the standard library to the build
target_link_libraries(Controller
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Controller PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(Controller
        hardware_pio
        tinyusb_device
        tinyusb_board
        )

pico_add_extra_outputs(Controller)

