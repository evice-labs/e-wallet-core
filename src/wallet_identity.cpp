#include "wallet_identity.h"
#include "wallet_helpers.h"
#include "../lib/lez_wallet_sdk.h"

namespace wallet_identity {

std::string request_identity_access(FfiSdkEngine* engine,
                                    const std::string& sessionId,
                                    const std::string& grantKind)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
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

    char* raw = lez_wallet_get_identity_commitment(engine, sessionId.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_identity_commitment_bytes(FfiSdkEngine* engine,
                                          const std::string& sessionId)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
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
