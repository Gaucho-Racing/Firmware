add_executable(valid-morse)

target_include_directories(valid-morse PRIVATE
    Inc
    ../Application/Inc
)

target_sources(valid-morse PRIVATE
    Src/valid-morse.c
    ../Application/Src/morse.c
)

target_link_libraries(valid-morse PRIVATE LOGOMATIC_LIB)

add_test(NucleoG4-Blinky-Test-ValidMorse
    valid-morse)
