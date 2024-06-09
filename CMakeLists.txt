cmake_minimum_required(VERSION 3.10.0)

project(smart)

set(Tartget smart)

message(STATUS "******************************************************************")
message(STATUS "CMAKE_SOURCE_DIR->" ${CMAKE_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR->" ${PROJECT_SOURCE_DIR})
message(STATUS "******************************************************************")

set(CMAKE_CXX_STANDARD 14)

#编译详情
set(CMAKE_VERBOSE_MAKEFILE ON)

#禁止编译警告：warning C4819：该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失
add_compile_options(/wd4819)
add_compile_options(/wd4200)  #禁止C4200警告
add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)  # 禁止C4996警告

#依赖库信息
set(APP_DEPEND_LIBS)
include(${PROJECT_SOURCE_DIR}/playsdk/Linklib.cmake)

#头文件路径
set(APP_INCLUDE_DIRS)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/common/infra)
include_directories(${PROJECT_SOURCE_DIR}/common/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/common/thirdparty/msgpack)
include_directories(${PROJECT_SOURCE_DIR}/common/thirdparty/freetype-2.13.0/include)
include_directories(${PROJECT_SOURCE_DIR}/common/common/mediafiles)
include_directories(${PROJECT_SOURCE_DIR}/common/common/mediafiles/common/libmov/include)
include_directories(${PROJECT_SOURCE_DIR}/common/middleware)
include_directories(${PROJECT_SOURCE_DIR}/applicaton)
include_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/ffmpeg/include)
include_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/glad/include)
include_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty)

#源文件
set(APP_SOURCE_FILES)
set(APP_SOURCE_HEADER_FILES)

add_subdirectory(common/infra)
add_subdirectory(common/common)
add_subdirectory(common/thirdparty)
add_subdirectory(common/middleware)
add_subdirectory(playsdk)

# for freetype
add_compile_definitions(DARWIN_NO_CARBON)
add_compile_definitions(FT2_BUILD_LIBRARY)

file(GLOB_RECURSE APPLICATION_SOURCE_FILES application/*.cpp mediasource/*.cpp)
file(GLOB_RECURSE APPLICATION_SOURCE_HEADER_FILES application/*.h mediasource/*.h)

set(APP_SOURCE_FILES ${APP_SOURCE_FILES} ${APPLICATION_SOURCE_FILES})
set(APP_SOURCE_HEADER_FILES ${APP_SOURCE_HEADER_FILES} ${APPLICATION_SOURCE_HEADER_FILES})

function(set_file_group prefix)
    message(STATUS "set_file_group " ${prefix} " " ${ARGC})
    foreach(FILE IN LISTS ARGN 1)
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
        # Remove common directory prefix to make the group
        string(REPLACE "${prefix}" "" GROUP "${PARENT_DIR}")
        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()

set_file_group(${CMAKE_SOURCE_DIR} ${APP_SOURCE_FILES} ${APP_SOURCE_HEADER_FILES})


#指定头文件路径
include_directories(${APP_INCLUDE_DIRS})
add_executable(${Tartget} ${APP_SOURCE_FILES} ${APP_SOURCE_HEADER_FILES})
#添加依赖库
target_link_libraries(${Tartget} ${APP_DEPEND_LIBS})

#输出目录
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../Build/${Platform})
