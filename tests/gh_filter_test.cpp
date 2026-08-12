#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "sift/filters/gh.hpp"

using namespace Catch::Matchers;

TEST_CASE("GHFilter order 0. A stationary object", "[gh]") {
    sift::GHFilter<double, 0> f({{0.}}, {{0.}}, 1.);
    f.predict();
    REQUIRE(f.state().derivatives[0] == 0.);
    f.update(0.);
    REQUIRE(f.state().derivatives[0] == 0.);
    f.predict();
    REQUIRE(f.state().derivatives[0] == 0.);
    f.update(0.);
    REQUIRE(f.state().derivatives[0] == 0.);
}

TEST_CASE("GHFilter order 0. Noisy measurements", "[gh]") {
    // Our real measurement is 44, but whatever is measuring it
    // is a little noisy
    std::vector<double> measurements{{43.4, 44.3, 44.1, 44.0, 43.9, 44.2, 44.1}};
    sift::GHFilter<double, 0> f({{43.9}}, {{0.5}}, 1.);
    for(double z: measurements) {
        f.predict();
        REQUIRE_THAT(f.state().derivatives[0], WithinRel(44.0, 0.2));
        f.update(z);
    }
    
    auto& state = f.state();
    REQUIRE_THAT(state.derivatives[0], WithinRel(44.0, 0.2));
}

TEST_CASE("GHFilter order 3. Predict matches a hand-derived Taylor series", "[gh]") {
    double x = 0.0, v = 1.0, a = 2.0, j = 6.0, dt = 1.0;
    sift::GHFilter<double, 3> f({{x, v, a, j}}, {{0.0, 0.0, 0.0, 0.0}}, dt);
    f.predict();
 
    double expected_pos = x + v * dt + a * dt * dt / 2.0 + j * dt * dt * dt / 6.0;
    REQUIRE_THAT(f.state().derivatives[0], WithinRel(expected_pos, 0.1));
}

TEST_CASE("GHFilter order 3 tracks a constant-jerk trajectory", "[gh]") {
    const double dt = 0.1;
    auto true_pos = [](double t) { return t * t * t; };
    auto true_vel = [](double t) { return 3.0 * t * t; };
    auto true_acc = [](double t) { return 6.0 * t; };
    const double true_jerk = 6.0;
 
    std::vector<double> measurements;
    for (int k = 1; k <= 30; ++k) {
        double t = k * dt;
        measurements.push_back(true_pos(t));
    }
 
    sift::GHFilter<double, 3> f(
        {{0.0, 0.0, 0.0, 0.0}},
        {{1, 1.5, 0.75, 0.095}},
        dt);
 
    int k = 0;
    for (double z : measurements) {
        ++k;
        f.predict();
        f.update(z);
 
        if (k == 10) {
            double t = k * dt;
            const auto& s = f.state();
            REQUIRE_THAT(s.derivatives[0], WithinAbs(true_pos(t), 1e-6));
            REQUIRE_THAT(s.derivatives[1], WithinAbs(true_vel(t), 1e-3));
            REQUIRE_THAT(s.derivatives[2], WithinAbs(true_acc(t), 1e-2));
            REQUIRE_THAT(s.derivatives[3], WithinAbs(true_jerk, 1e-1));
        }
    }
 
    double t_final = k * dt;
    const auto& s = f.state();
    REQUIRE_THAT(s.derivatives[0], WithinRel(true_pos(t_final), 1e-6));
    REQUIRE_THAT(s.derivatives[1], WithinRel(true_vel(t_final), 1e-6));
    REQUIRE_THAT(s.derivatives[2], WithinRel(true_acc(t_final), 1e-6));
    REQUIRE_THAT(s.derivatives[3], WithinRel(true_jerk, 1e-6));
}
