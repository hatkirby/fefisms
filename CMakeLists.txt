cmake_minimum_required (VERSION 3.1)
project (fefisms)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/verbly)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

include_directories(vendor/libtwittercpp/src vendor/verbly/lib vendor/yaml-cpp/include)
add_executable(fefisms fefisms.cpp main.cpp)
set_property(TARGET fefisms PROPERTY CXX_STANDARD 11)
set_property(TARGET fefisms PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(fefisms verbly twitter++ yaml-cpp)
