# TODO

# See ../Testbench/testbench.cmake for an example of what to put here

add_project(
    NAME "InboardSensors"
    PATH "InboardSensors"
    SERIES "h5"
    SPECIFIER "h503rb"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)
