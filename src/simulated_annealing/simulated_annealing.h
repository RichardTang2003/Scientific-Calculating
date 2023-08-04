#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <cmath>
#include <random>
#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../../deps/Xoshiro-cpp/XoshiroCpp.hpp"

template<typename status, typename count, typename energy_function, typename temperature_function, typename next_function, typename generator>
status simulated_annealing(status i_old, count c, const energy_function& ef, const temperature_function& tf, const next_function& nf, generator& g) {

	auto   e_old = ef(i_old);

	status i_best = i_old;
	auto   e_best = e_old;

	std::uniform_real_distribution<decltype(e_old)> rf(0, 1);

	for (; c > 0; --c) {
		status i_new = nf(i_old, g);
		auto   e_new = ef(i_new);

		if (e_new < e_best) {
			i_best = i_new;
			e_best = e_new;
		}

		if (e_new < e_old || std::exp((e_old - e_new) / tf(c)) > rf(g)) {
			i_old = std::move(i_new);
			e_old = std::move(e_new);
		}
	}
	return(i_best);
}

#endif // SIMULATED_ANNEALING_H
