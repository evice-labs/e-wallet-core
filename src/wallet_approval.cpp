#include "wallet_approval.h"
#include "wallet_helpers.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace wallet_approval {

std::string set_approval_callback(FfiSdkEngine* engine,
                                  FfiApprovalCallback callback,
                                  void* user_data) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (!callback) {
        return wallet_helpers::make_error_json("Approval callback cannot be null");
    }

    char* raw = lez_wallet_set_approval_callback(engine, callback, user_data);
    return wallet_helpers::consume_ffi_string(raw);
}

std::string submit_approval_decision(FfiSdkEngine* engine,
                                     const std::string& request_id,
                                     int64_t decision) {
    if (!engine) {
        return wallet_helpers::make_error_json("Wallet engine is not initialized");
    }
    if (request_id.empty()) {
        return wallet_helpers::make_error_json("Request ID cannot be empty");
    }
    if (decision != 0 && decision != 1) {
        return wallet_helpers::make_error_json("Approval decision must be 1 (Approve) or 0 (Reject)");
    }

    char* raw = lez_wallet_submit_approval_decision(engine, request_id.c_str(), static_cast<int32_t>(decision));
    return wallet_helpers::consume_ffi_string(raw);
}

std::string simulate_prompt_effect(const std::string& prompt_json) {
    try {
        json parsed = json::parse(prompt_json);

        auto getStringField = [&](const std::string& key, const std::string& fallback) -> std::string {
            if (parsed.contains(key) && !parsed[key].is_null() && parsed[key].is_string()) {
                return parsed[key].get<std::string>();
            }
            return fallback;
        };

        std::string reqId = getStringField("request_id", "");
        std::string sessId = getStringField("session_id", "");
        std::string dapp = getStringField("dapp_identity", "unknown");
        std::string kind = getStringField("request_kind", "unknown");
        std::string summary = getStringField("summary", "No summary provided");
        std::string effects = getStringField("effects", "");
        std::string verStatus = getStringField("verification_status", "unverified");

        json warnings = (parsed.contains("warnings") && parsed["warnings"].is_array())
                            ? parsed["warnings"]
                            : json::array();

        // Construct high-level human-readable interpretation
        std::string humanDesc = "[" + dapp + "] ";
        if (kind == "native_transfer") {
            humanDesc += "requests native asset transfer: " + summary;
        } else if (kind == "token_transfer") {
            humanDesc += "requests fungible token transfer: " + summary;
        } else if (kind == "generic_call") {
            humanDesc += "requests smart contract execution: " + summary;
        } else {
            humanDesc += summary;
        }

        json simulated = {
            {"ok", true},
            {"request_id", reqId},
            {"session_id", sessId},
            {"dapp_identity", dapp},
            {"request_kind", kind},
            {"summary", summary},
            {"effects", effects},
            {"verification_status", verStatus},
            {"warnings", warnings},
            {"human_description", humanDesc}
        };

        return simulated.dump();
    } catch (const std::exception& e) {
        return wallet_helpers::make_error_json(std::string("Failed to parse prompt JSON: ") + e.what());
    }
}

} // namespace wallet_approval
