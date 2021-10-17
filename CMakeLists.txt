cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(jsettings LANGUAGES CXX C)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_library(
	sqlite3
	STATIC
	3rdparty/sqlite3/sqlite3.c
)

add_executable(
	jsettings
	main.cpp
	)

target_link_libraries(
	jsettings
	PRIVATE Threads::Threads
	PRIVATE sqlite3
#	${CMAKE_DL_LIBS}
)

# Tests
add_executable(
	tests
	test.cpp
	test/ParamTest.cpp
	test/TableTest.cpp
)

target_link_libraries(
	tests
	PRIVATE sqlite3
)

target_include_directories(
	tests
	PRIVATE 3rdparty
)
