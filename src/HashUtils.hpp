#ifndef _HASH_UTILS_HPP_
#define _HASH_UTILS_HPP_

#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <cstring>

class HashUtils {
public:
    static bool computeHash(const std::string& unhashed, std::string& hashed);

    static bool compareHashes(const std::string& hash1, const std::string& hash2);
};

#endif