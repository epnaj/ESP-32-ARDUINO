#pragma once

#include <string>
#include <mbedtls/sha256.h>
#include "psram_custom_allocator.hpp"

ps::string sha256(const char *data, size_t dataLength);
ps::string sha256(const ps::string &data);