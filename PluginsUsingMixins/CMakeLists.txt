cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(PluginsUsingMixins)

#find_package(Boost 1.52 REQUIRED)

add_executable(plugin0x
	./main.cpp
	)

target_compile_options(plugin0x
	PUBLIC "-std=c++0x"
	#PUBLIC "-std=c++11"
	)

target_include_directories(plugin0x
	PRIVATE ${Boost_INCLUDE_DIR}
	)

target_link_libraries(plugin0x
	${Boost_LIBRARIES}
	)

