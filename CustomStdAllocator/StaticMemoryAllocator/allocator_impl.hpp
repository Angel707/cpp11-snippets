#ifndef NDEBUG

//#define DEBUG_SMA_TRACE_INTERFACE 1
#define DEBUG_SMA_TRACE_MEMALLOCATION 1
//#define DEBUG_SMA_TRACE_BITSET 1
//#define DEBUG_SMA_TRACE_CALC_POINTER_POS 1
//#define DEBUG_SMA_TRACE_FIND_FREE_MEM 1

#endif /* NDEBUG */

#include "allocator.hpp"

namespace StaticMemoryAllocator {

template <class T>
allocator<T>::allocator(void *const memstart, const size_type memsize, const std::string & memname) throw()
	: memstart(memstart),
	  memfree(std::make_shared<dynamic_bitset>(memsize)),
	  memname(memname)
{
	assert(this->memstart != nullptr);
	assert(memsize > 0);
	assert(memfree->size() == memsize);
	for (size_type i = 0; i < memsize; i++) memfree->set(i);
	assert(memfree->count() == memsize);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "construct allocator: "
		  << "start=" << this->memstart << ", "
		  << "size=" << this->memfree->size() << " bytes, "
		  << "name='" << this->memname << "'"
		  << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
}

template <class T>
allocator<T>::allocator(const allocator & a)
	: memstart(a.memstart),
	  memfree(a.memfree),
	  memname(a.memname)
{
	assert(this->memstart != nullptr);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "copy-construct allocator: "
		  << "start=" << this->memstart << ", "
		  << "size=" << this->memfree->size() << " bytes, "
		  << "name='" << this->memname << "'"
		  << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
}

template <class T>
allocator<T>::allocator(allocator && a)
	: memstart(a.memstart),
	  memfree(a.memfree),
	  memname(a.memname)
{
	assert(this->memstart != nullptr);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "move-construct allocator: "
		  << "start=" << this->memstart << ", "
		  << "size=" << this->memfree->size() << " bytes, "
		  << "name='" << this->memname << "'"
		  << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
}

template <class T>
template <class T2>
allocator<T>::allocator(const allocator<T2> & a) throw()
	: memstart(a.memstart),
	  memfree(a.memfree),
	  memname(a.memname)
{
	assert(this->memstart != nullptr);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "copy-construct allocator (of different value type): "
		  << "start=" << this->memstart << ", "
		  << "size=" << this->memfree->size() << " bytes, "
		  << "name='" << this->memname << "'"
		  << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
}

template <class T>
allocator<T>::~allocator() throw()
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "destruct allocator: "
		  << "start=" << this->memstart << ", "
		  << "size=" << this->memfree->size() << " bytes, "
		  << "name='" << this->memname << "'"
		  << std::endl;
#	endif
}

template <class T>
allocator<T> & allocator<T>::operator =(const allocator & a)
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "copy-assign allocator" << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
	return *this;
}

template <class T>
template <class T2>
allocator<T> & allocator<T>::operator =(const allocator<T2> & a)
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "copy-assign allocator (of different value type)" << std::endl;
#	endif
#       if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
	return *this;
}

template <class T>
typename allocator<T>::pointer allocator<T>::address(reference r) const // optional
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "get address" << std::endl;
#	endif
	return &r;
}

template <class T>
typename allocator<T>::const_pointer allocator<T>::address(const_reference r) const // optional
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "get address (const)" << std::endl;
#	endif
	return &r;
}

template <class T>
typename allocator<T>::pointer allocator<T>::allocate(size_type n, void *const hint)
{
	const size_type nb = n * sizeof(T);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "alloc " << nb << " bytes (n=" << n << ", hint=" << static_cast<void *>(hint) << ")" << std::endl;
#	endif
	assert(nb > 0);
	if (!(nb > 0)) {
		std::cerr << "cannot allocate a memory block of size " << nb << std::endl;
		goto badalloc;
	}
	if (nb > memfree->size()) {
		std::cerr << "triing to allocate a memory block of size " << nb << " bytes, "
			  << "but the managed memory has only a size of " << memfree->size() << " bytes." << std::endl;
		goto badalloc;
	}
	{//block
		auto mask = create_mask(nb, memfree->size());
		const size_type pos = find_free_memory(*memfree, mask);
		assert(0 <= pos && pos <= memfree->size());
		if (pos < memfree->size()) {
#			if DEBUG_SMA_TRACE_MEMALLOCATION
			std::cout << "reserved memory: " << mask.count() << " bytes -> "
				  << (memfree->count() - mask.count()) << " bytes free."
				  << std::endl;
#			endif
			*memfree = reserve_memory(*memfree, std::move(mask));
#			if DEBUG_SMA_TRACE_MEMALLOCATION
			print_free_memory();
#			endif
			return calc_pointer(memstart, pos);
		} else {
#			if DEBUG_SMA_TRACE_MEMALLOCATION
			std::cout << "not enough free memory to allocate " << nb << " bytes." << std::endl;
			print_free_memory();
#			endif
		}
	}
badalloc:
	/* not enough memory free */
	throw std::bad_alloc();
	return nullptr;
}

template <class T>
void allocator<T>::deallocate(pointer p, size_type n)
{
	const size_type nb = n * sizeof(T);
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "dealloc " << nb << " bytes (n=" << n << ", pointer=" << static_cast<void *>(p) <<")" << std::endl;
#	endif
	assert(nb > 0);
	const size_type pos = calc_pos(memstart, p);
	assert(0 <= pos && pos < memfree->size());
	auto mask = create_mask(nb, memfree->size()) << pos;
#	if DEBUG_SMA_TRACE_MEMALLOCATION
	std::cout << "freeed memory: " << mask.count() << " bytes -> "
		  << (memfree->count() + mask.count()) << " bytes free."
		  << std::endl;
#	endif
	*memfree = free_memory(*memfree, std::move(mask));
#	if DEBUG_SMA_TRACE_MEMALLOCATION
	print_free_memory();
#	endif
}

template <class T>
typename allocator<T>::size_type allocator<T>::max_size()
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "get max size" << std::endl; 
#	endif
	size_type max = 0;
	size_type k = 0;
	for (size_type i = 0; i < memfree->size(); i++) {
		if (memfree->test(i)) {
			k++;
		} else {
			if (k > max) max = k;
			k = 0;
		}
	}
#	if DEBUG_SMA_TRACE_MEMALLOCATION
	std::cout << "max allocatable blocksize is " << max << " bytes." << std::endl;
#	endif
	return max;
}

template <class T>
template <class U, class... Args>
void allocator<T>::construct(U *p, Args&&... args) // optional
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "contruct element in allocator at " << static_cast<void *>(p) << std::endl;
#	endif
	::new ((void *)p) U(std::forward<Args>(args)...);
}

template <class T>
template <class U>
void allocator<T>::destroy(U *p) // optional
{
#	if DEBUG_SMA_TRACE_INTERFACE
	std::cout << "destroy element in allocator at " << static_cast<void *>(p) << std::endl;
#	endif
	p->~U();
}

template <class T>
bool allocator<T>::operator !=(const allocator & a) const
{
	if (memfree->size() != a.memfree->size()) return true;
	for (size_type i = 0; i < memfree->size(); i++) {
		if (memfree->test(i) != a.memfree->test(i)) return true;
	}
	return false;
}

template <class T>
void allocator<T>::print_free_memory(void) const
{
	std::cout << "free memory (" << ((memname.empty()) ? "<unnamed>" : memname) << "): " 
		  << *memfree << " at (" << memend() << ", " << memstart << "]" << std::endl;
}

template <class T>
typename allocator<T>::dynamic_bitset allocator<T>::create_mask(const size_type nb, const size_type memsize)
{
	dynamic_bitset mask(memsize, false);
	for (size_type i = 0; i < nb; i++) mask.set(i);
	assert(mask.count() == nb);
	return mask;
}

template <class T>
typename allocator<T>::size_type allocator<T>::find_free_memory(const dynamic_bitset & memfree, dynamic_bitset & mask)
{
	/* search mask in memfree */
	const size_type needed_mem_size = mask.count();
	if (memfree.count() < needed_mem_size) {
#		if DEBUG_SMA_TRACE_FIND_FREE_MEM
		std::cout << "not enough free memory found" << std::endl;
#		endif
		return memfree.size();
	}
	assert(needed_mem_size <= memfree.size());
	const size_type asize = memfree.size() - needed_mem_size;
	size_type pos = 0;
	while (pos <= asize) {
#		if DEBUG_SMA_TRACE_FIND_FREE_MEM
		std::cout << "pos:     " << pos << "/" << asize << std::endl;
		std::cout << "mask:    " << mask << std::endl;
		std::cout << "memfree: " << memfree << std::endl;
#		endif
		/* check if enough memory is free in interval [pos, pos+needed_mem_size) */
		if ((memfree & mask) == mask) {
			/* free memory found */
#			if DEBUG_SMA_TRACE_FIND_FREE_MEM
			std::cout << "free memory found at pos=" << pos << std::endl;
#			endif
			return pos;
		} else {
			/* increment pos for next iteration */
#			if DEBUG_SMA_TRACE_FIND_FREE_MEM
			std::cout << "memfree:  " << memfree << "    "
				  << memfree << std::endl;
			std::cout << "inc mask: " << mask << " -> ";
#			endif
			const size_type oldpos = pos;
			pos = memfree.find_next(oldpos);
			assert(pos >= oldpos);
			mask <<= pos - oldpos;
#			if DEBUG_SMA_TRACE_FIND_FREE_MEM
			std::cout << mask << std::endl;
			std::cout << "pos: " << oldpos << " -> "
				  << pos << std::endl;
#			endif
			assert(mask.count() == needed_mem_size);
		}
	}
	/* no free memory found */
#	if DEBUG_SMA_TRACE_FIND_FREE_MEM
	std::cout << "not enough free memory found" << std::endl;
#	endif
	return memfree.size();
}

template <class T>
void *const allocator<T>::memend() const
{
	assert(memstart != nullptr);
	byte *const mem = static_cast<byte *>(memstart);
	void *const mend = &mem[memfree->size()];
	assert(mend != nullptr);
	return mend;
}

template <class T>
typename allocator<T>::pointer allocator<T>::calc_pointer(void *const start, const size_type pos)
{
	assert(start != nullptr);
	byte *const mem = static_cast<byte *>(start);
	void *pmem = &mem[pos];
#	if DEBUG_SMA_TRACE_CALC_POINTER_POS
	std::cout << "calc pointer(start=" << start << ", pos=" << pos << ") --> " << pmem << std::endl;
#	endif
	assert(pmem != nullptr);
	return static_cast<pointer>(pmem);
}

template <class T>
typename allocator<T>::size_type allocator<T>::calc_pos(void *const start, void *const pmem)
{
	assert(start != nullptr);
	assert(pmem != nullptr);
	byte *const mem = static_cast<byte *>(start);
	byte *const p = static_cast<byte *>(pmem);
	assert(p >= mem);
	const size_type pos = static_cast<size_type>(p - mem);
#	if DEBUG_SMA_TRACE_CALC_POINTER_POS
	std::cout << "calc pos(start=" << start << ", p=" << pmem << ") --> " << pos << std::endl;
#	endif
	return pos;
}

template <class T>
typename allocator<T>::dynamic_bitset allocator<T>::reserve_memory(const dynamic_bitset & memfree,
	                                                           dynamic_bitset && mask)
{
	auto res = memfree & ~mask;
#	if DEBUG_SMA_TRACE_BITSET
	std::cout << "a=memfree: " << memfree << std::endl;
	std::cout << "b=   mask: " << mask << std::endl;
	std::cout << "r= a & ~b: " << res << std::endl;
#	endif
	assert(memfree.count() - mask.count() == res.count());
	return res;
}

template <class T>
typename allocator<T>::dynamic_bitset allocator<T>::free_memory(const dynamic_bitset & memfree,
	                                                        dynamic_bitset && mask)
{
	assert(memfree.size() == mask.size());
	assert(memfree.count() + mask.count() <= memfree.size());
	auto res = memfree | mask;
#	if DEBUG_SMA_TRACE_BITSET
	std::cout << "a=memfree: " << memfree << std::endl;
	std::cout << "b=   mask: " << mask << std::endl;
	std::cout << "r=  a | b: " << res << std::endl;
#	endif
	assert(memfree.count() + mask.count() == res.count());
	return res;
}

} /* namespace StaticMemoryAllocator */

