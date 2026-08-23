add_project(
    NAME "CUBEMXTESTING"
    PATH "CUBEMXTESTING"
    SERIES "g4"
    SPECIFIER "g474re"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)

add_project(
    NAME "NucleoG4-Blinky"
    PATH "NucleoG4-Blinky"
    SERIES "g4"
    SPECIFIER "g474re"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)

add_project(
    NAME "NucleoG4-Hello"
    PATH "NucleoG4-Hello"
    SERIES "g4"
    SPECIFIER "g474re"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)
