cmake_minimum_required (VERSION 3.1)
project (fefisms)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/verbly)

include_directories(
  vendor/libtwittercpp/src
  vendor/verbly/lib
  ${yaml-cpp_INCLUDE_DIRS})

add_executable(fefisms fefisms.cpp main.cpp)
set_property(TARGET fefisms PROPERTY CXX_STANDARD 11)
set_property(TARGET fefisms PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(fefisms verbly twitter++ ${yaml-cpp_LIBRARIES})
