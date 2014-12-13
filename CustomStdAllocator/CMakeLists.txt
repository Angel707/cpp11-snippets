cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(CustomStdAllocator)

find_package(Boost 1.52 REQUIRED)

add_executable(shared_static_memory
	./shared_static_memory.cpp
	./StaticMemoryAllocator/allocator.cpp
	./StaticMemoryAllocator/allocator_impl.hpp
	./StaticMemoryAllocator/allocator.hpp
	)

add_executable(static_memory_management
	./static_memory_management.cpp
	./StaticMemoryAllocator/allocator.cpp
	./StaticMemoryAllocator/allocator_impl.hpp
	./StaticMemoryAllocator/allocator.hpp
	)

target_compile_options(shared_static_memory
	PUBLIC "-std=c++11"
	)

target_compile_options(static_memory_management
	PUBLIC "-std=c++11"
	)

target_include_directories(shared_static_memory
	PUBLIC ./StaticMemoryAllocator
	)

target_include_directories(static_memory_management
	PUBLIC ./StaticMemoryAllocator
	)

install(TARGETS shared_static_memory
	DESTINATION bin
	)

install(TARGETS static_memory_management
	DESTINATION bin
	)

