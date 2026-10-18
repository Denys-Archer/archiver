#ifndef ARCHIVER_HPP
#define ARCHIVER_HPP

#include "keys.hpp"

class Archiver {
public:
    int run(int argc, char* argv[]);
};

// C API для використання через libarchiver.so в інших мовах (Python, C тощо)
extern "C" {
    int run_archiver(int argc, char** argv);
}

#endif
