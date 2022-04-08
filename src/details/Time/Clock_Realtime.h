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
    std::chrono::steady_clock::time_point now() { return std::chrono::steady_clock::now(); }

private:
    std::chrono::steady_clock::time_point _time_origin;
    std::chrono::steady_clock::time_point _time_last_frame;
    std::chrono::steady_clock::time_point _time_this_frame;
    std::chrono::steady_clock::time_point _time_when_paused;
};

} // namespace details
} // namespace p6