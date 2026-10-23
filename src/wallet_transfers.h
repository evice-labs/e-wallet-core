#pragma once

#include <string>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_transfers {

/**
 * @brief Proposes a native LEZ asset transfer (supports transparent/shielded accounts and .lez names).
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Authorized dApp session ID holding 'propose' access.
 * @param from_account Source account ID.
 * @param to_account_or_name Recipient account ID or registered .lez name.
 * @param amount_str Amount to transfer as string.
 * @return JSON containing request_id or error details.
 */
std::string propose_native_transfer(FfiSdkEngine* engine,
                                    const std::string& session_id,
                                    const std::string& from_account,
                                    const std::string& to_account_or_name,
                                    const std::string& amount_str);

/**
 * @brief Proposes a fungible token program asset transfer.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param session_id Authorized dApp session ID.
 * @param from_holding Source token holding ID.
 * @param to_holding Destination token holding ID.
 * @param definition_id Token definition / mint ID.
 * @param amount_str Amount to transfer as string.
 * @return JSON containing request_id or error details.
 */
std::string propose_token_transfer(FfiSdkEngine* engine,
                                   const std::string& session_id,
                                   const std::string& from_holding,
                                   const std::string& to_holding,
                                   const std::string& definition_id,
                                   const std::string& amount_str);

/**
 * @brief Queries the execution / approval status of a proposed transfer request.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param request_id Unique request ID.
 * @return JSON containing request status ('pending', 'approved', 'rejected', etc.).
 */
std::string get_request_status(FfiSdkEngine* engine, const std::string& request_id);

/**
 * @brief Cancels a pending request prior to user approval.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param request_id Unique request ID to cancel.
 * @return JSON indicating cancellation success or error.
 */
std::string cancel_request(FfiSdkEngine* engine, const std::string& request_id);

} // namespace wallet_transfers
