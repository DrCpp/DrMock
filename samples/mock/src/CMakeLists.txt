cmake_minimum_required(VERSION 3.10)

add_library(DrMockSampleMock SHARED
  Order.cpp
  Warehouse.cpp
)

# Get the path to the mocker script.
file(TO_CMAKE_PATH 
  ${CMAKE_SOURCE_DIR}/../../python/DrMockGenerator 
  pathToMocker
)
DrMockModule(
  TARGET DrMockSampleMockMocked
  GENERATOR ${pathToMocker}
  HEADERS
    IWarehouse.h
)

