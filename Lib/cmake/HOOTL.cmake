set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER_FORCED TRUE)

enable_testing()

if(UNIX)
	set(_ADDRESS_SANITIZER_DEFAULT_ ON)
else()
	set(_ADDRESS_SANITIZER_DEFAULT_ OFF)
endif()

option(
	ADDRESS_SANITIZER
	"Enable address sanitizer for tests, breaks Valgrind"
	${_ADDRESS_SANITIZER_DEFAULT_}
)

add_compile_options(
	-fshort-enums
	-fdata-sections
	-ffunction-sections
	-fstack-usage
	-fno-lto
	-Wall
	-Wextra
	-Werror
	-Wpedantic
	-Wvla
	-g
	-DHOOTL_TEST
)

if(APPLE) # MacOS has a different syntax for linker fatal warnings
	add_link_options(-Wl,-fatal_warnings)
else()
	add_link_options(-Wl,--fatal-warnings)
endif()

if(ADDRESS_SANITIZER)
	message(STATUS "Address sanitizer enabled for tests")

	if(APPLE) # MacOS does not support leak sanitizer
		add_compile_options(
			-fno-omit-frame-pointer
			-fsanitize=address
			-fsanitize=undefined
		)
		add_link_options(
			-fsanitize=address
			-fsanitize=undefined
		)
	elseif(WIN32)
		message(FATAL_ERROR "Address sanitizer is not supported on Windows")
	else() # Assume Linux
		add_compile_options(
			-fno-omit-frame-pointer
			-fsanitize=address
			-fsanitize=undefined
			-fsanitize=leak
			# -fsanitize=thread
		)
		add_link_options(
			-fsanitize=address
			-fsanitize=undefined
			-fsanitize=leak
			# -fsanitize=thread
		)
	endif()
endif()
