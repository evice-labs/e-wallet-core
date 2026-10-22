#pragma once

#include <string>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_accounts {

/**
 * @brief Derives and registers a new transparent/public account with an optional label.
 */
std::string create_public_account(FfiSdkEngine* engine, const std::string& label = "");

/**
 * @brief Derives and registers a new shielded/private account with an optional label.
 */
std::string create_private_account(FfiSdkEngine* engine, const std::string& label = "");

/**
 * @brief Lists all accounts managed by the wallet (public and private) with balances and labels.
 */
std::string list_all_accounts(FfiSdkEngine* engine);

/**
 * @brief Updates the user-friendly label for a specific account.
 */
std::string set_account_label(FfiSdkEngine* engine,
                              const std::string& account_id,
                              const std::string& label,
                              bool is_private);

/**
 * @brief Retrieves native token balance for an account permitted under the given session.
 */
std::string get_balance(FfiSdkEngine* engine,
                        const std::string& session_id,
                        const std::string& account_id);

} // namespace wallet_accounts
