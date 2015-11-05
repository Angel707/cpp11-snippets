#pragma once

#include <cstdlib>
#include <cstdint>

namespace stat {

template <typename data_type, typename size_type> 
class plugin1
{
public:
	template <typename T = data_type> T getv() const { return static_cast<T>(v); }
	template <typename T = data_type> T getm() const { return static_cast<T>(m); }
	template <typename T = data_type> T getms() const { return static_cast<T>(ms); }

protected:
	void reset();
	bool check(const data_type value, const size_type n_current) const;
	void add_first(const data_type value, const size_type n_next);
	void add_next(const data_type value, const size_type n_next);

private:
	data_type v;
	data_type m;
	data_type ms;
};

} // namespace stat
