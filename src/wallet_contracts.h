#pragma once

#include <string>

struct FfiSdkEngine;

namespace wallet_contracts {

/**
 * @brief Proposes a generic smart contract transaction with calldata and
 * account mentions. The transaction is submitted to the wallet engine's pending
 * queue and triggers approval.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with propose permissions.
 * @param mentionsJson JSON array string of AccountId strings mentioned in the
 * transaction.
 * @param calldataHex Hex-encoded instruction calldata bytes.
 * @param description Human-readable description or purpose of the transaction.
 * @return JSON response string with request_id and pending status, or error.
 */
std::string propose_transaction(FfiSdkEngine *engine,
                                const std::string &sessionId,
                                const std::string &mentionsJson,
                                const std::string &calldataHex,
                                const std::string &description);

/**
 * @brief Proposes an explicit smart contract call with programId, payerId, and
 * mentions.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with propose permissions.
 * @param programId Contract program AccountId.
 * @param payerId AccountId of the fee payer / initiator.
 * @param mentionsJson JSON array string of AccountId strings.
 * @param calldataHex Hex-encoded instruction calldata bytes.
 * @param description Human-readable description of the call.
 * @return JSON response string with request_id and pending status, or error.
 */
std::string propose_contract_call(FfiSdkEngine *engine,
                                  const std::string &sessionId,
                                  const std::string &programId,
                                  const std::string &payerId,
                                  const std::string &mentionsJson,
                                  const std::string &calldataHex,
                                  const std::string &description);

} // namespace wallet_contracts
