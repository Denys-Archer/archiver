#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>
#include <vector>
#include <fstream>

inline bool read_file(const std::string& filename, std::vector<char>& buffer) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer.resize(size);
    if (!file.read(buffer.data(), size)) return false;
    return true;
}

#endif
