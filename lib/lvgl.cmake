# Add shared LVGL library to the root CMakeLists.txt
# LVGL is used by DashPanel
option(BUILD_SHARED_LVGL "Build shared LVGL library" ON)

if(BUILD_SHARED_LVGL)
    # LVGL settings for ARM cross-compilation
    set(LV_CONF_BUILD_DISABLE_DEMOS 1 CACHE BOOL "Disable LVGL demos" FORCE)
    set(LV_CONF_BUILD_DISABLE_EXAMPLES 1 CACHE BOOL "Disable LVGL examples" FORCE)
    
    # Add LVGL library
    add_subdirectory(lib/LVGL/lvgl)
    
    # Make LVGL available for other targets
    target_include_directories(lvgl PUBLIC
        lib/LVGL
    )
    
    message(STATUS "Built shared LVGL library for ARM targets")
endif()
