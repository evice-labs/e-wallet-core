#include "wallet_names.h"
#include "wallet_helpers.h"
#include "../lib/lez_wallet_sdk.h"
#include <algorithm>
#include <cctype>

namespace wallet_names {

namespace {

std::string trim_and_lower(const std::string& input) {
    size_t first = input.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = input.find_last_not_of(" \t\n\r");
    std::string trimmed = input.substr(first, (last - first + 1));
    std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return trimmed;
}

} // anonymous namespace

bool validate_lns_name(const std::string& name, std::string& errorMsg)
{
    std::string normalised = trim_and_lower(name);
    if (normalised.empty()) {
        errorMsg = "Domain name cannot be empty";
        return false;
    }

    const std::string suffix = ".lez";
    if (normalised.length() <= suffix.length() ||
        normalised.compare(normalised.length() - suffix.length(), suffix.length(), suffix) != 0) {
        errorMsg = "Domain name must end with '.lez'";
        return false;
    }

    std::string base = normalised.substr(0, normalised.length() - suffix.length());
    if (base.empty()) {
        errorMsg = "Domain base name cannot be empty";
        return false;
    }
    if (base.length() > 32) {
        errorMsg = "Domain base name exceeds maximum length of 32 characters";
        return false;
    }

    if (base.front() == '-' || base.back() == '-') {
        errorMsg = "Domain name cannot start or end with a hyphen";
        return false;
    }

    for (char c : base) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-') {
            errorMsg = std::string("Invalid character '") + c + "' in domain name. Only lowercase alphanumeric and hyphens allowed";
            return false;
        }
    }

    return true;
}

std::string resolve_name(FfiSdkEngine* engine,
                         const std::string& sessionId,
                         const std::string& name)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }

    std::string validationError;
    if (!validate_lns_name(name, validationError)) {
        return wallet_helpers::make_error_json(validationError);
    }

    char* raw = lez_wallet_resolve_name(engine, sessionId.c_str(), name.c_str());
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_names
