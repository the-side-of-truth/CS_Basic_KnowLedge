# CMakeList.txt: WanhuaTong 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)

project ("WanhuaTong")

set(LLVM_DIR "/root/autodl-tmp/Install/LLVM") # 改成自己的
find_package(LLVM REQUIRED CONFIG)
if(NOT LLVM_INCLUDE_DIRS)
    message(STATUS "LLVM_DIR: ${LLVM_DIR}")
    message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
    message(FATAL_ERROR "无法找到LLVM相关文件")
endif()
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "使用LLVM: ${LLVM_INCLUDE_DIRS}")

# 将源代码添加到此项目的可执行文件。
AUX_SOURCE_DIRECTORY(. src) # 添加当前目录下的所有文件
ADD_EXECUTABLE(main ${src})

# TODO: 如有需要，请添加测试并安装目标。
