#include <array>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "sift/filters/gh.hpp"

TEST_CASE("GH Filter benchmarks", "[!benchmark]") {
    using Filter = sift::GHFilter<double, 2>;

    const double pos_0 = 0.0;
    const double vel_0 = 1.0;
    auto const_vel = [pos_0, vel_0](double t) { return pos_0 + t*vel_0; };
    std::array<double, 10'000> measurements;
    for(double t = 0.0; t < 10e3; t += 1.0) {
        measurements[static_cast<size_t>(t)] = const_vel(t + 1.0);
    }


    BENCHMARK("GHFilter<double, 2> construct & destruct") { 
        return Filter({{pos_0, vel_0}}, {{1.0, 0.0}}, 1.0);
    };

    BENCHMARK_ADVANCED("GHFilter<double, 2> predict")
    (Catch::Benchmark::Chronometer meter) {
        std::vector<Filter> filters;
        filters.reserve(meter.runs());

        for (std::size_t i = 0; i < meter.runs(); ++i) {
            filters.emplace_back(
                Filter(
                    {{pos_0, vel_0}},
                    {{1.0, 0.0}},
                    1.0
                )
            );
        }

        meter.measure([&](int i) {
            filters[i].predict();
            return filters[i].state().derivatives[0];
        });
    };

    BENCHMARK_ADVANCED("GHFilter<double, 2> predict & update")
    (Catch::Benchmark::Chronometer meter) {
        std::vector<Filter> filters;
        filters.reserve(meter.runs());

        for (std::size_t i = 0; i < meter.runs(); ++i) {
            filters.emplace_back(
                Filter(
                    {{pos_0, vel_0}},
                    {{1.0, 0.0}},
                    1.0
                )
            );
        }

        meter.measure([&](int i) {
            filters[i].predict();
            filters[i].update(1.0);
            return filters[i].state().derivatives[0];
            }
        );
    };


    BENCHMARK_ADVANCED("GHFilter<double, 2> predict & update (10k passes)")
    (Catch::Benchmark::Chronometer meter) {
        std::vector<Filter> filters;
        filters.reserve(meter.runs());

        for (std::size_t i = 0; i < meter.runs(); ++i) {
            filters.emplace_back(
                Filter(
                    {{pos_0, vel_0}},
                    {{1.0, 0.0}},
                    1.0
                )
            );
        }
        meter.measure([&](int i) {
            for(double z: measurements) {
                filters[i].predict();
                filters[i].update(z);
            }
            return filters[i].state().derivatives[0];
        });
    };
};
