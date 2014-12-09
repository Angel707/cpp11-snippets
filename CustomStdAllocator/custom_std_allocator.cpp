#include "StaticMemoryAllocator/allocator.hpp"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <cstdint>

template <typename T = uint8_t>
using MyStaticMemoryAllocator = StaticMemoryAllocator::allocator<T>;

template <typename T>
using MyStaticMemoryVector = std::vector<T, MyStaticMemoryAllocator<T>>;

typedef int                       stype;
typedef std::tuple<int, int, int> ttype;
typedef size_t                    mtype;

void print_vec(const MyStaticMemoryVector<stype> & vec_uncompressed, const std::string & header)
{
	std::cout << header << ": ";
	for (const auto & i : vec_uncompressed) {
		std::cout << "(" << i << ") ";
	}
	std::cout << "." << std::endl;
}

void print_vec(const MyStaticMemoryVector<ttype> & vec_compressed, const std::string & header)
{
	std::cout << header << ": ";
	for (const auto & i : vec_compressed) {
		std::cout << "(" << std::get<0>(i) << "," << std::get<1>(i) << "," << std::get<2>(i) << ") ";
	}
	std::cout << "." << std::endl;
}

void *start1 = nullptr;
void *start2 = nullptr;

/**
 * main function.
 */
int main(int, char **)
{
	std::cout << "memory allocation.." << std::endl;
	const mtype size1 = 20;
	const mtype size2 = 20;
	start1 = malloc(size1 * sizeof(mtype));
	start2 = malloc(size2 * sizeof(mtype));

	try {
		std::cout << "construct allocators.." << std::endl;
		MyStaticMemoryAllocator<> myalloc1(start1, size1);
		MyStaticMemoryAllocator<> myalloc2(start2, size2);

		std::cout << "construct vectors using these allocators.." << std::endl;
		MyStaticMemoryVector<stype> vec1_uncompressed(myalloc1);
		MyStaticMemoryVector<ttype> vec1_compressed(myalloc1);
		MyStaticMemoryVector<ttype> vec2_compressed(myalloc2);

		std::cout << "reserve enough memory.." << std::endl;
		vec1_uncompressed.reserve(10);

		std::cout << "fill uncompressed vector.." << std::endl;
		vec1_uncompressed.push_back(1);
		vec1_uncompressed.push_back(2);
		vec1_uncompressed.push_back(3);
		vec1_uncompressed.push_back(4);
		print_vec(vec1_uncompressed, "vec1 uncompressed");
		print_vec(vec1_compressed,   "vec1 compressed");

		std::cout << "fill compressed vector.." << std::endl;
		vec1_compressed.push_back(std::make_tuple(11, 12, 13));
		print_vec(vec1_uncompressed, "vec1 uncompressed");
		print_vec(vec1_compressed,   "vec1 compressed");
		
		std::cout << "fill compressed vector.." << std::endl;
		vec2_compressed.push_back(std::make_tuple(21, 22, 23));
		print_vec(vec2_compressed,   "vec2 compressed");

	} catch(...) {
		std::cout << "exception raised." << std::endl;
	}

	std::cout << "memory freeing.." << std::endl;
	if (start1 != nullptr) {
		free(start1);
		start1 = nullptr;
	}
	if (start2 != nullptr) {
		free(start2);
		start2 = nullptr;
	}
	std::cout << std::endl;
	return 0;
}

