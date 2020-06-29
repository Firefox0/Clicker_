#pragma once

#include <Windows.h>

class Twitch {
private:
    static const int twitch_offset_x;
    static const int twitch_offset_y;

public:
    static const char* reward_color;
    static POINT calculate_twitch_reward(RECT window_pos);
    static POINT get_reward_pos(POINT window_size);
};