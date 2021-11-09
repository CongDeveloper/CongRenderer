# CMakeList.txt: CongRenderer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required (VERSION 3.8)

project ("CongRenderer")

# 将源代码添加到此项目的可执行文件。
add_executable (CongRenderer "CongRenderer.cpp" "CongRenderer.h"  "tgaimage.h"  "tgaimage.cpp" "Model.h" "Model.cpp"    "MathUtil.h" "MathUtil.cpp"  "GLUtil.hpp")

# TODO: 如有需要，请添加测试并安装目标。
