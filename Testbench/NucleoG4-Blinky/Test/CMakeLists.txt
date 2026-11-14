add_executable(valid-morse)

target_include_directories(valid-morse PRIVATE
	Inc
	Stub/Inc
	../Application/Inc
	../Core/Inc
)

target_sources(valid-morse PRIVATE
	valid-morse.c
	Stub/hal.c
)
