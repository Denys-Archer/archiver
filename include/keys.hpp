#ifndef KEYS_HPP
#define KEYS_HPP

#include <string>

enum class Operation {
    CompressMJ,
    CompressCP,
    Extract,
    List,
    Version,
    Help,
    Unknown
};

struct Options {
    Operation op = Operation::Unknown;
    std::string input_path;
    std::string archive_path;
    std::string output_path;
};

Options parse_arguments(int argc, char* argv[]);

#endif
