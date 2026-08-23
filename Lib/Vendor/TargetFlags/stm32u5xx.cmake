target_compile_options(
    ${TARGET_NAME}
    PRIVATE
    -mcpu=cortex-m33
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
)
