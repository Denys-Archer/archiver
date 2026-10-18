#ifndef COMPRESS_HPP
#define COMPRESS_HPP

#include <vector>

namespace Compressor {
    bool compress_data(const std::vector<char>& input, std::vector<char>& output, int level);
    bool decompress_data(const std::vector<char>& input, std::vector<char>& output, size_t original_size);
}

#endif
