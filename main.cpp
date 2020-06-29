#include "Clicker.h"
#include "Twitch.h"
#include "FileIO.h"

int main() {
    const char* coords_txt = "coordinates.txt";
    bool read = FALSE;

    Clicker clicker;
    POINT point = {};
    RECT rect = {};
    
    POINT window_size = {};

    while (true) {
        if (strstr(Clicker::get_window_name(), "Twitch")) {
            if (GetKeyState(VK_NUMPAD0) & 0x8000 && GetKeyState(VK_NUMPAD1) & 0x8000) {
                FileIO::write_pos(coords_txt);
                point = FileIO::read_from_file(coords_txt);
                read = TRUE;
            }
            if (!read) {
                rect = Clicker::get_window_rect_foreground();
                window_size = Clicker::rect_to_point(rect);
                point = Twitch::get_reward_pos(window_size);
                Clicker::client_to_screen(&point);
            }
            if (point.x != 0) {
                if (!Clicker::wait_and_click(point.x, point.y, Twitch::reward_color, 1)) {
                    continue;
                }
            }
            Sleep(1000);
        }
        Sleep(1000);
    }
    return 0;
}