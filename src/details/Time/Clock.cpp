#include "Clock.h"
#include <cassert>

namespace p6 {
namespace details {

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
    if (_is_playing) {
        pause();
    }
    else {
        play();
    }
}

} // namespace details
} // namespace p6