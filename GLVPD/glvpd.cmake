include(${CMAKE_CURRENT_LIST_DIR}/Common/common_glvpd.cmake)

add_project(
    NAME "MCU1"
    PATH "MCU1"
    SERIES "g4"
    SPECIFIER "g431cbu6"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)

add_project(
    NAME "MCU2"
    PATH "MCU2"
    SERIES "g4"
    SPECIFIER "g431cbu6"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)

add_project(
    NAME "MCU3"
    PATH "MCU3"
    SERIES "g4"
    SPECIFIER "g431cbu6"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)
