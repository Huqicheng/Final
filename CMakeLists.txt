cmake_minimum_required(VERSION 2.8)
project(ece650-threads)

find_package (Threads)
## ECE650 Specific configuration start here
## Project specific configurations go here
if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()
# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/minisat)

add_subdirectory(helper)

add_executable(input reader_wirter.cpp graph.cpp command.cpp)
# link minisat library with the main executable
target_link_libraries(input minisat-lib-static)
target_link_libraries(input helper ${CMAKE_THREAD_LIBS_INIT})