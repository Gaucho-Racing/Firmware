target_compile_options(
    "${TARGET_NAME}"
    PRIVATE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

target_link_options(
    "${TARGET_NAME}"
    PRIVATE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)
