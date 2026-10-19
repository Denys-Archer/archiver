#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <memory>

class ArchiveHandler {
public:
    virtual ~ArchiveHandler() = default;
    virtual bool create(const std::string& input_file, const std::string& archive_name) = 0;
    virtual bool extract(const std::string& archive_name, const std::string& output_path) = 0;
    virtual bool list(const std::string& archive_name) = 0;
};

std::unique_ptr<ArchiveHandler> get_handler_for_file(const std::string& filename);
std::unique_ptr<ArchiveHandler> get_handler_by_type(const std::string& type_flag);

#endif
