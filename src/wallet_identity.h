#pragma once

#include <string>
#include <cstdint>

struct FfiSdkEngine;

namespace wallet_identity {

/**
 * @brief Requests identity permission grants (`IdentityRead` or `IdentityPropose`) for a session.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier.
 * @param grantKind Grant kind string ("IdentityRead" or "IdentityPropose").
 * @return JSON response string with operation status.
 */
std::string request_identity_access(FfiSdkEngine* engine,
                                    const std::string& sessionId,
                                    const std::string& grantKind);

/**
 * @brief Retrieves the user's 32-byte public identity commitment as a hex string.
 * Requires `IdentityRead` permission grant.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with read permission.
 * @return JSON response string containing the commitment hex.
 */
std::string get_identity_commitment(FfiSdkEngine* engine,
                                    const std::string& sessionId);

/**
 * @brief Retrieves the 32-byte identity commitment directly via raw byte buffer.
 * Requires `IdentityRead` permission grant.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with read permission.
 * @return JSON response string containing hex representation and copy status.
 */
std::string get_identity_commitment_bytes(FfiSdkEngine* engine,
                                          const std::string& sessionId);

/**
 * @brief Derives a scope-specific pseudonymous nullifier hex.
 * Provides unlinkability across different scopes while maintaining determinism within a scope.
 * Requires `IdentityRead` permission grant.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with read permission.
 * @param scopeId Scope identifier string (e.g. governance forum or room ID).
 * @return JSON response string containing the scoped nullifier hex.
 */
std::string derive_scoped_nullifier(FfiSdkEngine* engine,
                                    const std::string& sessionId,
                                    const std::string& scopeId);

/**
 * @brief Signs an arbitrary message using the scoped identity credential.
 * Requires `IdentityPropose` permission grant.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with propose permission.
 * @param scopeId Scope identifier string.
 * @param messageHex Hex-encoded byte payload to sign.
 * @return JSON response string containing the signature, commitment, and nullifier.
 */
std::string sign_identity_attestation(FfiSdkEngine* engine,
                                      const std::string& sessionId,
                                      const std::string& scopeId,
                                      const std::string& messageHex);

/**
 * @brief Prepares an on-chain registration payload with Two-Tier Shamir Secret Sharing threshold shares.
 * Requires `IdentityPropose` permission grant.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier with propose permission.
 * @param scopeId Scope identifier string.
 * @param nodePubkeysJson JSON array of 32-byte hex public keys of trustee nodes.
 * @param thresholdK Reconstruction threshold k (must be 1 <= k <= total nodes).
 * @return JSON response string containing commitment, scope, threshold, and encrypted shares.
 */
std::string prepare_identity_registration(FfiSdkEngine* engine,
                                          const std::string& sessionId,
                                          const std::string& scopeId,
                                          const std::string& nodePubkeysJson,
                                          uint64_t thresholdK);

} // namespace wallet_identity
