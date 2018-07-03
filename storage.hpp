
#ifndef __blockchain__storage_hpp
#define __blockchain__storage_hpp

#include "helpers.hpp"
#include "blockchain.hpp"

#include <cstdio>
#include <cassert>

/*************************************************************************************************/

struct storage {
    storage(const char *fname)
        :m_file{std::fopen(fname, "a+b")}
    {
        if ( !m_file ) {
            throw std::runtime_error("can't open/create file");
        }

        std::setvbuf(m_file, nullptr, _IONBF, 0); // unbuffered IO
    }
    ~storage() {
        if ( m_file ) {
            std::fclose(m_file);
        }
    }

    bool empty() const {
        std::uint64_t size = fsize();

        return size == 0;
    }

    bool at_end() const {
        std::uint64_t size = fsize();
        std::uint64_t fpos = std::ftell(m_file);

        return size == fpos;
    }

    std::uint64_t blocks() {
        if ( empty() ) {
            return 0;
        }

        std::uint64_t n{};

        seek_to_begin();

        block b;
        do {
            b = read_block();
            ++n;
        } while ( !at_end() );

        return n;
    }
    block last_block(std::uint64_t *n = nullptr) {
        seek_to_begin();

        block b;
        do {
            b = read_block();
            if ( n ) {
                ++(*n);
            }
        } while ( !at_end() );

        return b;
    }

    void add(const block &b) {
        write_block(b);
    }

    block get(bool *ok, std::uint64_t idx) {
        block b;
        if ( empty() ) {
            *ok = false;
            return b;
        }

        b = first();
        if ( b.idx == idx ) {
            *ok = true;
            return b;
        }

        do {
            b = next();
            if ( b.idx == idx ) {
                *ok = true;
                return b;
            }
        } while ( !at_end() );

        *ok = false;

        return b;
    }
    block get(bool *ok, const std::string &hash) {
        block b;
        if ( empty() ) {
            *ok = false;
            return b;
        }

        b = first();
        if ( b.sha256 == hash ) {
            *ok = true;
            return b;
        }

        do {
            b = next();
            if ( b.sha256 == hash ) {
                *ok = true;
                return b;
            }
        } while ( !at_end() );

        *ok = false;

        return b;
    }

    block first() {
        seek_to_begin();
        block b = read_block();

        return b;
    }
    block next() {
        block b = read_block();

        return b;
    }

    bool recheck(std::uint64_t *bad_idx) {
        if ( empty() ) {
            return true;
        }

        block b = first();
        if ( b.sha256 != picosha2::hash256_hex_string(b.data.begin(), b.data.end()) ) {
            *bad_idx = b.idx;
            return false;
        }

        std::uint64_t pidx = b.idx;
        std::string phash = b.sha256;

        do {
            b = next();
            if ( b.sha256 != picosha2::hash256_hex_string(b.data.begin(), b.data.end()) ) {
                *bad_idx = b.idx;
                return false;
            }
            if ( pidx+1 != b.idx ) {
                *bad_idx = b.idx;
                return false;
            }
            if ( phash != b.prevsha256 ) {
                *bad_idx = b.idx;
                return false;
            }

            pidx = b.idx;
            phash = b.sha256;
        } while ( !at_end() );

        return true;
    }

private:
    std::uint64_t fsize() const {
        std::uint64_t ppos = std::ftell(m_file);
        std::fseek(m_file, 0, SEEK_END);
        std::uint64_t pos = std::ftell(m_file);
        std::fseek(m_file, ppos, SEEK_SET);

        return pos;
    }
    void seek_to_begin() {
        std::fseek(m_file, 0, SEEK_SET);
    }
    void seek_to_end() {
        std::fseek(m_file, 0, SEEK_END);
    }
    bool seek_to(std::uint64_t pos) {
        std::fseek(m_file, pos, SEEK_SET);
    }

    void write_string(const std::string &s) {
        std::uint32_t size = s.size();
        assert(std::fwrite(&size, 1, sizeof(size), m_file) == sizeof(size));
        assert(std::fwrite(s.data(), 1, size, m_file) == size);
    }
    void write_block(const block &b) {
        seek_to_end();

        assert(std::fwrite(&b.idx, 1, sizeof(b.idx), m_file) == sizeof(b.idx));
        assert(std::fwrite(&b.timestamp, 1, sizeof(b.timestamp), m_file) == sizeof(sizeof(b.timestamp)));
        write_string(b.prevsha256);
        write_string(b.data);
        write_string(b.sha256);
    }

    bool skeep_block() {
        enum { hdr = sizeof(block::idx)+sizeof(block::timestamp) };
    }

    std::string read_string() {
        std::uint32_t size{};
        assert(std::fread(&size, 1, sizeof(size), m_file) == sizeof(size));
        std::string s;
        s.resize(size);
        assert(std::fread(const_cast<char *>(s.data()), 1, size, m_file) == size);

        return s;
    }
    block read_block() {
        block b;

        assert(std::fread(&b.idx, 1, sizeof(b.idx), m_file) == sizeof(b.idx));
        assert(std::fread(&b.timestamp, 1, sizeof(b.timestamp), m_file) == sizeof(b.timestamp));
        b.prevsha256 = read_string();
        b.data = read_string();
        b.sha256 = read_string();

        return b;
    }

private:
    std::FILE *m_file;
};

/*************************************************************************************************/

#endif // __blockchain__storage_hpp
