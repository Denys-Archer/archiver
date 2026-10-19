#include "types.hpp"
#include "mj.hpp"
#include "cp.hpp"
#include <fstream>
#include <cstring>

std::unique_ptr<ArchiveHandler> get_handler_by_type(const std::string& type_flag) {
    if (type_flag == "-mj") return std::make_unique<MJArchive>();
    if (type_flag == "-cp") return std::make_unique<CPArchive>();
    return nullptr;
}

std::unique_ptr<ArchiveHandler> get_handler_for_file(const std::string& filename) {
    // 1. Check extension first if present
    if (filename.length() >= 3 && filename.substr(filename.length() - 3) == ".mj") {
        return std::make_unique<MJArchive>();
    }
    if (filename.length() >= 3 && filename.substr(filename.length() - 3) == ".cp") {
        return std::make_unique<CPArchive>();
    }

    // 2. Auto-detect by Magic Bytes if filename has no extension or different extension
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return nullptr;

    char magic[4];
    in.read(magic, 4);
    if (in) {
        if (std::strncmp(magic, "MJAR", 4) == 0) return std::make_unique<MJArchive>();
        if (std::strncmp(magic, "CPAR", 4) == 0) return std::make_unique<CPArchive>();
    }

    return nullptr;
}
