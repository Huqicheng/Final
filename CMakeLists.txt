cmake_minimum_required(VERSION 2.8)
project(ece650-a4)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(helper)

# create the main executable
## add additional .cpp files if needed
add_executable(main reader_writer.cpp graph.cpp command.cpp  )
# link MiniSAT libraries
target_link_libraries(main minisat-lib-static)
target_link_libraries(main helper ${CMAKE_THREAD_LIBS_INIT})

