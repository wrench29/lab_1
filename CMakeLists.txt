cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(program)

add_executable(program 
	${PROJECT_SOURCE_DIR}/src/main.cpp 
	${PROJECT_SOURCE_DIR}/src/scene.cpp
	${PROJECT_SOURCE_DIR}/src/utils.cpp
	${PROJECT_SOURCE_DIR}/src/aggregates.cpp)

target_link_libraries(program sfml-graphics sfml-window sfml-system pthread)
