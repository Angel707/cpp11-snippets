#ifndef STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_
#define STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_

/**
 * \file StaticMemoryAllocator\allocator.hpp
 * \author Angelos Drossos <angelos.drossos@gmail.com>
 */

#include <boost/dynamic_bitset.hpp>

#include <memory>
#include <cstdint>
#include <iostream>
#include <exception>    // std::exception
#include <new>          // std::bad_alloc
#include <string>

#include <assert.h>

namespace StaticMemoryAllocator {

template <class T>
class allocator
{
private:
	typedef uint8_t                            block_type;
	typedef uint8_t                            byte;
	typedef boost::dynamic_bitset<block_type>  dynamic_bitset;

public:
	typedef T                  value_type;
	typedef value_type       * pointer;
	typedef value_type       & reference;
	typedef const value_type * const_pointer;
	typedef const value_type & const_reference;
	typedef std::size_t        size_type;
	typedef std::ptrdiff_t     difference_type;

	template <class _T1>
	struct rebind
	{
		typedef allocator<_T1> other;
	};

/* constructors, destructors, assignment operators */
public:

	allocator() = delete;
	
	allocator(void *const memstart, const size_type memsize, const std::string & memname = "") throw();
	
	allocator(const allocator & a);
	
	allocator(allocator && a);
	
	template <class T2>
	allocator(const allocator<T2> & a) throw();
	
	~allocator() throw();
	
	allocator & operator =(const allocator & a);

	template <class T2>
	allocator & operator =(const allocator<T2> & a);

/* important allocator functions */
public:

	pointer address(reference r) const; // optional
	
	const_pointer address(const_reference r) const; // optional
	
	pointer allocate(size_type n, void *const hint = nullptr);
	
	void deallocate(pointer p, size_type n);
	
	size_type max_size();

	/**
	 * Constructs an element object on the location pointed by p.
	 *
	 * The function constructs an object of type U in-place by forwarding
	 * its arguments to the appropriate constructor, as if the following code was used:
	 * ::new ((void*)p) U (forward<Args>(args)...); 
	 *
	 * \note Notice that this does not allocate space for the element.
	 *       It should already be available at p (see member allocate to allocate space).
	 *
	 * \param p Pointer to a location with enough storage space to contain an element of type U.
	 * \param args Arguments forwarded to the constructor.
	 *             Args is a list of zero or more types.
	 */
	template <class U, class... Args>
	void construct(U *p, Args&&... args); // optional

	/**
	 * Destroys in-place the object pointed by \p p.
	 *
	 * The function uses U's destructor, as if the following code was used:
	 * p->~U();
	 *
	 * \note Notice that this does not deallocate the storage for the element (see member deallocate to release storage space).
	 * 
	 * \param p Pointer to the object to be destroyed.
	 */
	template <class U>
	void destroy(U *p); // optional

	bool operator !=(const allocator & a) const;

public:

	void *const memend(void) const;

	void print_free_memory(void) const;

private:

	static
	dynamic_bitset create_mask(const size_type nb, const size_type memsize);

	static
	size_type find_free_memory(const dynamic_bitset & memfree, dynamic_bitset & mask);

	static
	pointer calc_pointer(void *const memstart, const size_type mempos);

	static
	size_type calc_pos(void *const memstart, void *const pmem);

	static
	dynamic_bitset reserve_memory(const dynamic_bitset & memfree,
		                      dynamic_bitset && mask);

	static
	dynamic_bitset free_memory(const dynamic_bitset & memfree,
			           dynamic_bitset && mask);

/* member variables */
private:

	void *memstart;
	std::shared_ptr<dynamic_bitset> memfree;
	std::string memname;

template <class T2>
friend class allocator;
};


} /* namespace StaticMemoryAllocator */
#endif /* STATIC_MEMORY_ALLOCATOR__ALLOCATOR_H__AD_ */
