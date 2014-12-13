#include "StaticMemoryAllocator/allocator.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <cstdint>
#include <new>
#include <thread>

template <typename T>
using MyStaticMemoryAllocator = StaticMemoryAllocator::allocator<T>;

template <typename T>
using MyStaticMemoryVector = std::vector<T, MyStaticMemoryAllocator<T>>;

typedef uint8_t                               stype;
typedef std::tuple<uint8_t, uint8_t, uint8_t> ttype;

#include "StaticMemoryAllocator/allocator_impl.hpp"
template class StaticMemoryAllocator::allocator<stype>;
template class StaticMemoryAllocator::allocator<ttype>;

void wait(void)
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << std::endl;
}

void print_vec(const MyStaticMemoryVector<stype> & vec_uncompressed, const std::string & header, const std::string & footer = "")
{
	std::cout << header << "(" << vec_uncompressed.size() << "/" << vec_uncompressed.capacity() << "): ";
	if (vec_uncompressed.empty()) {
		std::cout << "EMPTY ";
	} else {
		for (const auto i : vec_uncompressed) {
			std::cout << "(" << static_cast<int>(i) << ") ";
		}
	}
	if (footer.empty()) {
		std::cout << "." << std::endl;
	} else {
		std::cout << "- " << footer << "." << std::endl;
	}
}

void print_vec(const MyStaticMemoryVector<ttype> & vec_compressed, const std::string & header, const std::string & footer = "")
{
	std::cout << header << "(" << vec_compressed.size() << "/" << vec_compressed.capacity() << "): ";
	if (vec_compressed.empty()) {
		std::cout << "EMPTY ";
	} else {
		for (const auto i : vec_compressed) {
			std::cout << "(" << static_cast<int>(std::get<0>(i))
				  << "," << static_cast<int>(std::get<1>(i))
				  << "," << static_cast<int>(std::get<2>(i))
				  << ") ";
		}
	}
	if (footer.empty()) {
		std::cout << "." << std::endl;
	} else {
		std::cout << "- " << footer << "." << std::endl;
	}
}

int test_memory_management(void *const start1, void *const start2, const size_t memsize)
{
	/**
	 * Idea:
	 * Two or more std-containers are able to use the same memory block.
	 * In this example, a uncompressed vector of type number
	 * and a compressed vector of type tuple of three numbers
	 * sharing the same memory block, starting at \p start1 and of size \p size.
	 */
	std::cout << "--> " << "construct allocators.." << std::endl;
	MyStaticMemoryAllocator<stype> sma_s1(start1, memsize, "mem1");
	MyStaticMemoryAllocator<ttype> sma_t1(sma_s1);
	MyStaticMemoryAllocator<ttype> sma_t2(start2, memsize, "mem2");
	wait();

	/*
	 * This excample demonstrates a cooperating memory management,
	 * that means, that the allocator of the uncompressed vector
	 * do NOT know anything about the allocator of the compressed vector.
	 * Thus, the programmer has to take care, when
	 * the uncompressed vector is used at runtime,
	 * that the compressed vector is not used and vise versa.
	 * But the advantage of this is, that an object can use both methods
	 * to access his meory block, e.g. by using range-based loops
	 * (i.e., no need to access the memory by C-arrays).
	 */
	std::cout << "--> " << "construct vectors using these allocators.." << std::endl;
	std::cout << "uncompressed vector 1 (uvec1) uses [start1, end1]." << std::endl;
	MyStaticMemoryVector<stype> uvec1(sma_s1);
	assert(uvec1.capacity() == 0);
	assert(uvec1.size() == 0);
	std::cout << "compressed vector 1 (cvec1) uses [start1, end1]." << std::endl;
	MyStaticMemoryVector<ttype> cvec1(sma_t1);
	assert(cvec1.capacity() == 0);
	assert(cvec1.size() == 0);
	std::cout << "compressed vector 2 (cvec2) uses [start2, end2]." << std::endl;
	MyStaticMemoryVector<ttype> cvec2(sma_t2);
	assert(cvec2.capacity() == 0);
	assert(cvec2.size() == 0);
	wait();

	std::cout << "--> " << "print all three vectors:" << std::endl;
	print_vec(uvec1, "uvec1");
	print_vec(cvec1, "cvec1");
	print_vec(cvec2, "cvec2");
	wait();

	std::cout << "--> " << "reserve memory for uvec1.." << std::endl;
	const size_t uvec1_memsize1 = 9;
	uvec1.reserve(uvec1_memsize1);
	assert(uvec1.capacity() == uvec1_memsize1);
	assert(uvec1.size() == 0);
	wait();

	std::cout << "--> " << "try to change reserved memory for uvec1.. but there is not enough memory" << std::endl;
	try {
		/* this call should raise a bad_alloc exception! */
		uvec1.reserve(memsize - uvec1_memsize1 + 1);
	} catch (const std::bad_alloc& ba) {
		/* this is OK */
		std::cout << "bad alloc caught: " << ba.what() << std::endl;
		std::cout << "problem: " << "the reserve() function tries to allocate the new memory block firstly, "
			  << "then frees the old one, but that is the problem." << std::endl;		  
	}
	assert(uvec1.capacity() == uvec1_memsize1);
	assert(uvec1.size() == 0);
	wait();
	
	std::cout << "--> " << "add some numbers to uvec1.." << std::endl;
	const size_t uvec1_added1 = 8;
	assert(uvec1_added1 <= uvec1.capacity());
	uvec1.push_back(1);
	uvec1.push_back(2);
	uvec1.push_back(3);
	uvec1.push_back(4);
	uvec1.push_back(5);
	uvec1.push_back(6);
	uvec1.push_back(7);
	uvec1.push_back(8);
	print_vec(uvec1, "uvec1", "after added eight numbers");
	assert(uvec1.capacity() == uvec1_memsize1);
	assert(uvec1.size() == uvec1_added1);
	wait();

	std::cout << "--> " << "reserve memory for cvec1.." << std::endl;
	const size_t cvec1_memsize1 = uvec1_memsize1 / 3;
	cvec1.reserve(cvec1_memsize1);
	assert(cvec1.capacity() == cvec1_memsize1);
	assert(cvec1.size() == 0);
	assert(uvec1.size() == uvec1_added1);
	assert(uvec1.capacity() == uvec1_memsize1);
	wait();

	std::cout << "--> " << "fill cvec1.." << std::endl;
	std::cout << "the compressed and uncompressed vector 1 shares the same memory block (the first 9 bytes)" << std::endl
		  << "thus, adding items to both effects the other vector.. but see yourself.." << std::endl;
	const size_t cvec1_added1 = 2;
	assert(cvec1_added1 <= cvec1.capacity());
	cvec1.push_back(std::make_tuple(11, 12, 13));
	cvec1.push_back(std::make_tuple(14, 15, 16));
	assert(cvec1.size() == cvec1_added1);
	print_vec(cvec1, "cvec1", "after adding two tuples of three numbers to cvec1");
	assert(uvec1.size() == uvec1_added1);
	print_vec(uvec1, "uvec1", "after adding tuples to cvec1 (i.e., not this vector)");
	wait();

	std::cout << "--> " << "try to change reserved memory for uvec1.." << std::endl;
	const size_t uvec1_memsize2 = memsize - uvec1_memsize1;
	try {
		/* this call should raise a bad_alloc exception! */
		uvec1.reserve(uvec1_memsize2);
	} catch (const std::bad_alloc& ba) {
		/* this is OK */
		std::cout << "bad alloc caught: " << ba.what() << std::endl;
		std::cout << "problem: " << "there is not enough memory available to allocate the block, "
			  << "cvec1 has also allocated memory in the same static memory block, beginning at start1." << std::endl;
	}
	assert(uvec1.capacity() == uvec1_memsize1);
	assert(uvec1.size() == uvec1_added1);
	print_vec(uvec1, "uvec1", "after triing to change reserved size");
	assert(cvec1.capacity() == cvec1_memsize1);
	assert(cvec1.size() == cvec1_added1);
	wait();

	std::cout << "--> " << "try to change reserved memory for uvec1 (second run).." << std::endl;
	const size_t uvec1_memsize3 = memsize - uvec1_memsize1 - cvec1_memsize1 * sizeof(ttype);
	uvec1.reserve(uvec1_memsize3);
	assert(uvec1.capacity() == uvec1_memsize3);
	assert(uvec1.size() == uvec1_added1);
	print_vec(uvec1, "uvec1", "after changing reserved size");
	assert(cvec1.capacity() == cvec1_memsize1);
	assert(cvec1.size() == cvec1_added1);
	wait();

	std::cout << "--> " << "fill uvec1 with more numbers.." << std::endl;
	const size_t uvec1_added2 = uvec1_added1 + 4;
	assert(uvec1_added2 <= uvec1.capacity());
	uvec1.push_back(21);
	uvec1.push_back(22);
	uvec1.push_back(23);
	uvec1.push_back(24);
	assert(uvec1.size() == uvec1_added2);
	print_vec(uvec1, "uvec1", "after adding four numbers");
	print_vec(cvec1, "cvec1", "after adding numbers to uvec1 (i.e., not this vector)");
	wait();

	std::cout << "--> " << "fill cvec1 with one more tuples of numbers.." << std::endl;
	const size_t cvec1_added2 = cvec1_added1 + 1;
	assert(cvec1_added2 <= cvec1.capacity());
	cvec1.push_back(std::make_tuple(31, 32, 33));
	assert(cvec1.size() == cvec1_added2);
	print_vec(cvec1, "cvec1", "after adding one tuple");
	print_vec(uvec1, "uvec1", "after adding one tuple to cvec1");
	wait();

	std::cout << "--> " << "reserve memory for cvec2.." << std::endl;
	const size_t cvec2_memsize1 = memsize / 3;
	cvec2.reserve(cvec2_memsize1);
	print_vec(cvec2, "cvec2", "after triing to reserve memory");
	print_vec(cvec1, "cvec1");
	wait();

	std::cout << "--> " << "copy cvec1 to cvec2.." << std::endl;
	cvec2 = cvec1;
	print_vec(cvec2, "cvec2", "after copying cvec1 to cvec2");
	print_vec(cvec1, "cvec1");
	wait();

	std::cout << "." << std::endl;
	return 0;
}


/**
 * main function.
 */
int main(int, char **)
{
	int ret = 0;
	std::cout << "memory allocation.." << std::endl;
	const size_t memsize = 30; // bytes
	std::vector<uint8_t> memvector1(memsize, 0);
	assert(memvector1.capacity() == memsize);
	assert(memvector1.size() == memsize);
	std::vector<uint8_t> memvector2(memsize, 0);
	assert(memvector2.capacity() == memsize);
	assert(memvector2.size() == memsize);

	try {
		std::cout << "test memory management.." << std::endl << std::endl;
		ret = test_memory_management(memvector1.data(), memvector2.data(), memsize);
		std::cout << std::endl << "end of test memory management: successfully." << std::endl;
	} catch (const std::bad_alloc& ba) {
		std::cout << std::endl << "end of test memory management: error." << std::endl;
		std::cerr << "bad alloc caught: " << ba.what() << std::endl;
		ret = 1;
	} catch(std::exception& e) {
		std::cout << std::endl << "end of test memory management: error." << std::endl;
		std::cerr << "exception caught: " << e.what() << std::endl;
		ret = 1;
	}

	return ret;
}

