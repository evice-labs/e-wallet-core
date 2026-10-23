#pragma once

#include <string>
#include <functional>
#include <cstdint>
#include "../lib/lez_wallet_sdk.h"

namespace wallet_approval {

/**
 * @brief Registers a C-ABI approval callback for interactive prompt handling in Basecamp UI.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param callback Function pointer conforming to FfiApprovalCallback.
 * @param user_data Arbitrary pointer passed back to the callback.
 * @return JSON indicating success or error.
 */
std::string set_approval_callback(FfiSdkEngine* engine,
                                  FfiApprovalCallback callback,
                                  void* user_data);

/**
 * @brief Submits a user approval decision for a pending request.
 *
 * @param engine Pointer to the active SdkEngine instance.
 * @param request_id Unique identifier of the pending request.
 * @param decision 1 for Approve, 0 for Reject.
 * @return JSON indicating decision submission status.
 */
std::string submit_approval_decision(FfiSdkEngine* engine,
                                     const std::string& request_id,
                                     int64_t decision);

/**
 * @brief Parses and simulates the human-readable effect summary of an approval prompt.
 *
 * Extracts dApp identity, request type, call effects, program source verification
 * status, and security warnings according to LP-0021 requirements.
 *
 * @param prompt_json Raw JSON string received from the approval callback.
 * @return JSON containing formatted prompt details and human-readable effect summary.
 */
std::string simulate_prompt_effect(const std::string& prompt_json);

} // namespace wallet_approval
