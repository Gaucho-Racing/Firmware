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

add_project(
    NAME "NucleoG4-Echo"
    PATH "NucleoG4-Echo"
    SERIES "g4"
    SPECIFIER "g474re"
    HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
    CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
    CMSIS_MAJOR "5"
)

add_project(
    NAME "DashboardU5-Blinky"
    PATH "DashboardU5-Blinky"
    SERIES "u5"
    SPECIFIER "u5a9xx"
    HAL_GIT_TAG "v1.6.3" # https://github.com/STMicroelectronics/stm32u5xx-hal-driver/tree/v1.6.3
    CMSIS_GIT_TAG "v1.4.3" # https://github.com/STMicroelectronics/cmsis-device-u5/tree/v1.4.3
    CMSIS_MAJOR "5"
)

add_project(
    NAME "NucleoL4-Blinky"
    PATH "NucleoL4-Blinky"
    SERIES "l4"
    SPECIFIER "l476re"
    HAL_GIT_TAG "v1.13.6" # https://github.com/STMicroelectronics/stm32l4xx-hal-driver/tree/v1.13.6
    CMSIS_GIT_TAG "v1.7.5" # https://github.com/STMicroelectronics/cmsis-device-l4/tree/v1.7.5
    CMSIS_MAJOR "5"
)
