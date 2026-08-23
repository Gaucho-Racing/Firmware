target_sources(
	${TARGET_NAME}
	PRIVATE
		Application/Src/CANdler.c
		Application/Src/Loop.c
		Core/Src/fdcan.c
		Core/Src/gpio.c
		Core/Src/main.c
		Core/Src/stm32g4xx_it.c
		Core/Src/tim.c
		Core/Src/usart.c
)

target_link_libraries(
	${TARGET_NAME}
	PRIVATE
		CUBE_CAN_LIB
		CUBE_VCP_LIB
		LOGOMATIC_LIB
)

target_include_directories(
	${TARGET_NAME}
	PRIVATE
		Core/Inc
		Application/Inc
)
