ifeq ($(OS), Windows_NT)
	rmd = rmdir /q /q
else
	rmd = rm -rf
endif

define cmake_build
	cmake --preset $(1)
	cmake --build build/$(1)
endef

.PHONY: clean Debug Release RelWithDebInfo MinSizeRel HOOTLTest sanitizer valgrind

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

sanitizer: HOOTLTest
	ctest --test-dir build/HOOTLTest

clean:
	$(rmd) build

valgrind:
	cmake -S . -B build/HOOTLValgrind --preset HOOTLTest -DADDRESS_SANITIZER=OFF
	cmake --build build/HOOTLValgrind
	for test in $$(find build/HOOTLValgrind -maxdepth 1 -type f -executable); do \
		echo ""; \
		echo "Running memory check on '$$test'"; \
		valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s --error-exitcode=1 "$$test"; \
		echo ""; \
	done
