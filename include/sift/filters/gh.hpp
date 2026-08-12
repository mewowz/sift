#pragma once

#include <array>
#include <stdexcept>
#include "sift/core/filter_concepts.hpp"

namespace sift {
template <typename T, std::size_t Order>
struct GHState {
    std::array<T, Order + 1> derivatives;
};

template <typename T, std::size_t Order>
class GHFilter {
public:
    using state_type = GHState<T, Order>;
    using measurement_type = T;


    GHFilter(state_type istate, std::array<T, Order + 1> gains, T dt)
        : state_(istate), gains_(gains), dt_(dt) {
        if (!(dt > T{0})) {
            throw std::invalid_argument("GHFilter: dt must be > 0");
        }
        
        T dt_pow = T{1};
        T fact = T{1};
        for (std::size_t i = 0; i <= Order; ++i) {
            predict_coeffs_[i] = (dt_pow / fact);
            update_coeffs_[i] = (gains_[i] * fact) / dt_pow;
            dt_pow *= dt_;
            fact *= static_cast<T>(i + 1);
        }
    }


    void predict() {
        state_type pred{};
        for (std::size_t k = 0; k <= Order; ++k) {
            T sum = T{0};
            for (std::size_t i = 0; k + i <= Order; ++i) {
                sum += state_.derivatives[k + i] * predict_coeffs_[i];
            }
            pred.derivatives[k] = sum;
        }
        state_ = pred;
    }

    void update(measurement_type z) {
        T res = z - state_.derivatives[0];
        for (std::size_t k = 0; k <= Order; ++k) {
            state_.derivatives[k] += res * update_coeffs_[k];
        }
    }
    
    const state_type& state() const { return state_; }

private:
    state_type state_;
    std::array<T, Order + 1> gains_;
    T dt_;

    std::array<T, Order + 1> predict_coeffs_;
    std::array<T, Order + 1> update_coeffs_;
};
} //namespace sift
