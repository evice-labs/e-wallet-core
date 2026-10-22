#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_helpers {

/**
 * @brief RAII helper to safely capture and free C-ABI allocated strings.
 * Ensures lez_wallet_free_string is always called and prevents memory leaks.
 */
inline std::string consume_ffi_string(char* raw) {
    if (!raw) {
        return "{\"ok\":false,\"error\":\"Null pointer returned from SDK FFI\"}";
    }
    std::string result(raw);
    lez_wallet_free_string(raw);
    return result;
}

/**
 * @brief Constructs a standardised error JSON string.
 */
inline std::string make_error_json(const std::string& message) {
    nlohmann::json err = {
        {"ok", false},
        {"error", message}
    };
    return err.dump();
}

/**
 * @brief Constructs a standardised success JSON string.
 */
inline std::string make_ok_json(const nlohmann::json& payload = nlohmann::json::object()) {
    nlohmann::json res = payload;
    res["ok"] = true;
    return res.dump();
}

/**
 * @brief Encodes raw byte buffer into lowercase hex string.
 */
inline std::string bytes_to_hex(const uint8_t* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(data[i]);
    }
    return oss.str();
}

/**
 * @brief Decodes hex string into raw byte vector.
 */
inline bool hex_to_bytes(const std::string& hex, std::vector<uint8_t>& out) {
    out.clear();
    std::string cleaned = hex;
    if (cleaned.rfind("0x", 0) == 0 || cleaned.rfind("0X", 0) == 0) {
        cleaned = cleaned.substr(2);
    }
    if (cleaned.length() % 2 != 0) {
        return false;
    }
    out.reserve(cleaned.length() / 2);
    for (size_t i = 0; i < cleaned.length(); i += 2) {
        std::string byteString = cleaned.substr(i, 2);
        char* endPtr = nullptr;
        unsigned long byteVal = std::strtoul(byteString.c_str(), &endPtr, 16);
        if (endPtr != byteString.c_str() + 2) {
            out.clear();
            return false;
        }
        out.push_back(static_cast<uint8_t>(byteVal));
    }
    return true;
}

} // namespace wallet_helpers
