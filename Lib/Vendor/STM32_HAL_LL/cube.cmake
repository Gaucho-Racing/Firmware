include("${CMAKE_CURRENT_LIST_DIR}/Common/stm32common.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/STM32G4xx/stm32g4xx.cmake")
target_link_libraries(STM32_HAL_LL_G4xx INTERFACE STM32_Cube_Common)

include("${CMAKE_CURRENT_LIST_DIR}/STM32U5xx/stm32u5xx.cmake")
target_link_libraries(STM32_HAL_LL_U5xx INTERFACE STM32_Cube_Common)

include("${CMAKE_CURRENT_LIST_DIR}/STM32L4xx/stm32l4xx.cmake")
target_link_libraries(STM32_HAL_LL_L4xx INTERFACE STM32_Cube_Common)

# Add other STM32 series as needed by including and linking them similarly to above
