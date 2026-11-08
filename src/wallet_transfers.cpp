#include "wallet_transfers.h"
#include "wallet_helpers.h"

namespace wallet_transfers {

std::string propose_native_transfer(FfiSdkEngine *engine,
                                    const std::string &session_id,
                                    const std::string &from_account,
                                    const std::string &to_account_or_name,
                                    const std::string &amount_str) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_propose_native_transfer(
      engine, session_id.c_str(), from_account.c_str(),
      to_account_or_name.c_str(), amount_str.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

std::string propose_token_transfer(FfiSdkEngine *engine,
                                   const std::string &session_id,
                                   const std::string &from_holding,
                                   const std::string &to_holding,
                                   const std::string &definition_id,
                                   const std::string &amount_str) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_propose_token_transfer(
      engine, session_id.c_str(), from_holding.c_str(), to_holding.c_str(),
      definition_id.c_str(), amount_str.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

std::string get_request_status(FfiSdkEngine *engine,
                               const std::string &request_id) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (request_id.empty()) {
    return wallet_helpers::make_error_json("Request ID cannot be empty");
  }

  char *raw = e_wallet_get_request_status(engine, request_id.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

std::string cancel_request(FfiSdkEngine *engine,
                           const std::string &request_id) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (request_id.empty()) {
    return wallet_helpers::make_error_json("Request ID cannot be empty");
  }

  char *raw = e_wallet_cancel_request(engine, request_id.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

std::string list_token_holdings(FfiSdkEngine *engine,
                                const std::string &session_id) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (session_id.empty()) {
    return wallet_helpers::make_error_json("Session ID cannot be empty");
  }

  char *raw = e_wallet_list_token_holdings(engine, session_id.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

std::string list_all_token_holdings(FfiSdkEngine *engine) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_list_all_token_holdings(engine);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string register_token_holding(FfiSdkEngine *engine,
                                   const std::string &holding_id,
                                   const std::string &definition_id,
                                   const std::string &symbol,
                                   const std::string &name, bool is_shielded) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (holding_id.empty() || definition_id.empty()) {
    return wallet_helpers::make_error_json(
        "Holding ID and Definition ID cannot be empty");
  }

  char *raw = e_wallet_register_token_holding(
      engine, holding_id.c_str(), definition_id.c_str(), symbol.c_str(),
      name.c_str(), is_shielded);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string start_pow_mining(FfiSdkEngine *engine, uint32_t threads) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_pow_start_mining(engine, threads);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string stop_pow_mining(FfiSdkEngine *engine) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_pow_stop_mining(engine);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string get_pow_status(FfiSdkEngine *engine) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }

  char *raw = e_wallet_pow_status(engine);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string channel_deposit(FfiSdkEngine *engine, const std::string &account_id,
                            const std::string &amount_str) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (account_id.empty()) {
    return wallet_helpers::make_error_json(
        "Recipient account ID cannot be empty");
  }

  char *raw =
      e_wallet_channel_deposit(engine, account_id.c_str(), amount_str.c_str());
  return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_transfers
