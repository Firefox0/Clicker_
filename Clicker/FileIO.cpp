#include "FileIO.h"

POINT FileIO::read_from_file(const char* file_name) {
    POINT point = {};
    std::ifstream file(file_name);
    if (!file.good()) {
        return point;
    }
    std::string line;
    getline(file, line);
    point.x = (int)strtol(line.c_str(), nullptr, 10);
    getline(file, line);
    point.y = (int)strtol(line.c_str(), nullptr, 10);
    file.close();
    return point;
}

void FileIO::write_pos(const char* file_name) {
    POINT point = Clicker::get_cursor_pos();
    char* output = new char[Clicker::buffer];
    sprintf_s(output, sizeof(char) * Clicker::buffer, "%i\n%i\n", point.x, point.y);
    FileIO::write_to_file(file_name, output);
    delete[] output;
}

void FileIO::write_string_vector(const char* file_name, std::vector<std::string>&array, int length) {
    std::ofstream file(file_name);
    for (int i = 0; i < length; i++) {
        file << array[i];
    }
    file.close();
}

void FileIO::write_to_file(const char* file_name, char* output) {
    std::ofstream file;
    file.open(file_name);
    file << output;
    file.close();
}
