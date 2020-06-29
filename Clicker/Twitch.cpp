#include "Twitch.h"

const int Twitch::twitch_offset_x = 260;
const int Twitch::twitch_offset_y = 40;
const char* Twitch::reward_color = "0x0e6cb";

POINT Twitch::calculate_twitch_reward(RECT window_pos) {
    POINT point = {};
    point.x = window_pos.right - Twitch::twitch_offset_x;
    point.y = window_pos.bottom - Twitch::twitch_offset_y;
    return point;
}

POINT Twitch::get_reward_pos(POINT window_size) {
    POINT point = {};
    point.x = window_size.x - Twitch::twitch_offset_x;
    point.y = window_size.y - Twitch::twitch_offset_y;
    return point;
}