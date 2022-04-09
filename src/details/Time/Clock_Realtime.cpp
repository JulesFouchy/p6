#include "Clock_Realtime.h"

namespace p6
{
namespace details
{
Clock_Realtime::Clock_Realtime()
    : _time_origin(now())
    , _time_last_frame(now())
    , _time_this_frame(now())
    , _time_when_paused(now())
{
}

float Clock_Realtime::delta_time() const
{
    if (is_playing())
    {
        const std::chrono::duration<float> dt = _time_this_frame - _time_last_frame;
        return dt.count();
    }
    else
    {
        return 0.f;
    }
}

float Clock_Realtime::time() const
{
    return std::chrono::duration<float>{
        _time_this_frame - _time_origin}
        .count();
}

void Clock_Realtime::set_time(float new_time)
{
    const auto new_chrono_time = std::chrono::nanoseconds{static_cast<std::chrono::nanoseconds::rep>(
        1000000000.f * new_time // Convert from seconds to nanoseconds
        )};

    _time_origin = _time_this_frame - new_chrono_time;
}

void Clock_Realtime::update()
{
    _time_last_frame = _time_this_frame;
    _time_this_frame = now();
}

void Clock_Realtime::play()
{
    Clock::play();
    const auto delta = now() - _time_when_paused; // ---
    _time_origin += delta;                        // Forward everybody in time
    _time_last_frame += delta;                    // to make as if the pause never happened
    _time_this_frame += delta;                    // ---
}

void Clock_Realtime::pause()
{
    Clock::pause();
    _time_when_paused = now();
}

} // namespace details
} // namespace p6