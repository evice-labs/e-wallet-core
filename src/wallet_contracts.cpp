#include "wallet_contracts.h"
#include "wallet_helpers.h"
#include "../lib/lez_wallet_sdk.h"

namespace wallet_contracts {

std::string propose_transaction(FfiSdkEngine* engine,
                                const std::string& sessionId,
                                const std::string& mentionsJson,
                                const std::string& calldataHex,
                                const std::string& description)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }
    if (sessionId.empty()) {
        return wallet_helpers::make_error_json("Session ID cannot be empty");
    }
    if (mentionsJson.empty()) {
        return wallet_helpers::make_error_json("Mentions JSON cannot be empty");
    }
    if (calldataHex.empty()) {
        return wallet_helpers::make_error_json("Calldata hex cannot be empty");
    }

    // Validate mentions JSON structure
    nlohmann::json parsedMentions;
    try {
        parsedMentions = nlohmann::json::parse(mentionsJson);
    } catch (const std::exception& e) {
        return wallet_helpers::make_error_json(std::string("Malformed mentions JSON: ") + e.what());
    }

    if (!parsedMentions.is_array()) {
        return wallet_helpers::make_error_json("Mentions JSON must be an array of account ID strings");
    }

    for (size_t i = 0; i < parsedMentions.size(); ++i) {
        if (!parsedMentions[i].is_string()) {
            return wallet_helpers::make_error_json("Mention item at index " + std::to_string(i) + " must be a string");
        }
        if (parsedMentions[i].get<std::string>().empty()) {
            return wallet_helpers::make_error_json("Mention item at index " + std::to_string(i) + " cannot be empty");
        }
    }

    // Validate calldata hex format
    std::vector<uint8_t> calldataBytes;
    if (!wallet_helpers::hex_to_bytes(calldataHex, calldataBytes)) {
        return wallet_helpers::make_error_json("Invalid calldata hex format");
    }

    const char* descPtr = description.empty() ? nullptr : description.c_str();

    char* raw = lez_wallet_propose_transaction(
        engine,
        sessionId.c_str(),
        mentionsJson.c_str(),
        calldataHex.c_str(),
        descPtr
    );
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_contracts
