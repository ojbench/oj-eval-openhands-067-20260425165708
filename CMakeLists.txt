cmake_minimum_required(VERSION 3.10)
project(RefCell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(code main.cpp)

# Set compiler flags
target_compile_options(code PRIVATE -Wall -Wextra -O2)
