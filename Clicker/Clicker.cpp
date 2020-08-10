#include "Clicker.h"
#include <vector>

const int Clicker::large_buffer = 5000;
const int Clicker::string_buffer = 200;
const unsigned int Clicker::buffer = 50;

POINT Clicker::get_cursor_pos() {
    POINT point;
    GetCursorPos(&point);
    return point;
}

POINT Clicker::scan_area(int x_min, int x_max, int y_min, int y_max, const char* color, int debug) {
    POINT point = {};
    std::string hex_color;

    for (int i = x_min; i <= x_max; i++) {
        for (int j = y_min; j <= y_max; j++) {
            hex_color = Clicker::get_hex_color(i, j);
            if (hex_color.compare(color) == 0) {
                point.x = i;
                point.y = j;
                return point;
            }
        }
    }
    return point;
}

POINT Clicker::rect_to_point(RECT rect) {
    POINT point = {};
    point.x = rect.right - rect.left;
    point.y = rect.bottom - rect.top;
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

std::string Clicker::colorref_to_color(COLORREF colorref) {
    int r = GetRValue(colorref);
    int g = GetGValue(colorref);
    int b = GetBValue(colorref);
    std::ostringstream oss;
    oss << "0x" << std::hex << r << std::hex << g << std::hex << b;
    return oss.str();
}

std::string Clicker::get_window_name() {
    HWND hwnd = GetForegroundWindow();
    char* temp_name = new char[100];
    GetWindowTextA(hwnd, temp_name, 100);
    std::string window_name(temp_name);
    return window_name;
}

std::string Clicker::get_hex_color(int x, int y) {
    COLORREF colorref = Clicker::get_pixel_screen(x, y);
    return colorref_to_color(colorref);
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

void Clicker::client_to_screen(POINT* point) {
    HWND hwnd = GetForegroundWindow();
    ClientToScreen(hwnd, point);
}

void Clicker::log_mouse(int timeout, int sleep_ms) {
    clock_t start = clock();
    POINT point;
    std::string hex_color;

    while (Clicker::check_timeout(start, timeout) == 0) {
        point = Clicker::get_cursor_pos();
        hex_color = Clicker::get_hex_color(point.x, point.y);
        std::cout << "(" << point.x << ", " << point.y << "): " << hex_color << std::endl;
        Sleep(sleep_ms);
    }
}

int Clicker::wait_for_pixel(int x, int y, const char* color, int timeout, int sleep_ms) {
    const clock_t start = clock();
    std::string hex_color;

    while (true) {
        hex_color = get_hex_color(x, y);
        if (hex_color.compare(color) == 0) {
            return 1;
        }
        if (timeout) {
            if (Clicker::check_timeout(start, timeout)) {
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

RECT Clicker::get_window_rect_foreground() {
    RECT rect;
    HWND hwnd = GetForegroundWindow();
    GetWindowRect(hwnd, &rect);
    return rect;
}

void Clicker::log_area(int x_min, int x_max, int y_min, int y_max, const char* file_name) {
    COLORREF colorref;
    int buffer = (x_max - x_min) * (y_max - y_min);
    std::string hex_color;
    std::vector<std::string> logs;
    std::ostringstream oss;

    for (int i = x_min; i < x_max; i++) {
        for (int j = y_min; j < y_max; j++) {
            colorref = Clicker::get_pixel_screen(i, j);
            hex_color = Clicker::colorref_to_color(colorref);
            oss << "x: " << i << ", y: " << j << ", color: " << hex_color << std::endl;
            logs.push_back(oss.str());
        }
    }
    FileIO::write_string_vector(file_name, logs, buffer);
}