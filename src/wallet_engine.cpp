#include "wallet_engine.h"
#include "wallet_helpers.h"

namespace wallet_engine {

FfiSdkEngine* init_engine(const std::string& data_dir, const std::string& password) {
    if (data_dir.empty()) {
        return nullptr;
    }

    if (password.empty()) {
        return lez_wallet_engine_new(data_dir.c_str());
    } else {
        return lez_wallet_engine_new_with_password(data_dir.c_str(), password.c_str());
    }
}

void destroy_engine(FfiSdkEngine* engine) {
    if (engine) {
        lez_wallet_engine_free(engine);
    }
}

std::string create_wallet(FfiSdkEngine* engine, const std::string& password) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (password.empty()) {
        return wallet_helpers::make_error_json("Password cannot be empty");
    }

    char* raw = lez_wallet_create_wallet(engine, password.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string restore_wallet(FfiSdkEngine* engine, const std::string& mnemonic, const std::string& password) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (mnemonic.empty()) {
        return wallet_helpers::make_error_json("Mnemonic cannot be empty");
    }
    if (password.empty()) {
        return wallet_helpers::make_error_json("Password cannot be empty");
    }

    char* raw = lez_wallet_restore_wallet(engine, mnemonic.c_str(), password.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string lock_wallet(FfiSdkEngine* engine) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_lock(engine);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string unlock_wallet(FfiSdkEngine* engine, const std::string& password) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (password.empty()) {
        return wallet_helpers::make_error_json("Password cannot be empty");
    }

    char* raw = lez_wallet_unlock(engine, password.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_sdk_info(FfiSdkEngine* engine) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_sdk_info(engine);
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_engine
