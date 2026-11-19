# TODO

# See ../Testbench/testbench.cmake for an example of what to put here

add_project(
    NAME "InboardSensors"
    PATH "InboardSensors"
    SERIES "h5"
    SPECIFIER "h503rb"
    HAL_GIT_TAG "v1.7.0" # https://github.com/STMicroelectronics/stm32h5xx-hal-driver/tree/v1.7.0
    CMSIS_GIT_TAG "v1.7.0" # https://github.com/STMicroelectronics/cmsis-device-h5/tree/v1.7.0
    CMSIS_MAJOR "5"
)
