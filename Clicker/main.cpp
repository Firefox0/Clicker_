#include "Clicker.h"
#include "Twitch.h"
#include "FileIO.h"

int main() {
    const char* coords_txt = "coordinates.txt";
    bool read = FALSE;

    POINT point = {};
    RECT rect = {};

    POINT window_size = {};

    while (true) {
        if (Clicker::get_window_name().find("Twitch") != -1) {
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
                point = Clicker::scan_area(point.x, point.x + 20, point.y, point.y + 10, Twitch::reward_color);
                if (point.x != 0) {
                    Clicker::move_and_click(point.x, point.y);
                    Sleep(3000);
                }
            }
            /* fix this 
            else {
                Clicker::move_and_click(point.x, point.y);
            }
            */
            Sleep(1000);
        }
        Sleep(1000);
    }
    return 0;
}