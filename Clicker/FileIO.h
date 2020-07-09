#pragma once

#include "Clicker.h"

#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>

class FileIO {
public:
	static POINT read_from_file(const char* file_name);
	static void write_pos(const char* file_name);
	static void write_string_vector(const char* file_name, std::vector<std::string> &array, int length);
	static void write_to_file(const char* file_name, char* output);
};