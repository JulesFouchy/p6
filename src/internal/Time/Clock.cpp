#include "Clock.h"
#include <cassert>

namespace p6::internal {

void Clock::play()
{
    _is_playing = true;
}

void Clock::pause()
{
    _is_playing = false;
}

void Clock::toggle_play_pause()
{
    if (_is_playing)
    {
        pause();
    }
    else
    {
        play();
    }
}

} // namespace p6::internal