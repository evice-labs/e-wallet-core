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
 * @brief Changes the master encryption password of the wallet storage.
 */
std::string change_password(FfiSdkEngine* engine, const std::string& old_password, const std::string& new_password);

/**
 * @brief Queries engine version, API version, and supported capabilities.
 */
std::string get_sdk_info(FfiSdkEngine* engine);

/**
 * @brief Opens the storage directory using the system file manager.
 */
std::string open_storage_folder(const std::string& data_dir);

/**
 * @brief Retrieves the active transaction gas limit seed from the SDK.
 */
std::string get_gas_limit(FfiSdkEngine* engine);

/**
 * @brief Sets the transaction gas limit seed on the SDK.
 */
std::string set_gas_limit(FfiSdkEngine* engine, uint64_t limit);

} // namespace wallet_engine
