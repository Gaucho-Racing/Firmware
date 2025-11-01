# Note: Depending on use this Makefile may be deprecated and removed at a later date
# If you are using VS Code with CMake Tools extension this Makefile is not needed at all

ifeq ($(OS), Windows_NT)
	rmd = rmdir /q /q
else
	rmd = rm -rf
endif

define cmake_build
	cmake --preset $(1)
	cmake --build build/$(1)
endef

.PHONY: Debug Release RelWithDebInfo MinSizeRel HOOTLTest test clean

all: Debug Release RelWithDebInfo MinSizeRel HOOTLTest

Debug:
	$(call cmake_build,$(@))

Release:
	$(call cmake_build,$(@))

RelWithDebInfo:
	$(call cmake_build,$(@))

MinSizeRel:
	$(call cmake_build,$(@))

HOOTLTest:
	$(call cmake_build,$(@))

test: HOOTLTest
	ctest --test-dir build/HOOTLTest

clean:
	$(rmd) build

memory_check: HOOTLTest
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s --error-exitcode=1 ./build/HOOTLTest/logomatic_simple
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s --error-exitcode=1 ./build/HOOTLTest/logomatic_float
