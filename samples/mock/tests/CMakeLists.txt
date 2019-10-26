cmake_minimum_required(VERSION 3.10)

include_directories(../src)

DrMockTest(
  LIBS
    DrMockSampleMock
    DrMockSampleMockMocked
  TESTS
    OrderTest.cpp
)
