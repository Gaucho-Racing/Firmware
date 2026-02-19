add_library(STM32_Cube_Common INTERFACE)

target_include_directories(STM32_Cube_Common
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(STM32_Cube_Common
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/syscalls.c
		${CMAKE_CURRENT_LIST_DIR}/Src/sysmem.c
)
