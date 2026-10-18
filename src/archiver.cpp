#include "archiver.hpp"
#include "types.hpp"
#include <iostream>

bool ends_with(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void print_help(const char* program_name) {
    std::cout << "Usage:\n";
    std::cout << "  Compress to MJ:  " << program_name << " -mj <input_file/dir> <archive.mj>\n";
    std::cout << "  Compress to CP:  " << program_name << " -cp <input_file/dir> <archive.cp>\n";
    std::cout << "  Extract archive: " << program_name << " -x <archive_name> [output_dir]\n";
    std::cout << "  List archive:    " << program_name << " -l <archive_name>\n";
    std::cout << "  Show version:    " << program_name << " -v | --version\n";
    std::cout << "  Show help:       " << program_name << " -h | --help\n";
}

int Archiver::run(int argc, char* argv[]) {
    Options opts = parse_arguments(argc, argv);

    if (opts.op == Operation::Version) {
        std::cout << "Archiver version 0.1\n";
        std::cout << "License: GNU GPL v3.0\n";
        std::cout << "Read more about the license: https://www.gnu.org/licenses/gpl-3.0.html\n";
        return 0;
    }

    if (opts.op == Operation::Help || opts.op == Operation::Unknown) {
        print_help(argv[0]);
        return (opts.op == Operation::Unknown) ? 1 : 0;
    }

    if (opts.op == Operation::CompressMJ) {
        if (!ends_with(opts.archive_path, ".mj")) {
            std::cerr << "Error: MJ archive name must end with '.mj'\n";
            return 1;
        }
        auto handler = get_handler_by_type("-mj");
        if (!handler->create(opts.input_path, opts.archive_path)) return 1;
    } else if (opts.op == Operation::CompressCP) {
        if (!ends_with(opts.archive_path, ".cp")) {
            std::cerr << "Error: CP archive name must end with '.cp'\n";
            return 1;
        }
        auto handler = get_handler_by_type("-cp");
        if (!handler->create(opts.input_path, opts.archive_path)) return 1;
    } else if (opts.op == Operation::Extract) {
        auto handler = get_handler_for_file(opts.archive_path);
        if (!handler) {
            std::cerr << "Error: Unknown archive format or file not found.\n";
            return 1;
        }
        if (!handler->extract(opts.archive_path, opts.output_path)) return 1;
    } else if (opts.op == Operation::List) {
        auto handler = get_handler_for_file(opts.archive_path);
        if (!handler) {
            std::cerr << "Error: Unknown archive format or file not found.\n";
            return 1;
        }
        if (!handler->list(opts.archive_path)) return 1;
    }

    return 0;
}

extern "C" {
    int run_archiver(int argc, char** argv) {
        Archiver archiver;
        return archiver.run(argc, argv);
    }
}

int main(int argc, char* argv[]) {
    Archiver archiver;
    return archiver.run(argc, argv);
}
