SET(ToolPlatform)

# 检查指针大小
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Building for Win32")
    SET(ToolPlatform "win32")
    # 添加 Win32 特定的设置
    add_compile_definitions(WIN32)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Building for x64")
    SET(ToolPlatform "x64")
    # 添加 x64 特定的设置
    add_compile_definitions(_WIN64)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
