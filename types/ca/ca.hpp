#ifndef CA_HPP
#define CA_HPP

#include <string>

class CAArchive {
public:
    bool create(const std::string& input_path, const std::string& output_path);
    bool extract(const std::string& archive_path, const std::string& output_dir);
    bool list(const std::string& archive_path);
};

#endif
