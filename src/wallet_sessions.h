#pragma once

#include <string>
#include <cstdint>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_sessions {

/**
 * @brief Connects an external caller module (dApp / Basecamp plugin) and negotiates an active session.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param caller_identity Identifier/name of the requesting caller module.
 * @param api_version API capability version requested by caller.
 * @return JSON containing session_id or rejection error.
 */
std::string connect(FfiSdkEngine* engine,
                    const std::string& caller_identity,
                    uint64_t api_version = 1);

/**
 * @brief Disconnects and terminates an active dApp session gracefully.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Session ID to terminate.
 * @return JSON indicating disconnection status.
 */
std::string disconnect(FfiSdkEngine* engine, const std::string& session_id);

/**
 * @brief Requests the user/wallet to select and share accounts with the session.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Session ID requesting account sharing.
 * @return JSON containing requested accounts.
 */
std::string request_accounts(FfiSdkEngine* engine, const std::string& session_id);

/**
 * @brief Lists all accounts currently permitted for the given session.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Session ID.
 * @return JSON containing array of permitted account IDs and permissions.
 */
std::string list_accounts(FfiSdkEngine* engine, const std::string& session_id);

/**
 * @brief Grants account permission ('read' or 'propose') to a specific session.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Session ID receiving the grant.
 * @param account_id Account ID being granted.
 * @param grant_kind Grant type: "read" or "propose".
 * @return JSON indicating grant status.
 */
std::string grant_account(FfiSdkEngine* engine,
                          const std::string& session_id,
                          const std::string& account_id,
                          const std::string& grant_kind);

/**
 * @brief Lists all active dApp sessions across the wallet engine.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @return JSON containing array of active session metadata.
 */
std::string list_sessions(FfiSdkEngine* engine);

/**
 * @brief Revokes a specific dApp session by session ID.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Session ID to revoke.
 * @return JSON indicating revocation status.
 */
std::string revoke_session(FfiSdkEngine* engine, const std::string& session_id);

} // namespace wallet_sessions
