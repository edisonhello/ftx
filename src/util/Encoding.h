#pragma once

#include <openssl/evp.h>
#include <openssl/hmac.h>

#undef string_to_hex // pollution from openssl lib

#include <string>


namespace util::encoding {

struct HmacCtx {
    HMAC_CTX *ctx;
    HmacCtx() { ctx = HMAC_CTX_new(); }
    ~HmacCtx() { HMAC_CTX_free(ctx); }
};

std::string hmac(const std::string& secret,
                 std::string msg,
                 std::size_t signed_len);


std::string string_to_hex(unsigned char* data, std::size_t len);

} // namespace util::encoding