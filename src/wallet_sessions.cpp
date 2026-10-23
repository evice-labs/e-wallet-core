#include "wallet_sessions.h"
#include "wallet_helpers.h"

namespace wallet_sessions {

std::string connect(FfiSdkEngine* engine,
                    const std::string& caller_identity,
                    uint64_t api_version) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (caller_identity.empty()) {
        return wallet_helpers::make_error_json("Caller identity cannot be empty");
    }

    char* raw = lez_wallet_connect(engine, caller_identity.c_str(), static_cast<uint32_t>(api_version));
    return wallet_helpers::consume_ffi_string(raw);
}

std::string disconnect(FfiSdkEngine* engine, const std::string& session_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_disconnect(engine, session_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string request_accounts(FfiSdkEngine* engine, const std::string& session_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_request_accounts(engine, session_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string list_accounts(FfiSdkEngine* engine, const std::string& session_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_list_accounts(engine, session_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string grant_account(FfiSdkEngine* engine,
                          const std::string& session_id,
                          const std::string& account_id,
                          const std::string& grant_kind) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (account_id.empty()) {
        return wallet_helpers::make_error_json("Account ID cannot be empty");
    }
    if (grant_kind != "read" && grant_kind != "propose") {
        return wallet_helpers::make_error_json("Grant kind must be 'read' or 'propose'");
    }

    char* raw = lez_wallet_grant_account(engine,
                                         session_id.c_str(),
                                         account_id.c_str(),
                                         grant_kind.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string list_sessions(FfiSdkEngine* engine) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_list_sessions(engine);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string revoke_session(FfiSdkEngine* engine, const std::string& session_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_revoke_session(engine, session_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_sessions
