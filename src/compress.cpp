#include "compress.hpp"
#include <zlib.h>
#include <vector>
#include <iostream>

namespace Compressor {
    bool compress_data(const std::vector<char>& input, std::vector<char>& output, int level) {
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        if (deflateInit(&strm, level) != Z_OK) return false;

        strm.avail_in = input.size();
        strm.next_in = (Bytef*)input.data();

        int ret;
        char buffer[32768];

        do {
            strm.avail_out = sizeof(buffer);
            strm.next_out = (Bytef*)buffer;
            ret = deflate(&strm, Z_FINISH);
            
            if (ret == Z_STREAM_ERROR) {
                deflateEnd(&strm);
                return false;
            }
            
            size_t have = sizeof(buffer) - strm.avail_out;
            output.insert(output.end(), buffer, buffer + have);
        } while (strm.avail_out == 0);

        deflateEnd(&strm);
        return true;
    }

    bool decompress_data(const std::vector<char>& input, std::vector<char>& output, size_t original_size) {
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        if (inflateInit(&strm) != Z_OK) return false;

        strm.avail_in = input.size();
        strm.next_in = (Bytef*)input.data();

        output.resize(original_size);
        strm.avail_out = original_size;
        strm.next_out = (Bytef*)output.data();

        int ret = inflate(&strm, Z_FINISH);
        inflateEnd(&strm);

        return (ret == Z_STREAM_END);
    }
}
