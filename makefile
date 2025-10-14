builddir := Debug/build

#rm rf macros
ifeq ($(OS),Windows_NT)
	rm = del /f /q
	rmd = -rmdir /q /q
	mkd = -mkdir
else
	rm = rm -rf
	rmd = rm -rf
	mkd = mkdir -p
endif

all:
	cmake -S . -B $(builddir) -G Ninja --preset=Debug
	cd $(builddir) && ninja

clean:
	$(rmd) $(builddir)
.PHONY: all
