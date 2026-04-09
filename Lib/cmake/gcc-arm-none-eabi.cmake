# System Configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compiler Configuration
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

# Executable Suffixes and Type
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Linker, Compiler, and Assembler Flags
set(
	CMAKE_C_LINK_FLAGS
	"${TARGET_FLAGS} --specs=nano.specs -Wl,--gc-sections -Wl,--start-group -lc -lm -Wl,--end-group -Wl,--print-memory-usage"
)
set(
	CMAKE_C_FLAGS
	"${TARGET_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections -fstack-usage"
)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")

# Compiler Flags for Build Types
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(
	CMAKE_C_FLAGS_RELWITHDEBINFO
	"-Og -g3 -Werror"
)
set(CMAKE_C_FLAGS_RELEASE "-O2 -g0 -Werror -flto=auto")
set(
	CMAKE_C_FLAGS_MINSIZEREL
	"-Os -g0 -Werror -flto=auto"
)

# Linker Flags for Build Types
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fno-lto")
set(
	CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
	"-Wl,--fatal-warnings"
)
set(
	CMAKE_EXE_LINKER_FLAGS_RELEASE
	"-Wl,--fatal-warnings -flto=auto"
)
set(
	CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
	"-Wl,--fatal-warnings -flto=auto"
)
