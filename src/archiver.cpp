#include <iostream>
#include <string>
#include "mj.hpp"
#include "cp.hpp"
#include "ca.hpp"

void print_help() {
    std::cout << "Usage:\n";
    std::cout << "  bavovna-archiver -mj <input_file> <archive.mj>    Compress to MJ\n";
    std::cout << "  bavovna-archiver -cp <input_file> <archive.cp>    Compress to CP\n";
    std::cout << "  bavovna-archiver -ca <input_file> <archive.ca>    Compress to CA (balanced)\n";
    std::cout << "  bavovna-archiver -x <archive_name> [output_dir]   Extract archive\n";
    std::cout << "  bavovna-archiver -l <archive_name>                List archive contents\n";
    std::cout << "  bavovna-archiver -h | --help                      Show help\n";
    std::cout << "  bavovna-archiver -v | --version                   Show version\n";
}

void print_version() {
    std::cout << "bavovna-archiver version 1.5.6\n";
    std::cout << "Copyright (C) 2026 Denys Laptiev\n";
    std::cout << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
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

    // Стиснення: -mj, -cp, -ca
    if (flag == "-mj" || flag == "-cp" || flag == "-ca") {
        if (argc < 4) {
            std::cerr << "Error: Missing input or output file arguments.\n";
            print_help();
            return 1;
        }

        std::string input = argv[2];
        std::string output = argv[3];
        bool success = false;

        if (flag == "-mj") {
            MJArchive mj;
            success = mj.create(input, output);
        } else if (flag == "-cp") {
            CPArchive cp;
            success = cp.create(input, output);
        } else if (flag == "-ca") {
            CAArchive ca;
            success = ca.create(input, output);
        }

        if (!success) {
            std::cerr << "Error: Compression failed for type " << flag << "\n";
            return 1;
        }
    } 
    // Розпакування: -x
    else if (flag == "-x") {
        if (argc < 3) {
            std::cerr << "Error: Missing archive name for extraction.\n";
            return 1;
        }

        std::string archive = argv[2];
        std::string out_dir = (argc >= 4) ? argv[3] : ".";
        bool success = false;

        if (archive.rfind(".mj") != std::string::npos) {
            MJArchive mj;
            success = mj.extract(archive, out_dir);
        } else if (archive.rfind(".cp") != std::string::npos) {
            CPArchive cp;
            success = cp.extract(archive, out_dir);
        } else if (archive.rfind(".ca") != std::string::npos) {
            CAArchive ca;
            success = ca.extract(archive, out_dir);
        } else {
            MJArchive mj;
            if (mj.extract(archive, out_dir)) {
                success = true;
            } else {
                CPArchive cp;
                if (cp.extract(archive, out_dir)) {
                    success = true;
                } else {
                    CAArchive ca;
                    success = ca.extract(archive, out_dir);
                }
            }
        }

        if (!success) {
            std::cerr << "Error: Extraction failed\n";
            return 1;
        }
    } 
    // Перегляд вмісту: -l
    else if (flag == "-l") {
        if (argc < 3) {
            std::cerr << "Error: Missing archive name for listing.\n";
            return 1;
        }

        std::string archive = argv[2];
        bool success = false;

        if (archive.rfind(".mj") != std::string::npos) {
            MJArchive mj;
            success = mj.list(archive);
        } else if (archive.rfind(".cp") != std::string::npos) {
            CPArchive cp;
            success = cp.list(archive);
        } else if (archive.rfind(".ca") != std::string::npos) {
            CAArchive ca;
            success = ca.list(archive);
        } else {
            MJArchive mj;
            if (mj.list(archive)) {
                success = true;
            } else {
                CPArchive cp;
                if (cp.list(archive)) {
                    success = true;
                } else {
                    CAArchive ca;
                    success = ca.list(archive);
                }
            }
        }

        if (!success) {
            std::cerr << "Error: Could not list archive contents\n";
            return 1;
        }
    } 
    else {
        std::cerr << "Unknown flag: " << flag << "\n";
        print_help();
        return 1;
    }

    std::cout << "Done successfully!\n";
    return 0;
}
