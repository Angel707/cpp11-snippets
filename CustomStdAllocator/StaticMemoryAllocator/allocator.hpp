#ifndef STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_
#define STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_

/**
 * \file StaticMemoryAllocator\allocator.hpp
 * \author Angelos Drossos <angelos.drossos@gmail.com>
 */

#include <memory>
#include <array>
#include <cstdint>
#include <iostream>

namespace StaticMemoryAllocator {

template <typename T>
class allocator : public std::allocator<T>
{
public:
	typedef size_t       size_type;
	typedef T            value_type;
	typedef T*           pointer;
	typedef const T*     const_pointer;

	template <typename _Tp1>
	struct rebind
	{
		typedef allocator<_Tp1> other;
	};

/* important allocator functions */
public:

	pointer allocate(size_type n, const void *hint = nullptr)
	{
		std::cout << "alloc " << (n*sizeof(T)) << " bytes (n=" << n << ", hint=" << hint << ")";
		void *const p = &start[current_size];
		std::cout << " --> " << p << std::endl;
		current_size += n*sizeof(T);
		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type n)
	{
		std::cout << "dealloc " << (n*sizeof(T)) << " bytes (n=" << n << ", pointer=" << p <<")" << std::endl;
		current_size -= n*sizeof(T);
	}

/* constructors, destructors, assignment operators */
public:

	allocator() = delete;

	allocator(void *start, size_type size) throw()
		: start(static_cast<uint8_t *>(start)),
		  size(size),
		  current_size(0)
	{
		std::cout << "construct allocator "
			  << "(" << static_cast<void *>(start) << ", " << size << ")"
			  << std::endl;
	}

	allocator(const allocator & other)
		: start(other.GetStart()),
		  size(other.GetSize()),
		  current_size(0)
	{
		std::cout << "construct allocator by copying "
			  << "(" << static_cast<void *>(start) << ", " << size << ")"
			  << std::endl;
	}

	template <class U>
	allocator(const allocator<U> & other) throw()
		: start(other.GetStart()),
		  size(other.GetSize()),
		  current_size(0)
	{
		std::cout << "construct allocator by copying from type U " 
			  << "(" << static_cast<void *>(start) << ", " << size << ")"
			  << std::endl;
	}

	~allocator() throw()
	{
	}

/* getter and setter */
public:

	uint8_t *GetStart() const { return start; }
	size_type GetSize() const { return size; }
	size_type GetCurrentSize() const { return current_size; }

/* member variables */
private:
	uint8_t *start;
	size_type size;
	size_type current_size;
};

} /* namespace StaticMemoryAllocator */
#endif /* STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_ */
