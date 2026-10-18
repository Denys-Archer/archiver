#ifndef MJ_HPP
#define MJ_HPP

#include "types.hpp"

class MJArchive : public ArchiveHandler {
public:
    bool create(const std::string& input_file, const std::string& archive_name) override;
    bool extract(const std::string& archive_name, const std::string& output_file) override;
    bool list(const std::string& archive_name) override;
};

#endif
