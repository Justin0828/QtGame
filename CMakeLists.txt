cmake_minimum_required(VERSION 3.16)
project(QtGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
)

file(GLOB_RECURSE HEADERS 
    "include/*.h"
)

# Create executable
add_executable(QtGame ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(QtGame Qt6::Core Qt6::Widgets)

# Set output directory
set_target_properties(QtGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 