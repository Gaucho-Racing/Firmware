add_library(CANfigurator INTERFACE)
target_include_directories(CANfigurator INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	add_executable(Autogen_CAN_Bus_Compilation_Test)
	target_sources(
		Autogen_CAN_Bus_Compilation_Test
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/autogen_can_bus_compilation_test.c
	)
	target_link_libraries(
		Autogen_CAN_Bus_Compilation_Test
		PRIVATE
			CANfigurator
	)
	add_test(Autogen_CAN_Bus_Compilation_Test Autogen_CAN_Bus_Compilation_Test)

	add_executable(Autogen_CAN_Custom_Compilation_Test)
	target_sources(
		Autogen_CAN_Custom_Compilation_Test
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/autogen_can_custom_compilation_test.c
	)
	target_link_libraries(
		Autogen_CAN_Custom_Compilation_Test
		PRIVATE
			CANfigurator
	)
	add_test(Autogen_CAN_Custom_Compilation_Test Autogen_CAN_Custom_Compilation_Test)

	add_executable(Autogen_CAN_Msg_Data_Compilation_Test)
	target_sources(
		Autogen_CAN_Msg_Data_Compilation_Test
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/autogen_can_msg_data_compilation_test.c
	)
	target_link_libraries(
		Autogen_CAN_Msg_Data_Compilation_Test
		PRIVATE
			CANfigurator
	)
	add_test(Autogen_CAN_Msg_Data_Compilation_Test Autogen_CAN_Msg_Data_Compilation_Test)

	add_executable(Autogen_CAN_Msg_ID_Compilation_Test)
	target_sources(
		Autogen_CAN_Msg_ID_Compilation_Test
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/autogen_can_msg_id_compilation_test.c
	)
	target_link_libraries(
		Autogen_CAN_Msg_ID_Compilation_Test
		PRIVATE
			CANfigurator
	)
	add_test(Autogen_CAN_Msg_ID_Compilation_Test Autogen_CAN_Msg_ID_Compilation_Test)

	add_executable(Autogen_CAN_Node_ID_Compilation_Test)
	target_sources(
		Autogen_CAN_Node_ID_Compilation_Test
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/autogen_can_node_id_compilation_test.c
	)
	target_link_libraries(
		Autogen_CAN_Node_ID_Compilation_Test
		PRIVATE
			CANfigurator
	)
	add_test(Autogen_CAN_Node_ID_Compilation_Test Autogen_CAN_Node_ID_Compilation_Test)

	# TODO Add more tests
endif()
