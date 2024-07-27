cmake_minimum_required(VERSION 3.17)

project(sussy VERSION 0.0.0 LANGUAGES CXX)

add_executable(sussy main.cpp src/Lexer.cpp src/AST.cpp src/Variable.cpp src/Exec.cpp src/Structure.cpp)

target_link_libraries(sussy stdc++fs)

set_target_properties(sussy PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

target_include_directories(sussy
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
	TARGETS sussy
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)
