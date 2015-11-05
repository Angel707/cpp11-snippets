#pragma once

/**
 * Plugin interface.
 */
namespace stat { namespace plugins {

	template <typename data_type, typename T> bool check(const data_type, const T&);
	template <typename data_type, typename T> void add_first(const data_type, T&);
	template <typename data_type, typename T> void add_next(const data_type, T&);

}} /*< namespace stat::plugins */
