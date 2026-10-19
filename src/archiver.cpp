#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <zlib.h>

#define CHUNK 16384
#define MAGIC_MJ "MJAR"
#define MAGIC_CP "CPAR"

// Виведення чистої довідки без довгих шляхів
void print_help() {
    std::cout << "Usage:\n"
              << "  Compress to MJ:  archiver -mj <input_file/dir> <archive.mj>\n"
              << "  Compress to CP:  archiver -cp <input_file/dir> <archive.cp>\n"
              << "  Extract archive: archiver -x <archive_name> [output_dir]\n"
              << "  List archive:    archiver -l <archive_name>\n"
              << "  Show version:    archiver -v | --version\n"
              << "  Show help:       archiver -h | --help\n";
}

void print_version() {
    std::cout << "archiver v1.1.0 (GPL v3.0)\n"
              << "High-performance C++ Archiver with zlib backend.\n";
}

// Стиснення потоку з використанням zlib
bool compress_stream(std::ifstream& in, std::ofstream& out, int level) {
    z_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    
    if (deflateInit(&strm, level) != Z_OK) return false;

    std::vector<char> in_buffer(CHUNK);
    std::vector<char> out_buffer(CHUNK);

    do {
        in.read(in_buffer.data(), CHUNK);
        strm.avail_in = in.gcount();
        if (in.bad()) {
            deflateEnd(&strm);
            return false;
        }
        strm.next_in = reinterpret_cast<Bytef*>(in_buffer.data());

        bool flush = in.eof();
        do {
            strm.avail_out = CHUNK;
            strm.next_out = reinterpret_cast<Bytef*>(out_buffer.data());
            
            if (deflate(&strm, flush ? Z_FINISH : Z_NO_FLUSH) == Z_STREAM_ERROR) {
                deflateEnd(&strm);
                return false;
            }
            
            size_t have = CHUNK - strm.avail_out;
            out.write(out_buffer.data(), have);
            if (out.bad()) {
                deflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);

    } while (!in.eof());

    deflateEnd(&strm);
    return true;
}

// Розпакування потоку
bool decompress_stream(std::ifstream& in, std::ofstream& out) {
    z_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    
    if (inflateInit(&strm) != Z_OK) return false;

    std::vector<char> in_buffer(CHUNK);
    std::vector<char> out_buffer(CHUNK);

    do {
        in.read(in_buffer.data(), CHUNK);
        strm.avail_in = in.gcount();
        if (in.bad()) {
            inflateEnd(&strm);
            return false;
        }
        if (strm.avail_in == 0) break;
        strm.next_in = reinterpret_cast<Bytef*>(in_buffer.data());

        do {
            strm.avail_out = CHUNK;
            strm.next_out = reinterpret_cast<Bytef*>(out_buffer.data());
            
            int ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    return false;
            }
            
            size_t have = CHUNK - strm.avail_out;
            out.write(out_buffer.data(), have);
            if (out.bad()) {
                inflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);

    } while (true);

    inflateEnd(&strm);
    return true;
}

// Функція архівації
bool create_archive(const std::string& input_path, const std::string& archive_path, bool max_compression) {
    std::ifstream in(input_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Cannot open input file: " << input_path << "\n";
        return false;
    }

    std::ofstream out(archive_path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Cannot create archive file: " << archive_path << "\n";
        return false;
    }

    // Записуємо сигнатуру (магічні байти)
    if (max_compression) {
        out.write(MAGIC_MJ, 4);
    } else {
        out.write(MAGIC_CP, 4);
    }

    // Зберігаємо довжину і ім'я оригінального файлу для відновлення
    uint32_t name_len = static_cast<uint32_t>(input_path.size());
    out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
    out.write(input_path.c_str(), name_len);

    int level = max_compression ? Z_BEST_COMPRESSION : Z_BEST_SPEED;
    bool success = compress_stream(in, out, level);

    return success;
}

// Функція вилучення
bool extract_archive(const std::string& archive_path, const std::string& output_dir) {
    std::ifstream in(archive_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Cannot open archive: " << archive_path << "\n";
        return false;
    }

    char magic[5] = {0};
    in.read(magic, 4);

    if (std::string(magic, 4) != MAGIC_MJ && std::string(magic, 4) != MAGIC_CP) {
        std::cerr << "Error: Invalid archive format (bad magic bytes).\n";
        return false;
    }

    uint32_t name_len = 0;
    in.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

    std::vector<char> name_buf(name_len + 1, 0);
    in.read(name_buf.data(), name_len);
    std::string original_name(name_buf.data());

    std::string out_path = output_dir.empty() ? original_name : output_dir + "/" + original_name;

    std::ofstream out(out_path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Cannot create output file: " << out_path << "\n";
        return false;
    }

    return decompress_stream(in, out);
}

// Список вмісту архіву
bool list_archive(const std::string& archive_path) {
    std::ifstream in(archive_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Cannot open archive: " << archive_path << "\n";
        return false;
    }

    char magic[5] = {0};
    in.read(magic, 4);

    if (std::string(magic, 4) != MAGIC_MJ && std::string(magic, 4) != MAGIC_CP) {
        std::cerr << "Error: Invalid archive format.\n";
        return false;
    }

    uint32_t name_len = 0;
    in.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

    std::vector<char> name_buf(name_len + 1, 0);
    in.read(name_buf.data(), name_len);
    std::string original_name(name_buf.data());

    std::cout << original_name << "\n";
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    std::string flag = argv[1];

    if (flag == "-h" || flag == "--help") {
        print_help();
        return 0;
    }

    if (flag == "-v" || flag == "--version") {
        print_version();
        return 0;
    }

    if (flag == "-mj" && argc == 4) {
        if (create_archive(argv[2], argv[3], true)) {
            std::cout << "Successfully compressed to MJ format.\n";
            return 0;
        }
        return 1;
    }

    if (flag == "-cp" && argc == 4) {
        if (create_archive(argv[2], argv[3], false)) {
            std::cout << "Successfully compressed to CP format.\n";
            return 0;
        }
        return 1;
    }

    if (flag == "-x" && (argc == 3 || argc == 4)) {
        std::string out_dir = (argc == 4) ? argv[3] : "";
        if (extract_archive(argv[2], out_dir)) {
            std::cout << "Successfully extracted archive.\n";
            return 0;
        }
        return 1;
    }

    if (flag == "-l" && argc == 3) {
        if (list_archive(argv[2])) {
            return 0;
        }
        return 1;
    }

    std::cerr << "Error: Invalid arguments or flags.\n";
    print_help();
    return 1;
}
