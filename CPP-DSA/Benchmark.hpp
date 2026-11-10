#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>

namespace Algorithms {

	template <auto OPERATION, typename... ARGUMENTS>
	volatile void benchmark(ARGUMENTS&&... arguments) {
		double minimumPerIteration = 999999.0;
		OPERATION(std::forward<ARGUMENTS>(arguments)...);

		for (int run = 0; run < 100; ++run) {
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 10000; ++i)
				OPERATION(std::forward<ARGUMENTS>(arguments)...);
			auto end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double, std::nano> elapsed = end - start;
			double timePerIteration = elapsed.count() / 10000;
			minimumPerIteration = std::min(minimumPerIteration, timePerIteration);
		}

		std::cout << "Time: " << minimumPerIteration << " ns\n";
	}

	template <typename OPERATION, typename... ARGUMENTS>
	volatile void benchmark(OPERATION&& operation, ARGUMENTS&&... arguments) {
		double minimumPerIteration = 999999.0;
		operation(std::forward<ARGUMENTS>(arguments)...);

		for (int run = 0; run < 100; ++run) {
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 10000; ++i)
				operation(std::forward<ARGUMENTS>(arguments)...);
			auto end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double, std::nano> elapsed = end - start;
			double timePerIteration = elapsed.count() / 10000;
			minimumPerIteration = std::min(minimumPerIteration, timePerIteration);
		}

		std::cout << "Time: " << minimumPerIteration << " ns\n";
	}

}