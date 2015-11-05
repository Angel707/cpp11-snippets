#pragma once

#include "stat.hpp"
#include "plugin1.hpp"

#include <iostream>

namespace stat {

template <typename data_type, typename size_type>
void plugin1<data_type, size_type>::reset()
{
	std::cout << "plugin1::reset" << std::endl;
	v = 0;
	m = 0;
	ms = 1;
}

template <typename data_type, typename size_type>
bool plugin1<data_type, size_type>::check(const data_type value, const size_type n) const
{
	std::cout << "plugin1::check: " << value << std::endl;
	return (value >= 1);
}

template <typename data_type, typename size_type>
void plugin1<data_type, size_type>::add_first(const data_type value, const size_type n)
{
	std::cout << "plugin1::add_first: " << value << std::endl;
	v = value;
	m = 0;
	ms = 3;
}

template <typename data_type, typename size_type>
void plugin1<data_type, size_type>::add_next(const data_type value, const size_type n)
{
	std::cout << "plugin1::add_next: " << value << std::endl;
	v += value;
	m += 1;
	ms += 2;
}


} // namespace stat
