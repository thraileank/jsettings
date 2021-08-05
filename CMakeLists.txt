cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(jsettings LANGUAGES CXX C)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
add_executable(
	jsettings
	main.cpp
	3rdparty/sqlite3/sqlite3.c
	)

add_executable(
	tests
	test.cpp
	test/ParamTest.cpp
	test/TableTest.cpp
	3rdparty/sqlite3/sqlite3.c
	)

target_link_libraries(
	jsettings
	PRIVATE Threads::Threads
	${CMAKE_DL_LIBS}
)

find_package(Catch2 REQUIRED)
target_link_libraries(tests PRIVATE Catch2::Catch2)
