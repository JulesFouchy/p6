#pragma once

namespace p6 {
namespace details {

class Clock {
public:
    Clock()          = default;
    virtual ~Clock() = default;

    virtual float delta_time() const = 0;
    virtual float time() const       = 0;

    virtual void set_time(float new_time) = 0;
    virtual void update()                 = 0;

    virtual void play();
    virtual void pause();
    void         toggle_play_pause();
    inline bool  is_playing() const { return _is_playing; }

private:
    bool _is_playing = true;
};

} // namespace details
} // namespace p6