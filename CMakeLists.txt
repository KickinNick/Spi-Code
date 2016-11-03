project(spi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

add_executable(spi
  SpiConnect.cpp
  main.cpp)

