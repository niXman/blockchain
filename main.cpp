
#include "blockchain.hpp"
#include "storage.hpp"

#include <cstring>

#include <iostream>

/*************************************************************************************************/

void usage(const char *argv0) {
    const char *p = std::strrchr(argv0, '/');
    p = p ? p+1 : p;

    std::cout
    << "usage:" << std::endl
    << "  " << p << " a|i|h|r" << std::endl
    << "    a \"some string\" - add block" << std::endl
    << "    i <idx> - get by idx" << std::endl
    << "    h <hash> - get by block hash" << std::endl
    << "    r - recheck blockchain" << std::endl
    << "    d - dump blockchain" << std::endl;
}

/*************************************************************************************************/

void add_block(storage &st, const std::string &data) {
    block b{};
    if ( st.empty() ) {
        b = new_block("", 0, data.data(), data.size());
        st.add(b);
    } else {
        std::uint64_t num{};
        block last = st.last_block(&num);
        b = new_block(last.sha256, num, data.data(), data.size());
        st.add(b);
    }
}

bool get_by_idx(storage &st, std::uint64_t idx) {
    bool ok{};
    block b = st.get(&ok, idx);
    if ( !ok ) {
        return ok;
    }

    dump(std::cout, b);
}

bool get_by_hash(storage &st, const std::string &hash) {
    bool ok{};
    block b = st.get(&ok, hash);
    if ( !ok ) {
        return ok;
    }

    dump(std::cout, b);
}

bool recheck(std::uint64_t *bad_idx, storage &st) {
    return st.recheck(bad_idx);
}

void dump(storage &st) {
    if ( !st.empty() ) {
        block b = st.first();
        dump(std::cout, b);

        while ( !st.at_end() ) {
            b = st.next();

            std::cout << "/*********************************************************************/" << std::endl;
            dump(std::cout, b);
        }
    }
}

/*************************************************************************************************/

int main(int argc, char **argv) {
    if ( argc == 1 || argv[1] == std::string("help") ) {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    storage storage("blockchain.dat");

    const char arg = argv[1][0];
    switch ( arg ) {
        case 'a': {
            std::string data;
            for ( auto i = 2u; ; ++i ) {
                const char *p = argv[i];
                if ( !p ) break;

                if ( i == 2 ) {
                    data += p;
                } else {
                    data += " ";
                    data += p;
                }
            }

            add_block(storage, data);

            break;
        }

        case 'i': {
            std::uint64_t idx = std::stoul(argv[2]);

            bool ok = get_by_idx(storage, idx);
            if ( !ok ) {
                std::cout << "bad index!" << std::endl;

                return EXIT_FAILURE;
            }

            break;
        }

        case 'h': {
            std::string hash = argv[2];

            bool ok = get_by_hash(storage, hash);
            if ( !ok ) {
                std::cout << "bad hash!" << std::endl;

                return EXIT_FAILURE;
            }

            break;
        }

        case 'r': {
            std::uint64_t bad_idx{};
            bool ok = recheck(&bad_idx, storage);
            if ( !ok ) {
                std::cout << "bad block detected at idx=" << bad_idx << std::endl;

                return EXIT_FAILURE;
            } else {
                std::cout << "blockchain is correct!" << std::endl;
            }

            break;
        }

        case 'd' : {
            dump(storage);

            break;
        }

        default: {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/*************************************************************************************************/
