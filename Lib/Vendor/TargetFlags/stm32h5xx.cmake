target_compile_options(
	"${TARGET_NAME}"
	PRIVATE
		-mcpu=cortex-m33
		-mfpu=fpv5-sp-d16
		-mfloat-abi=hard
)

target_link_options(
	"${TARGET_NAME}"
	PRIVATE
		-mcpu=cortex-m33
		-mfpu=fpv5-sp-d16
		-mfloat-abi=hard
)
