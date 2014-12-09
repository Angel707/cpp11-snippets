cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(CustomStdAllocator)

add_executable(CustomStdAllocator
	./custom_std_allocator.cpp
	./StaticMemoryAllocator/allocator.cpp
	./StaticMemoryAllocator/allocator.hpp
	)

add_executable(StaticMemoryAllocatorTest
	./StaticMemoryAllocator/test.cpp
	./StaticMemoryAllocator/allocator.cpp
	./StaticMemoryAllocator/allocator.hpp
	)

target_compile_options(CustomStdAllocator
	PUBLIC "-std=c++11"
	)

target_compile_options(StaticMemoryAllocatorTest
	PUBLIC "-std=c++11"
	)

target_include_directories(CustomStdAllocator
	PUBLIC ./StaticMemoryAllocator
	)

target_include_directories(StaticMemoryAllocatorTest
	PUBLIC ./StaticMemoryAllocator
	)

install(TARGETS CustomStdAllocator
	DESTINATION bin
	)
