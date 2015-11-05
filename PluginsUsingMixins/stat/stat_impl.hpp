#pragma once

#include "stat.hpp"
#include "plugins/plugin_impl.hpp"

#include <cstdio>

namespace stat {

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
stat<data_type, size_type, P1, P2, P3, P4>::stat()
: num(0)
{
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
stat<data_type, size_type, P1, P2, P3, P4>::~stat()
{
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
void stat<data_type, size_type, P1, P2, P3, P4>::reset()
{
	P1::reset();
	P2::reset();
	P3::reset();
	P4::reset();
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
bool stat<data_type, size_type, P1, P2, P3, P4>::add(const data_type value)
{
	if (!check(value))
		return false;

	if (num < 1)
		add_first(value);
	else
		add_next(value);
	return true;
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
bool stat<data_type, size_type, P1, P2, P3, P4>::check(const data_type value) const
{
	bool succ = true;
	succ = succ && P1::check(value, num);
	succ = succ && P2::check(value, num);
	succ = succ && P3::check(value, num);
	succ = succ && P4::check(value, num);
	return succ;
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
void stat<data_type, size_type, P1, P2, P3, P4>::add_first(const data_type value)
{
	++num;
	P1::add_first(value, num);
	P2::add_first(value, num);
	P3::add_first(value, num);
	P4::add_first(value, num);
}

template <typename data_type, typename size_type,
          class P1, class P2, class P3, class P4>
void stat<data_type, size_type, P1, P2, P3, P4>::add_next(const data_type value)
{
	++num;
	P1::add_next(value, num);
	P2::add_next(value, num);
	P3::add_next(value, num);
	P4::add_next(value, num);
}


} // namespace stat
