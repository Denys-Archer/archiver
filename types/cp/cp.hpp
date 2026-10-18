#ifndef CP_HPP
#define CP_HPP

#include "types.hpp"

class CPArchive : public ArchiveHandler {
public:
    bool create(const std::string& input_file, const std::string& archive_name) override;
    bool extract(const std::string& archive_name, const std::string& output_file) override;
    bool list(const std::string& archive_name) override;
};

#endif
