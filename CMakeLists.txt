#CMake的最低版本要求
cmake_minimum_required(VERSION 3.0)
 
#项目名称
PROJECT (tingx)
 
#设置版本是 Debug 还是 Release
SET(CMAKE_BUILD_TYPE "Debug")
 
#设置SRC_FILE的值,指示源代码文件夹
SET(SRC_FILE src/)
 
#设置包含目录
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

#设置输出目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
 
#添加编译参数,比如add_definitions(-DDEBUG)将在gcc命令行添加DEBUG宏定义
add_definitions(-g)
 
#添加可执行文件
ADD_EXECUTABLE(tingx ${SRC_FILE}/tingx.cc)
ADD_EXECUTABLE(client ${SRC_FILE}/test/client.cc)