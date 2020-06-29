#include "Clicker.h"

using namespace std;

const int Clicker::large_buffer = 5000;
const int Clicker::string_buffer = 200;
const unsigned int Clicker::buffer = 50;

POINT Clicker::get_cursor_pos() {
    POINT point;
    GetCursorPos(&point);
    return point;
}

COLORREF Clicker::get_pixel_screen(int x, int y) {
    HDC hdc = GetWindowDC(NULL);
    COLORREF colorref = GetPixel(hdc, x, y);
    ReleaseDC(NULL, hdc);
    return colorref;
}

COLORREF Clicker::get_pixel_foreground(int x, int y) {
    HWND hwnd = GetForegroundWindow();
    HDC hdc = GetWindowDC(hwnd);
    COLORREF colorref = GetPixel(hdc, x, y);
    ReleaseDC(hwnd, hdc);
    return colorref;
}

char* Clicker::colorref_to_color(COLORREF colorref) {
    char* hex_color = new char[Clicker::buffer];
    sprintf_s(hex_color, sizeof(char) * Clicker::buffer, "0x%x%x%x", GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
    return hex_color;
}

char* Clicker::get_window_name() {
    HWND hwnd = GetForegroundWindow();
    char* window_name = new char[100];
    GetWindowTextA(hwnd, window_name, 100);
    return window_name;
}

void Clicker::click_release_left() {
    INPUT* input = new INPUT;
    input->type = INPUT_MOUSE;
    input->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, input, sizeof(*input));
    input->mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, input, sizeof(*input));
    delete input;
}

void Clicker::move_and_click(int x, int y, unsigned int restore) {
    POINT point;
    if (restore) {
        GetCursorPos(&point);
    }
    SetCursorPos(x, y);
    Clicker::click_release_left();
    if (restore) {
        SetCursorPos(point.x, point.y);
    }
}

char* Clicker::get_hex_color(int x, int y) {
    COLORREF colorref = Clicker::get_pixel_screen(x, y);
    return colorref_to_color(colorref);
}

int Clicker::wait_for_pixel(int x, int y, const char* color, int timeout, int sleep_ms) {
    const clock_t start = clock();
    char* hex_color = new char[Clicker::buffer];
    while (true) {
        hex_color = get_hex_color(x, y);
        if (strcmp(hex_color, color) == 0) {
            delete[] hex_color;
            return 1;
        }
        if (timeout) {
            if (Clicker::check_timeout(start, timeout)) {
                delete[] hex_color;
                return 0;
            }
        }
        Sleep(sleep_ms);
    }
}

int Clicker::wait_and_click(int x, int y, const char* color, int timeout, int sleep_ms) {
    int i = Clicker::wait_for_pixel(x, y, color, timeout, sleep_ms);
    if (i) {
        Clicker::move_and_click(x, y);
    }
    return i;
}

void Clicker::client_to_screen(POINT* point) {
    HWND hwnd = GetForegroundWindow();
    ClientToScreen(hwnd, point);
}

POINT Clicker::scan_area(int x_min, int x_max, int y_min, int y_max, const char* color, int debug) {
    POINT point = {};
    char* hex_color = new char[Clicker::buffer];

    for (int i = x_min; i <= x_max; i++) {
        for (int j = y_min; j <= y_max; j++) {
            hex_color = Clicker::get_hex_color(i, j);
            if (strcmp(hex_color, color) == 0) {
                delete[] hex_color;
                point.x = i;
                point.y = j;
                return point;
            }
        }
    }
    delete[] hex_color;
    return point;
}

RECT Clicker::get_window_rect_foreground() {
    RECT rect;
    HWND hwnd = GetForegroundWindow();
    GetWindowRect(hwnd, &rect);
    return rect;
}

POINT Clicker::rect_to_point(RECT rect) {
    POINT point = {};
    point.x = rect.right - rect.left;
    point.y = rect.bottom - rect.top;
    return point;
}

int Clicker::check_rect_bounds(RECT rect, POINT point) {
    POINT rect_size = Clicker::rect_to_point(rect); 
    int in_bounds = 0;
    if (point.x <= rect_size.x && point.y <= rect_size.y) {
        in_bounds = 1;
    }
    return in_bounds;
}

int Clicker::check_timeout(clock_t start, int timeout) {
    int timeouted = 0;
    if ((clock() - start) / CLOCKS_PER_SEC > timeout) {
        timeouted = 1;
    }
    return timeouted;
}

void Clicker::log_mouse(int timeout) {
    clock_t start = clock();
    POINT point;
    char* hex_color = new char[8];
    while (Clicker::check_timeout(start, timeout)) {
        point = Clicker::get_cursor_pos();
        hex_color = Clicker::get_hex_color(point.x, point.y);
        printf("(%i, %i): %s\n", point.x, point.y, hex_color);
    }
}

void Clicker::log_area(int x_min, int x_max, int y_min, int y_max, const char* file_name) {
    COLORREF colorref;
    int buffer = (x_max - x_min) * (y_max - y_min);
    char* hex_color = new char[Clicker::buffer];
    char** logs = new char* [buffer];
    for (int a = 0; a < buffer; a++) {
        logs[a] = new char[Clicker::string_buffer];
    }
    int counter = 0;
    for (int i = x_min; i < x_max; i++) {
        for (int j = y_min; j < y_max; j++) {
            char* log = new char[Clicker::string_buffer];
            colorref = Clicker::get_pixel_screen(i, j);
            hex_color = Clicker::colorref_to_color(colorref);
            sprintf_s(log, sizeof(char) * Clicker::string_buffer, "x: %i, y: %i, color: %s\n", i, j, hex_color);
            strcpy_s(logs[counter], sizeof(char) * Clicker::string_buffer, log);
            counter++;
            delete[] log;
        }
    }
    delete[] hex_color;
    FileIO::write_2d_char_array(file_name, logs, buffer);
    for (int a = 0; a < buffer; a++) {
        delete[] logs[a];
    }
    delete[] logs;
}