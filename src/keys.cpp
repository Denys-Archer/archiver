#include "keys.hpp"

Options parse_arguments(int argc, char* argv[]) {
    Options opts;
    if (argc < 2) return opts;

    std::string flag = argv[1];
    if (flag == "-mj") {
        opts.op = Operation::CompressMJ;
        if (argc >= 3) opts.input_path = argv[2];
        if (argc >= 4) opts.archive_path = argv[3];
    } else if (flag == "-cp") {
        opts.op = Operation::CompressCP;
        if (argc >= 3) opts.input_path = argv[2];
        if (argc >= 4) opts.archive_path = argv[3];
    } else if (flag == "-x") {
        opts.op = Operation::Extract;
        if (argc >= 3) opts.archive_path = argv[2];
        if (argc >= 4) opts.output_path = argv[3];
    } else if (flag == "-l") {
        opts.op = Operation::List;
        if (argc >= 3) opts.archive_path = argv[2];
    } else if (flag == "-v" || flag == "--version") {
        opts.op = Operation::Version;
    } else if (flag == "-h" || flag == "--help") {
        opts.op = Operation::Help;
    }
    return opts;
}
