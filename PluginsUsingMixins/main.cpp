/* Note:
 * The class stat works with float values but
 * the class plugin1 should work with higher precision and uses double values. 
 * Thus, the return values of plugin1 have double precision by default, too.
 */
#include "stat/plugin1_impl.hpp"
#include "stat/stat_impl.hpp"
typedef stat::stat<float, size_t, stat::plugin1<double, size_t>> mystat;

#include <iostream>
#include <typeinfo>

/**
 * main function.
 */
int main(int, char **)
{
	std::cout << "Test plugin system.." << std::endl;
	int ret = 0;
	mystat b;
	b.reset();
	b.add(1);
	b.add(2.1f);
	const auto v1 = b.getv();
	std::cout << "v1 is " << v1 << std::endl;
	std::cout << "type of v1 is " << typeid(v1).name() << std::endl;
	const auto v2 = b.getv<long>();
	std::cout << "v2 is " << v2 << std::endl;
	std::cout << "type of v2 is " << typeid(v2).name() << std::endl;
	return ret;
}

