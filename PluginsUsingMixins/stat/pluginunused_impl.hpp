#pragma once

#include "pluginunused.hpp"

#include <iostream>

namespace stat {

template <class data_type, class size_type>
void pluginunused<data_type, size_type>::reset()
{
	std::cout << "default::reset" << std::endl;
}

template <class data_type, class size_type>
bool pluginunused<data_type, size_type>::check(const data_type value, const size_type n_current) const
{
	std::cout << "default::check: " << value << std::endl;
	return true;
}

template <class data_type, class size_type>
void pluginunused<data_type, size_type>::add_first(const data_type value, const size_type n_next)
{
	std::cout << "default::add_first: " << value << std::endl;
}

template <class data_type, class size_type>
void pluginunused<data_type, size_type>::add_next(const data_type value, const size_type n_next)
{
	std::cout << "default::add_next: " << value << std::endl;
}

} // namespace stat
