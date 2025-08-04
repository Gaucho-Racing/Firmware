set(CHIP "") # REPLACE: with the appropriate platform name
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")

set(TARGET_FLAGS "") # REPLACE: with the appropriate linkerscript

add_library(${CHIP}_LIB INTERFACE)

# enable_language(C ASM)

# used to strip the last 2 characters of CHIP to make the compile def
string(SUBSTRING ${CHIP} 0 9 TEMP)  # Scary
target_compile_definitions(${CHIP}_LIB INTERFACE 
	USE_FULL_LL_DRIVER
	USE_HAL_DRIVER
	${TEMP}xx # REPLACE: Check that this either works or needs to be overwritten w/ the new
)


target_include_directories(${CHIP}_LIB INTERFACE
    ${CHIP_PATH}/Drivers/CMSIS/Include
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Inc
    ${CHIP_PATH}/Drivers/CMSIS/Device/ST/____/Include # REPLACE: Whatever the new cmsis include path is
)

target_sources(${CHIP}_LIB INTERFACE
    # REPLACE: everything here with the files in the hal-driver & the startup_<platform>.s
    # you can just ls and use that but don't forget to comment out the templates
)


function(add_executable_${CHIP} TARGET_NAME)

set(TARGET_FLAGS "") # REPLACE with the appropriate flags

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "") # REPLACE: with the appropriate linkerscript

endfunction()
