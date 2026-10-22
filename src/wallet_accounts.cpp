#include "wallet_accounts.h"
#include "wallet_helpers.h"

namespace wallet_accounts {

std::string create_public_account(FfiSdkEngine* engine, const std::string& label) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_create_account_public(engine, label.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string create_private_account(FfiSdkEngine* engine, const std::string& label) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_create_account_private(engine, label.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string list_all_accounts(FfiSdkEngine* engine) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_list_all_accounts(engine);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string set_account_label(FfiSdkEngine* engine,
                              const std::string& account_id,
                              const std::string& label,
                              bool is_private) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (account_id.empty()) {
        return wallet_helpers::make_error_json("Account ID cannot be empty");
    }

    char* raw = lez_wallet_set_account_label(engine, account_id.c_str(), label.c_str(), is_private);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_balance(FfiSdkEngine* engine,
                        const std::string& session_id,
                        const std::string& account_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (account_id.empty()) {
        return wallet_helpers::make_error_json("Account ID cannot be empty");
    }

    char* raw = lez_wallet_get_balance(engine, session_id.c_str(), account_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_accounts
