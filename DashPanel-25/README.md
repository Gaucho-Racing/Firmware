# Dash Panel (v2)
LVGL (Light and Versatile Graphics Library) for the 5-inch Riverdi STM32 Embedded Displays

## Git Submodule
To get the submodule added correctly, please run:
```
git submodule init
git submodule update --remote
cmake --preset Debug
cd build/Debug
ninja
```

## Install
See [ECU-25](https://github.com/Gaucho-Racing/ECU-25) for a likely install chain.

## Origin
Forked from [Riverdi](https://github.com/riverdi/riverdi-50-stm32u5-lvgl) which uses lots of code from [LVGL](https://github.com/lvgl/lvgl) `v9.3.0-dev`

Borrows heavily from [`ECU-25`](https://github.com/Gaucho-Racing/ECU-25) (internal)
