set(CMAKE_CXX_STANDARD          17)
cmake_minimum_required(VERSION  3.22.0)
project(ex_server_client        VERSION 0.1.0 LANGUAGES C CXX)


add_executable(ex_server_client main.cpp)


add_subdirectory(src)


include(CTest)
enable_testing()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
