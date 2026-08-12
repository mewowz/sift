#pragma once

#include <concepts>
#include <utility>

/* 
* StateEstimator is a concept because we don't want to assume that
* every estimator has the same shape, members, or steps to it. However, 
* basically every estimator is a predict/update loop. Nearly 
* everything should be able to be implemented based on this
* general concept at the very least
*/

namespace sift::core {

template <typename F>
concept StateEstimator = requires(F f) {
    typename F::state_type;
    typename F::measurement_type;
    { f.predict() } -> std::same_as<void>;
    { f.update(std::declval<typename F::measurement_type>()) } -> std::same_as<void>;
    { f.state() } -> std::convertible_to<typename F::state_type>;
};

}  // namespace sift::core
