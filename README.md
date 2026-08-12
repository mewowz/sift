# sift

A general-purpose C++ library implementing the standard family of recursive state estimation filters under a single, minimal interface, with two first-class performance targets:

1. **Single filter, hot loop.** For embedded platforms, or you can't pay for extra overhead, and need one filter instance updated as fast as possible, every cycle, for the life of the process.
2. **Many filters, in parallel.** Running a filter bank and need to score and select among them as fast as your tools will let you.

## Status

**Current Filters**
- g-h filter to the n-th order

## Planned filters

- Kalman Filter Family (KF, UKF, EKF, EnKF, etc.)
- Particle filter (bootstrap, Rao-Blackwellized, auxiliary)
- H-infinity filter

## Goals

- **Header-first C++20 core**, using concepts rather than virtual dispatch. That way, filters that don't need a covariance matrix or Jacobian are't forced to carry one.
- **CPU backend** optimized for low-latency single-filter execution with cache-friendly layout, minimal call overhead and an embedded-safe build mode
- **CUDA backend** optimized for batched multi-filter execution. SoA layout, batched small-matrix operations for the linear/EKF/UKF family, and CUDA-parallelized NIS scoring across a filter bank.
- **Python bindings** as a layer on top of the core

## Build
Current way to build and execute tests:
```sh
cmake -S . -B build -DSIFT_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
