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
	"${CMAKE_C_LINK_FLAGS} ${TARGET_FLAGS} --specs=nano.specs -Wl,--gc-sections -Wl,--start-group -lc -lm -Wl,--end-group -Wl,--print-memory-usage"
)
set(
	CMAKE_C_FLAGS
	"${CMAKE_C_FLAGS} ${TARGET_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections -fstack-usage"
)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")

# Build Type Configuration
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g3 -Werror")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g0 -Werror -flto=auto")
	set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -flto=auto")
elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g0 -Werror -flto=auto")
	set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -flto=auto")
else()
	message(SEND_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()
