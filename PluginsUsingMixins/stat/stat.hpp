#pragma once

#include "pluginunused.hpp"
#include "pluginunused_impl.hpp"

namespace stat {

template <class data_type, class size_type> class pluginunused1 : protected pluginunused<data_type, size_type> {};
template <class data_type, class size_type> class pluginunused2 : protected pluginunused<data_type, size_type> {};
template <class data_type, class size_type> class pluginunused3 : protected pluginunused<data_type, size_type> {};
template <class data_type, class size_type> class pluginunused4 : protected pluginunused<data_type, size_type> {};

template <typename data_type, typename size_type,
          class P1 = pluginunused1<data_type, size_type>,
	  class P2 = pluginunused2<data_type, size_type>,
	  class P3 = pluginunused3<data_type, size_type>,
	  class P4 = pluginunused4<data_type, size_type>>
class stat : public P1, public P2, public P3, public P4
{
public:
	stat();
	~stat();

public:
	void reset();
	bool add(const data_type value);

private:
	bool check(const data_type value) const;
	void add_first(const data_type a);
	void add_next(const data_type a);

private:
	size_type num;
};

} // namespace stat
