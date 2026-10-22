#pragma once

#include <string>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_engine {

/**
 * @brief Initializes the SdkEngine instance with data directory and optional password.
 */
FfiSdkEngine* init_engine(const std::string& data_dir, const std::string& password = "");

/**
 * @brief Frees the SdkEngine handle and releases all internal resources.
 */
void destroy_engine(FfiSdkEngine* engine);

/**
 * @brief Creates a fresh HD wallet encrypted under the provided password.
 * Generates and returns a 12/24-word BIP39 mnemonic.
 */
std::string create_wallet(FfiSdkEngine* engine, const std::string& password);

/**
 * @brief Restores a wallet from a BIP39 mnemonic and sets an encryption password.
 */
std::string restore_wallet(FfiSdkEngine* engine, const std::string& mnemonic, const std::string& password);

/**
 * @brief Locks the wallet, purging secret keys and active dApp sessions from memory.
 */
std::string lock_wallet(FfiSdkEngine* engine);

/**
 * @brief Unlocks the wallet using the encryption password.
 */
std::string unlock_wallet(FfiSdkEngine* engine, const std::string& password);

/**
 * @brief Queries engine version, API version, and supported capabilities.
 */
std::string get_sdk_info(FfiSdkEngine* engine);

} // namespace wallet_engine
