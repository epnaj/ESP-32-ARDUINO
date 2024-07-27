#include "sha256.hpp"

ps::string sha256(const char *data, size_t dataLength){
    ps::string output(64, ' ');
    constexpr size_t shaReslultSize{32};
    // uint8_t shaResult[shaReslultSize]{0};
    ps::vector <uint8_t> shaResult(shaReslultSize, 0);
    mbedtls_sha256_ret(
        reinterpret_cast <const unsigned char *> (data),
        dataLength,
        shaResult.data(),
        0
    );
    for(int i=0; i<shaReslultSize; ++i){
        sprintf(&output[i * 2], "%02x", static_cast <int> (shaResult[i]));
    }
    return output;
}

ps::string sha256(const ps::string &data){
    return sha256(data.c_str(), data.size());
}