cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(intelli)

# Set C++ standards
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default dir and source files
set(LIBRARY /usr/local/lib)
set(INCLUDE /usr/local/include)
set(INCLUDE_LCD /usr/local/include/lcdgfx)
set(SRC_LIST src/mapper.cpp executable/main.cpp src/display.cpp)

# Link directories
link_directories(${LIBRARY})
include_directories(${CMAKE_SOURCE_DIR}/include ${INCLUDE} ${INCLUDE_LCD})

# Build executable
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE robomaster lcdgfx sense xcrsf)