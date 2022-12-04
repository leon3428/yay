#include "Utils.hpp"

ScopeTimer::ScopeTimer()
    : start(std::chrono::high_resolution_clock::now()) {}

ScopeTimer::~ScopeTimer() {
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    double ms = duration.count() * 1000.0f;
    std::cout << "Scoped timer: " << ms << "ms\n";
}