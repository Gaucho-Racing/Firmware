# CMSIS Shared Configuration
# Provides centralized CMSIS driver management for all STM32 boards

# Function to create a shared CMSIS library for STM32U5 targets
function(create_shared_cmsis_stm32u5 TARGET_NAME)
    # Create the interface library for shared CMSIS
    add_library(${TARGET_NAME} INTERFACE)
    
    # CMSIS include directories (relative to board root)
    target_include_directories(${TARGET_NAME} INTERFACE
        ../lib/CMSIS/Device/ST/STM32U5xx/Include
        ../lib/CMSIS/Include
        ../lib/CMSIS/Core/Include
    )
    
    # Add CMSIS system source file if it exists in the shared location
    # Note: system_stm32u5xx.c is typically board-specific and remains in Core/Src
    # but CMSIS headers are now shared
endfunction()

# Function to create a shared CMSIS library for STM32G4 targets  
function(create_shared_cmsis_stm32g4 TARGET_NAME)
    # Create the interface library for shared CMSIS
    add_library(${TARGET_NAME} INTERFACE)
    
    # CMSIS include directories (relative to board root)
    target_include_directories(${TARGET_NAME} INTERFACE
        ../lib/CMSIS/Device/ST/STM32G4xx/Include
        ../lib/CMSIS/Include
        ../lib/CMSIS/Core/Include
    )
    
    # Add CMSIS system source file if it exists in the shared location
    # Note: system_stm32g4xx.c is typically board-specific and remains in Core/Src
    # but CMSIS headers are now shared
endfunction()

# Function to add shared CMSIS to any target
function(link_shared_cmsis TARGET_NAME MCU_FAMILY)
    if(MCU_FAMILY STREQUAL "STM32U5")
        create_shared_cmsis_stm32u5(shared_cmsis_${MCU_FAMILY})
        target_link_libraries(${TARGET_NAME} INTERFACE shared_cmsis_${MCU_FAMILY})
    elseif(MCU_FAMILY STREQUAL "STM32G4")
        create_shared_cmsis_stm32g4(shared_cmsis_${MCU_FAMILY})
        target_link_libraries(${TARGET_NAME} INTERFACE shared_cmsis_${MCU_FAMILY})
    else()
        message(WARNING "Unsupported MCU family: ${MCU_FAMILY}")
    endif()
endfunction()
