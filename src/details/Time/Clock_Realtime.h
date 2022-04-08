#pragma once

#include <chrono>
#include "Clock.h"

namespace p6 {
namespace details {

class Clock_Realtime : public Clock {
public:
    Clock_Realtime();

    float delta_time() const override;
    float time() const override;

    void set_time(float new_time) override;
    void update() override;

    void play() override;
    void pause() override;

private:
    std::chrono::steady_clock::time_point std_time() { return std::chrono::steady_clock::now(); }

private:
    std::chrono::steady_clock::time_point _initial_time;
    std::chrono::steady_clock::time_point _last_time;
    std::chrono::steady_clock::time_point _current_time;
    std::chrono::steady_clock::time_point _time_when_paused;
    std::chrono::nanoseconds              _offset_with_std_time{0}; // Allows us to set_time as we wish
};

} // namespace details
} // namespace p6