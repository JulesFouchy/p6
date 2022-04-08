#include "Clock_Realtime.h"

namespace p6 {
namespace details {

Clock_Realtime::Clock_Realtime()
    : _initial_time(std_time())
    , _last_time(std_time())
    , _current_time(std_time())
{
}

float Clock_Realtime::delta_time() const
{
    if (is_playing()) {
        const std::chrono::duration<float> dt = _current_time - _last_time;
        return dt.count();
    }
    else {
        return 0.f;
    }
}

float Clock_Realtime::time() const
{
    const std::chrono::duration<float> t = _current_time - _initial_time - _offset_with_std_time;
    return t.count();
}

void Clock_Realtime::set_time(float new_time)
{
    const std::chrono::nanoseconds new_chrono_time{static_cast<std::chrono::nanoseconds::rep>(1000000000.f * new_time)};
    _offset_with_std_time = _current_time - _initial_time - new_chrono_time;
}

void Clock_Realtime::update()
{
    if (is_playing()) {
        _last_time    = _current_time;
        _current_time = std_time();
    }
}

void Clock_Realtime::play()
{
    Clock::play();
    const auto delta = std_time() - _time_when_paused;
    _offset_with_std_time += delta;
    _last_time += delta;
    _current_time += delta;
}

void Clock_Realtime::pause()
{
    Clock::pause();
    _time_when_paused = std_time();
}

} // namespace details
} // namespace p6