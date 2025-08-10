set(CHIP "") # REPLACE: with the appropriate platform name
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")
set(TARGET_FLAGS "") # REPLACE: with the appropriate linkerscript

function(add_executable_${CHIP} TARGET_NAME)

set(CHIP "") # REPLACE: with the appropriate platform name
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")
set(TARGET_FLAGS "") # REPLACE: with the appropriate linkerscript
add_executable(${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "") # REPLACE: with the appropriate linkerscript

# Cleanup (do not change, leave these 3 lines as is, trust)
set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
set(CHIP_PATH "YOUHAVENOTCONFIGUREDCHIPPATHRIGHT!")
set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")

endfunction()

add_library(${CHIP}_LIB INTERFACE)

# enable_language(C ASM)

# used to strip the last 2 characters of CHIP to make the compile def
string(SUBSTRING ${CHIP} 0 9 TEMP)  # FIXME Scary
target_compile_definitions(${CHIP}_LIB INTERFACE
	USE_FULL_LL_DRIVER
	USE_HAL_DRIVER
	${TEMP}xx # REPLACE: Check that this either works or needs to be overwritten w/ the new
    FIXME   # If you get a TON of legacy warnings about things then replace this with the STM32?? of the chip
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
