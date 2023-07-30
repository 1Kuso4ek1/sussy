cmake_minimum_required(VERSION 3.17)

project(sussy VERSION 0.0.0 LANGUAGES CXX)

add_executable(sussy main.cpp src/Lexer.cpp src/AST.cpp src/Variable.cpp)

set_target_properties(sussy PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(sussy
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
