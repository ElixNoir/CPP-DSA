#pragma once

#include <algorithm>
#include <chrono>


namespace Algorithms {

    template <size_t ITERATIONS, typename F, size_t RUNS = 10>
    [[nodiscard]] double benchmark(F&& function) {
        using Clock = std::chrono::steady_clock;

        function(); // Warm up

        double results[RUNS]{};

        for (size_t run = 0; run < RUNS; ++run) {
            auto start = Clock::now();
            function();
            auto end = Clock::now();
            results[run] = std::chrono::duration<double, std::nano>(end - start).count() / ITERATIONS;
        }

        std::sort(results.begin(), results.end());

        return results[RUNS / 2];
    }

}