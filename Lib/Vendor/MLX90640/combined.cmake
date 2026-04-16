add_library(MLX90640_LIB INTERFACE)

target_include_directories(MLX90640_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
	MLX90640_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/MLX90640_API.c
		${CMAKE_CURRENT_LIST_DIR}/Src/arm_sqrt_q31.c
)

target_link_libraries(MLX90640_LIB INTERFACE m)

# FIXME Figure out an alternative to this down the road as it is quite a drastic speed reduction to not use the FPU
target_compile_options(MLX90640_LIB INTERFACE -Wno-double-promotion -Wno-float-conversion)
