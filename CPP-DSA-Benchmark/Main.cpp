#include <benchmark/benchmark.h>
#include <cstddef>
#include <vector>

#include "Stack.hpp"
#include "Types.hpp"

using namespace DSA;

template <typename T>
using CustomStack =
DSA::Stack<DSA::ContainerPolicy<DSA::HeapStoragePolicy<>>, T>;

// ============================================================
// Preallocated: no allocation or growth during the benchmark
// ============================================================

template <typename T>
static void BM_Stack_Preallocated(benchmark::State& state)
{
    const std::size_t count = state.range(0);

    CustomStack<T> stack(count);
    T value{};

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
            stack.push(value);

        benchmark::ClobberMemory();

        for (std::size_t i = 0; i < count; ++i)
            stack.remove();
    }

    state.SetItemsProcessed(state.iterations() * count * 2);
}

template <typename T>
static void BM_Vector_Preallocated(benchmark::State& state)
{
    const std::size_t count = state.range(0);

    std::vector<T> vector;
    vector.reserve(count);

    T value{};

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
            vector.push_back(value);

        benchmark::ClobberMemory();

        for (std::size_t i = 0; i < count; ++i)
            vector.pop_back();
    }

    state.SetItemsProcessed(state.iterations() * count * 2);
}


// ============================================================
// Growing: allocation and deallocation included in each
// iteration; both containers start with capacity N / 4.
// ============================================================

template <typename T>
static void BM_Stack_Growing(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    const std::size_t initialCapacity = count / 4;

    T value{};
    std::size_t finalCapacity = 0;

    for (auto _ : state)
    {
        CustomStack<T> stack(initialCapacity);

        for (std::size_t i = 0; i < count; ++i)
        {
            if (!stack.can_push())
                stack.expand();

            stack.push(value);
        }

        benchmark::ClobberMemory();

        finalCapacity = stack.get_capacity();

        for (std::size_t i = 0; i < count; ++i)
            stack.remove();
    }

    state.counters["FinalCapacity"] = static_cast<double>(finalCapacity);
    state.SetItemsProcessed(state.iterations() * count * 2);
}

template <typename T>
static void BM_Vector_Growing(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    const std::size_t initialCapacity = count / 4;

    T value{};
    std::size_t finalCapacity = 0;

    for (auto _ : state)
    {
        std::vector<T> vector;
        vector.reserve(initialCapacity);

        for (std::size_t i = 0; i < count; ++i)
            vector.push_back(value);

        benchmark::ClobberMemory();

        finalCapacity = vector.capacity();

        for (std::size_t i = 0; i < count; ++i)
            vector.pop_back();
    }

    state.counters["FinalCapacity"] = static_cast<double>(finalCapacity);
    state.SetItemsProcessed(state.iterations() * count * 2);
}


// ============================================================
// Registration
// ============================================================

#define REGISTER_BENCHMARKS(TYPE)                                      \
    BENCHMARK_TEMPLATE(BM_Stack_Preallocated, TYPE)                    \
        ->RangeMultiplier(2)->Range(16, 65536);                        \
    BENCHMARK_TEMPLATE(BM_Vector_Preallocated, TYPE)                   \
        ->RangeMultiplier(2)->Range(16, 65536);                        \
    BENCHMARK_TEMPLATE(BM_Stack_Growing, TYPE)                         \
        ->RangeMultiplier(2)->Range(16, 65536);                        \
    BENCHMARK_TEMPLATE(BM_Vector_Growing, TYPE)                        \
        ->RangeMultiplier(2)->Range(16, 65536);

REGISTER_BENCHMARKS(int)
REGISTER_BENCHMARKS(LargeTrivial)
REGISTER_BENCHMARKS(NonTrivial)

#undef REGISTER_BENCHMARKS

BENCHMARK_MAIN();