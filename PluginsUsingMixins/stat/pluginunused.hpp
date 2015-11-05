#pragma once

namespace stat {

template <class data_type, class size_type>
class pluginunused
{
protected:
	void reset();
	bool check(const data_type value, const size_type n_current) const;
	void add_first(const data_type value, const size_type n_next);
	void add_next(const data_type value, const size_type n_next);
};

} // namespace stat
