#include "wallet_transfers.h"
#include "wallet_helpers.h"

namespace wallet_transfers {

std::string propose_native_transfer(FfiSdkEngine* engine,
                                    const std::string& session_id,
                                    const std::string& from_account,
                                    const std::string& to_account_or_name,
                                    const std::string& amount_str) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (from_account.empty()) {
        return wallet_helpers::make_error_json("Sender account cannot be empty");
    }
    if (to_account_or_name.empty()) {
        return wallet_helpers::make_error_json("Destination account or name cannot be empty");
    }
    if (amount_str.empty() || amount_str == "0") {
        return wallet_helpers::make_error_json("Transfer amount must be greater than zero");
    }
    if (from_account == to_account_or_name) {
        return wallet_helpers::make_error_json("Self-transfer is not permitted");
    }

    char* raw = lez_wallet_propose_native_transfer(engine,
                                                   session_id.c_str(),
                                                   from_account.c_str(),
                                                   to_account_or_name.c_str(),
                                                   amount_str.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string propose_token_transfer(FfiSdkEngine* engine,
                                   const std::string& session_id,
                                   const std::string& from_holding,
                                   const std::string& to_holding,
                                   const std::string& definition_id,
                                   const std::string& amount_str) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (from_holding.empty() || to_holding.empty()) {
        return wallet_helpers::make_error_json("Source and destination token holdings cannot be empty");
    }
    if (definition_id.empty()) {
        return wallet_helpers::make_error_json("Token definition ID cannot be empty");
    }
    if (amount_str.empty() || amount_str == "0") {
        return wallet_helpers::make_error_json("Transfer amount must be greater than zero");
    }
    if (from_holding == to_holding) {
        return wallet_helpers::make_error_json("Self-transfer of token holdings is not permitted");
    }

    char* raw = lez_wallet_propose_token_transfer(engine,
                                                  session_id.c_str(),
                                                  from_holding.c_str(),
                                                  to_holding.c_str(),
                                                  definition_id.c_str(),
                                                  amount_str.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string get_request_status(FfiSdkEngine* engine, const std::string& request_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (request_id.empty()) {
        return wallet_helpers::make_error_json("Request ID cannot be empty");
    }

    char* raw = lez_wallet_get_request_status(engine, request_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string cancel_request(FfiSdkEngine* engine, const std::string& request_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (request_id.empty()) {
        return wallet_helpers::make_error_json("Request ID cannot be empty");
    }

    char* raw = lez_wallet_cancel_request(engine, request_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_transfers
