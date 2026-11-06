#include "wallet_transfers.h"
#include "wallet_helpers.h"
#include <algorithm>
#include <cctype>

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

    std::string lower_from = from_account;
    std::string lower_to = to_account_or_name;
    std::transform(lower_from.begin(), lower_from.end(), lower_from.begin(), ::tolower);
    std::transform(lower_to.begin(), lower_to.end(), lower_to.begin(), ::tolower);

    bool from_is_priv = (lower_from.rfind("private/", 0) == 0);
    bool to_is_priv = (lower_to.rfind("private/", 0) == 0);

    std::string clean_from = (lower_from.rfind("public/", 0) == 0) ? from_account.substr(7)
                           : (from_is_priv ? from_account.substr(8) : from_account);
    std::string clean_to = (lower_to.rfind("public/", 0) == 0) ? to_account_or_name.substr(7)
                         : (to_is_priv ? to_account_or_name.substr(8) : to_account_or_name);

    if (clean_from == clean_to) {
        return wallet_helpers::make_error_json("Self-transfer to the same account is not permitted");
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

std::string list_token_holdings(FfiSdkEngine* engine, const std::string& session_id) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (session_id.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    char* raw = lez_wallet_list_token_holdings(engine, session_id.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

std::string list_all_token_holdings(FfiSdkEngine* engine) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }

    char* raw = lez_wallet_list_all_token_holdings(engine);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string register_token_holding(FfiSdkEngine* engine,
                                   const std::string& holding_id,
                                   const std::string& definition_id,
                                   const std::string& symbol,
                                   const std::string& name,
                                   bool is_shielded) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (holding_id.empty() || definition_id.empty()) {
        return wallet_helpers::make_error_json("Holding ID and Definition ID cannot be empty");
    }

    char* raw = lez_wallet_register_token_holding(engine,
                                                  holding_id.c_str(),
                                                  definition_id.c_str(),
                                                  symbol.c_str(),
                                                  name.c_str(),
                                                  is_shielded);
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_transfers
