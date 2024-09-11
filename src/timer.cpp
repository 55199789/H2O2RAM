#include "timer.hpp"

Timer::Timer()
    : start_clock(std::chrono::steady_clock::now()),
      last_stop(std::chrono::steady_clock::now())
{
}

double Timer::get_total_time() const
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - this->start_clock).count() / 1000000.0;
}

double Timer::get_interval_time()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double interval_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now - this->last_stop).count();
    this->last_stop = now;
    return interval_time / 1000000000.0;
}