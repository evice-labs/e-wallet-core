#include "wallet_identity.h"
#include "wallet_helpers.h"
#include "../lib/lez_wallet_sdk.h"
#include <algorithm>
#include <cctype>

namespace wallet_identity {

namespace {

std::string to_lower_copy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

} // anonymous namespace

std::string request_identity_access(FfiSdkEngine* engine,
                                    const std::string& sessionId,
                                    const std::string& grantKind)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (grantKind.empty()) {
        return wallet_helpers::make_error_json("Grant kind cannot be empty");
    }

    std::string lowerKind = to_lower_copy(grantKind);
    if (lowerKind != "identityread" && lowerKind != "identitypropose" &&
        lowerKind != "read" && lowerKind != "propose") {
        return wallet_helpers::make_error_json("Invalid identity grant kind: must be 'IdentityRead' or 'IdentityPropose'");
    }

    char* raw = lez_wallet_request_identity_access(engine, sessionId.c_str(), grantKind.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_identity_commitment(FfiSdkEngine* engine,
                                    const std::string& sessionId)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_get_identity_commitment(engine, sessionId.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_identity_commitment_bytes(FfiSdkEngine* engine,
                                          const std::string& sessionId)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    uint8_t outBuffer[32] = {0};
    int32_t rc = lez_wallet_get_identity_commitment_bytes(engine, sessionId.c_str(), outBuffer);
    if (rc != 0) {
        return wallet_helpers::make_error_json("Failed to retrieve identity commitment bytes (code " + std::to_string(rc) + ")");
    }

    nlohmann::json res = {
        {"ok", true},
        {"commitment", wallet_helpers::bytes_to_hex(outBuffer, 32)},
        {"length", 32}
    };
    return res.dump();
}

std::string derive_scoped_nullifier(FfiSdkEngine* engine,
                                    const std::string& sessionId,
                                    const std::string& scopeId)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (scopeId.empty()) {
        return wallet_helpers::make_error_json("Scope ID cannot be empty");
    }

    char* raw = lez_wallet_derive_scoped_nullifier(engine, sessionId.c_str(), scopeId.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string sign_identity_attestation(FfiSdkEngine* engine,
                                      const std::string& sessionId,
                                      const std::string& scopeId,
                                      const std::string& messageHex)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (scopeId.empty()) {
        return wallet_helpers::make_error_json("Scope ID cannot be empty");
    }
    if (messageHex.empty()) {
        return wallet_helpers::make_error_json("Message hex cannot be empty");
    }

    std::vector<uint8_t> dummyBytes;
    if (!wallet_helpers::hex_to_bytes(messageHex, dummyBytes)) {
        return wallet_helpers::make_error_json("Invalid message hex format");
    }

    char* raw = lez_wallet_sign_identity_attestation(engine, sessionId.c_str(), scopeId.c_str(), messageHex.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string prepare_identity_registration(FfiSdkEngine* engine,
                                          const std::string& sessionId,
                                          const std::string& scopeId,
                                          const std::string& nodePubkeysJson,
                                          uint64_t thresholdK)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (scopeId.empty()) {
        return wallet_helpers::make_error_json("Scope ID cannot be empty");
    }
    if (nodePubkeysJson.empty()) {
        return wallet_helpers::make_error_json("Node public keys JSON cannot be empty");
    }

    nlohmann::json parsedKeys;
    try {
        parsedKeys = nlohmann::json::parse(nodePubkeysJson);
    } catch (const std::exception& e) {
        return wallet_helpers::make_error_json(std::string("Malformed node public keys JSON: ") + e.what());
    }

    if (!parsedKeys.is_array()) {
        return wallet_helpers::make_error_json("Node public keys JSON must be an array");
    }
    if (parsedKeys.empty()) {
        return wallet_helpers::make_error_json("Node public keys array cannot be empty");
    }

    for (size_t i = 0; i < parsedKeys.size(); ++i) {
        if (!parsedKeys[i].is_string()) {
            return wallet_helpers::make_error_json("Each trustee node public key must be a string");
        }
        std::string pkHex = parsedKeys[i].get<std::string>();
        std::vector<uint8_t> pkBytes;
        if (!wallet_helpers::hex_to_bytes(pkHex, pkBytes) || pkBytes.size() != 32) {
            return wallet_helpers::make_error_json("Invalid 32-byte public key hex at index " + std::to_string(i));
        }
    }

    if (thresholdK == 0 || thresholdK > parsedKeys.size() || thresholdK > 255) {
        return wallet_helpers::make_error_json("Invalid threshold k: must be 1 <= k <= total trustee nodes (max 255)");
    }

    char* raw = lez_wallet_prepare_identity_registration(
        engine,
        sessionId.c_str(),
        scopeId.c_str(),
        nodePubkeysJson.c_str(),
        static_cast<uint8_t>(thresholdK)
    );
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_identity
