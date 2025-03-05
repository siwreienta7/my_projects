cmake_minimum_required(VERSION 3.22)

project(tic-tac-toe CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)  # Recommended by Boost.DLL's manual

find_package(Boost 1.71 REQUIRED filesystem)  # Required by Boost.DLL
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)

add_executable(tic-tac-toe src/main.cpp src/game.cpp)
target_link_libraries(tic-tac-toe ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})

add_library(view-compact SHARED src/view_compact.cpp src/game.cpp)
add_library(view-compactzero SHARED src/view_compact.cpp src/game.cpp)
target_compile_definitions(view-compactzero PRIVATE VIEW_COMPACT_USE_ZERO)  # -DVIEW_COMPACT_USE_ZERO for compiler for `view-compactzero` only
add_library(view-pretty SHARED src/view_pretty.cpp src/game.cpp)
