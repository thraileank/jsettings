cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(jsettings LANGUAGES CXX C)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

#sqlite
add_library(
	sqlite3
	STATIC 3rdparty/sqlite3/sqlite3.c
)

#jsettings
add_library(
	jsettings
	STATIC
	include/Table.hpp
)

target_include_directories(
	jsettings
	PRIVATE .
	PRIVATE 3rdparty/
)

target_link_libraries(
	jsettings
	PRIVATE Threads::Threads
	PRIVATE sqlite3
)

set_target_properties(jsettings PROPERTIES LINKER_LANGUAGE CXX)

#main
add_executable(
	main
	main.cpp
	)

target_include_directories(
	main
	PRIVATE .
	PRIVATE include/
	PRIVATE 3rdparty/
)

target_link_libraries(
	main
	PRIVATE jsettings
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
	PRIVATE jsettings
)

target_include_directories(
	tests
	PRIVATE .
	PRIVATE include/
	PRIVATE 3rdparty/
)
