
#ifndef __blockchain__blockchain_hpp
#define __blockchain__blockchain_hpp

#include "helpers.hpp"

#include "picosha2.h"

#include <cstdlib>
#include <ostream>

/*************************************************************************************************/

struct block {
    std::uint64_t idx;
    std::uint64_t timestamp;
    std::string prevsha256;
    std::string data;
    std::string sha256;
};

/*************************************************************************************************/

inline
block new_block(const std::string &prevsha256, std::uint64_t nblocks, const char *data, std::size_t size) {
    block b;
    b.idx = nblocks;
    b.timestamp = timestamp();
    b.prevsha256 = prevsha256;
    b.data.assign(data, size);
    b.sha256 = picosha2::hash256_hex_string(b.data.begin(), b.data.end());

    return b;
}

/*************************************************************************************************/

inline
std::ostream& dump(std::ostream &os, const block &b) {
    os
    << "index    = " << b.idx << std::endl
    << "timestamp= " << format_timestamp(b.timestamp) << std::endl
    << "prevhash = " << b.prevsha256 << std::endl
    << "hash     = " << b.sha256 << std::endl;

    return os;
}

/*************************************************************************************************/

#endif // __blockchain__blockchain_hpp
