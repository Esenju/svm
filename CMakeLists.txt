# CMakeLists.txt (top-level)
cmake_minimum_required(VERSION 3.20)
project(uvm_lite LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(SystemCLanguages REQUIRED) # or find_package(SystemC REQUIRED) depending on install

add_executable(uvm_lite_sim
  sim/main.cpp
  sim/tb_top.cpp
  soc/soc_top.cpp
  # + IP + agents + vkit sources...
)

target_include_directories(uvm_lite_sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(uvm_lite_sim PRIVATE SystemC::systemc)
