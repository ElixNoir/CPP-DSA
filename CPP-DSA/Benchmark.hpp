#include <chrono>
#include <algorithm>
#include <array>

static constexpr size_t RUNS = 10;

template <size_t ITERATIONS, typename F>
[[nodiscard]] double benchmark(F&& function) {
    using Clock = std::chrono::steady_clock;

    function(); // Warm up

    std::array<double, RUNS> results{};

    for (size_t run = 0; run < RUNS; ++run) {
        auto start = Clock::now();
        function();
        auto end = Clock::now();
        results[run] = std::chrono::duration<double, std::nano>(end - start).count() / ITERATIONS;
    }

    std::sort(results.begin(), results.end());

    return results[RUNS / 2];
}