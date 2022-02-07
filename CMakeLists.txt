#CMake的最低版本要求
cmake_minimum_required(VERSION 3.0)
 
#项目名称
project(tingx)
 
#设置版本是 Debug 还是 Release
set(CMAKE_BUILD_TYPE "Debug")
 
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/**.cc ${PROJECT_SOURCE_DIR}/src/**.hpp ${PROJECT_SOURCE_DIR}/src/**.h) 

list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/tingx.cc)

#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src)

#设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
 
#添加编译参数,比如add_definitions(-DDEBUG)将在gcc命令行添加DEBUG宏定义
add_definitions(-g)
 
#添加可执行文件
add_executable(tingx ${SRC_FILES} ${PROJECT_SOURCE_DIR}/src/tingx.cc)
add_executable(client ${PROJECT_SOURCE_DIR}/test/client.cc)
add_executable(test ${PROJECT_SOURCE_DIR}/test/test.cc ${SRC_FILES})
add_executable(PtrTest ${PROJECT_SOURCE_DIR}/test/PtrTest.cc)