#include "compress.hpp"
#include <zlib.h>

namespace Compressor {
    bool compress_data(const std::vector<char>& input, std::vector<char>& output, int level) {
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        
        if (deflateInit(&strm, level) != Z_OK) return false;

        uLongf max_size = compressBound(input.size());
        output.resize(max_size);

        strm.next_in = (Bytef*)input.data();
        strm.avail_in = input.size();
        strm.next_out = (Bytef*)output.data();
        strm.avail_out = max_size;

        int ret = deflate(&strm, Z_FINISH);
        if (ret != Z_STREAM_END) {
            deflateEnd(&strm);
            return false;
        }

        output.resize(strm.total_out);
        deflateEnd(&strm);
        return true;
    }

    bool decompress_data(const std::vector<char>& input, std::vector<char>& output, size_t original_size) {
        output.resize(original_size);
        uLongf dest_size = original_size;
        int res = uncompress((Bytef*)output.data(), &dest_size, (const Bytef*)input.data(), input.size());
        if (res != Z_OK) return false;
        return true;
    }
}
