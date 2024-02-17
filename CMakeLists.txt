cmake_minimum_required(VERSION 3.14)
project(GGBoy
        VERSION 0.1.0 
        LANGUAGES CXX)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE MAIN_FILES CONFIGURE_DEPENDS "src/*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

add_executable(main
    ${MAIN_FILES}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})
target_link_libraries(main ${SDL2_LIBRARIES})