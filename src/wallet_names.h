#pragma once

#include <string>

struct FfiSdkEngine;

namespace wallet_names {

/**
 * @brief Validates an LNS domain name against the specification.
 * - Must end with suffix '.lez' (case-insensitive)
 * - Base name must be 1 to 32 characters
 * - Characters allowed: lowercase ASCII alphanumeric and internal hyphens
 * - Cannot start or end with a hyphen
 * @param name The domain name to validate.
 * @param errorMsg Populated with failure reason if invalid.
 * @return True if valid, false otherwise.
 */
bool validate_lns_name(const std::string& name, std::string& errorMsg);

/**
 * @brief Resolves an LNS '.lez' human-readable domain name to destination accounts or keys.
 * @param engine Pointer to active FFI SDK engine.
 * @param sessionId Valid active session identifier.
 * @param name Domain name to resolve (e.g., 'alice.lez').
 * @return JSON response string with resolution data (account, npk, vpk, expires, texts) or error.
 */
std::string resolve_name(FfiSdkEngine* engine,
                         const std::string& sessionId,
                         const std::string& name);

} // namespace wallet_names
