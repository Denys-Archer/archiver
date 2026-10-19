#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <string>
#include <vector>
#include <fstream>

inline bool write_file(const std::string& filename, const std::vector<char>& buffer) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    file.write(buffer.data(), buffer.size());
    return true;
}

#endif
