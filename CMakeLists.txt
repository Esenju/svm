cmake_minimum_required(VERSION 3.20)
project(svm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer SYSTEMC_HOME like your working g++ flow
if(NOT DEFINED ENV{SYSTEMC_HOME})
  message(FATAL_ERROR "SYSTEMC_HOME is not set. Example: export SYSTEMC_HOME=/usr/local/systemc-3.0.2")
endif()

set(SYSTEMC_HOME $ENV{SYSTEMC_HOME})
message(STATUS "Using SYSTEMC_HOME = ${SYSTEMC_HOME}")

# SystemC include + lib paths (common layout)
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64 ${SYSTEMC_HOME}/lib)

add_executable(uvm_lite_sim
  sim/main.cpp
  sim/tb_top.cpp
  soc/soc_top.cpp
  vkit/sequencer.cpp

  # add your IP sources here if they have .cpp files
  # soc/ip/spi.cpp
  # soc/ip/axi_dma.cpp
  # soc/ip/timer.cpp
  # add your agent sources here if split into .cpp
  # agents/uart_agent.cpp ...
)

target_include_directories(uvm_lite_sim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link SystemC (and math, pthread if needed)
target_link_libraries(uvm_lite_sim PRIVATE systemc m pthread)
