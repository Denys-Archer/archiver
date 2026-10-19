#include "mj.hpp"
#include "compress.hpp"
#include "input.hpp"
#include "output.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <zlib.h>

namespace fs = std::filesystem;

bool MJArchive::create(const std::string& input_path, const std::string& archive_name) {
    fs::path base_path(input_path);
    if (!fs::exists(base_path)) {
        std::cerr << "Error: Input path does not exist: " << input_path << "\n";
        return false;
    }

    std::vector<std::pair<std::string, std::string>> files_to_pack;

    if (fs::is_regular_file(base_path)) {
        files_to_pack.push_back({base_path.filename().string(), input_path});
    } else if (fs::is_directory(base_path)) {
        fs::path parent = base_path.has_parent_path() ? base_path.parent_path() : ".";

        for (auto const& dir_entry : fs::recursive_directory_iterator(base_path)) {
            if (dir_entry.is_regular_file()) {
                fs::path full_p = dir_entry.path();
                fs::path rel_p = fs::relative(full_p, parent);
                files_to_pack.push_back({rel_p.generic_string(), full_p.string()});
            }
        }
    }

    std::ofstream out(archive_name, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Failed to create archive: " << archive_name << "\n";
        return false;
    }

    out.write("MJAR", 4);
    uint32_t file_count = files_to_pack.size();
    out.write(reinterpret_cast<const char*>(&file_count), sizeof(file_count));

    for (const auto& [rel_path, full_path] : files_to_pack) {
        std::vector<char> input_data;
        if (!read_file(full_path, input_data)) continue;

        std::vector<char> compressed_data;
        if (!Compressor::compress_data(input_data, compressed_data, Z_BEST_COMPRESSION)) {
            std::cerr << "Error: Compression failed for " << rel_path << "\n";
            return false;
        }

        uint16_t path_len = rel_path.size();
        out.write(reinterpret_cast<const char*>(&path_len), sizeof(path_len));
        out.write(rel_path.data(), path_len);

        uint64_t orig_size = input_data.size();
        uint64_t comp_size = compressed_data.size();
        out.write(reinterpret_cast<const char*>(&orig_size), sizeof(orig_size));
        out.write(reinterpret_cast<const char*>(&comp_size), sizeof(comp_size));
        out.write(compressed_data.data(), comp_size);
    }

    std::cout << "Success: Created MJ archive " << archive_name << " (" << file_count << " items)\n";
    return true;
}

bool MJArchive::extract(const std::string& archive_name, const std::string& output_path) {
    std::ifstream in(archive_name, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Failed to open archive: " << archive_name << "\n";
        return false;
    }

    char magic[4];
    in.read(magic, 4);
    if (std::strncmp(magic, "MJAR", 4) != 0) {
        std::cerr << "Error: Invalid MJ archive format.\n";
        return false;
    }

    uint32_t file_count = 0;
    in.read(reinterpret_cast<char*>(&file_count), sizeof(file_count));

    fs::path target_dir = output_path.empty() ? fs::path("extracted_archive") : fs::path(output_path);
    fs::create_directories(target_dir);

    for (uint32_t i = 0; i < file_count; ++i) {
        uint16_t path_len = 0;
        in.read(reinterpret_cast<char*>(&path_len), sizeof(path_len));
        std::string rel_path(path_len, '\0');
        in.read(&rel_path[0], path_len);

        uint64_t orig_size = 0, comp_size = 0;
        in.read(reinterpret_cast<char*>(&orig_size), sizeof(orig_size));
        in.read(reinterpret_cast<char*>(&comp_size), sizeof(comp_size));

        std::vector<char> compressed_data(comp_size);
        in.read(compressed_data.data(), comp_size);

        std::vector<char> output_data;
        if (!Compressor::decompress_data(compressed_data, output_data, orig_size)) {
            std::cerr << "Error: Decompression failed for " << rel_path << "\n";
            return false;
        }

        fs::path final_file_path = target_dir / rel_path;
        fs::create_directories(final_file_path.parent_path());

        if (!write_file(final_file_path.string(), output_data)) {
            std::cerr << "Error: Failed to write file: " << final_file_path << "\n";
            return false;
        }
    }

    std::cout << "Success: Extracted MJ archive to " << target_dir << "\n";
    return true;
}

bool MJArchive::list(const std::string& archive_name) {
    std::ifstream in(archive_name, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Failed to open archive: " << archive_name << "\n";
        return false;
    }

    char magic[4];
    in.read(magic, 4);
    if (std::strncmp(magic, "MJAR", 4) != 0) {
        std::cerr << "Error: Invalid MJ archive format.\n";
        return false;
    }

    uint32_t file_count = 0;
    in.read(reinterpret_cast<char*>(&file_count), sizeof(file_count));

    std::map<std::string, std::vector<std::string>> groups;

    for (uint32_t i = 0; i < file_count; ++i) {
        uint16_t path_len = 0;
        in.read(reinterpret_cast<char*>(&path_len), sizeof(path_len));
        std::string rel_path(path_len, '\0');
        in.read(&rel_path[0], path_len);

        uint64_t orig_size = 0, comp_size = 0;
        in.read(reinterpret_cast<char*>(&orig_size), sizeof(orig_size));
        in.read(reinterpret_cast<char*>(&comp_size), sizeof(comp_size));

        in.seekg(comp_size, std::ios::cur);

        fs::path p(rel_path);
        std::vector<std::string> parts;
        for (auto const& part : p) {
            parts.push_back(part.string());
        }

        for (size_t j = 0; j < parts.size(); ++j) {
            std::string parent_dir = "";
            for (size_t k = 0; k < j; ++k) {
                parent_dir = parent_dir.empty() ? parts[k] : parent_dir + "/" + parts[k];
            }
            std::string child = parts[j];

            auto& vec = groups[parent_dir];
            if (std::find(vec.begin(), vec.end(), child) == vec.end()) {
                vec.push_back(child);
            }
        }
    }

    if (groups.find("") != groups.end()) {
        std::cout << "archive files: ";
        const auto& root_items = groups[""];
        for (size_t i = 0; i < root_items.size(); ++i) {
            std::cout << root_items[i] << (i + 1 < root_items.size() ? ", " : "");
        }
        std::cout << "\n";
    }

    for (const auto& [dir_path, items] : groups) {
        if (dir_path.empty()) continue;
        std::cout << dir_path << " files: ";
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << items[i] << (i + 1 < items.size() ? ", " : "");
        }
        std::cout << "\n";
    }

    return true;
}
