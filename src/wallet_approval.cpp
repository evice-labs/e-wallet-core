#include "wallet_approval.h"
#include "wallet_helpers.h"
#include <algorithm>
#include <mutex>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

namespace wallet_approval {

namespace {
std::mutex g_prompts_mutex;
std::vector<json> g_pending_prompts;

extern "C" int32_t default_approval_callback(const char *prompt_json,
                                             void * /*user_data*/) {
  if (!prompt_json) {
    return -1;
  }
  try {
    json parsed = json::parse(prompt_json);
    std::string dapp = parsed.value("dapp_identity", "");
    // Internal wallet UI actions submit their own approval decisions directly
    if (dapp == "internal_wallet_ui" || dapp.rfind("internal_", 0) == 0) {
      return -1;
    }

    std::string req_id = parsed.value("request_id", "");
    if (!req_id.empty()) {
      std::lock_guard<std::mutex> lock(g_prompts_mutex);
      auto it = std::find_if(g_pending_prompts.begin(), g_pending_prompts.end(),
                             [&](const json &item) {
                               return item.value("request_id", "") == req_id;
                             });
      if (it == g_pending_prompts.end()) {
        g_pending_prompts.push_back(parsed);
      } else {
        *it = parsed;
      }
    }
  } catch (...) {
  }
  return -1; // Deferred resolution via submitApprovalDecision
}
} // namespace

void register_default_callback(FfiSdkEngine *engine) {
  if (!engine) {
    return;
  }
  char *raw = e_wallet_set_approval_callback(engine, default_approval_callback,
                                             nullptr);
  wallet_helpers::consume_ffi_string(raw);
}

std::string list_pending_approvals() {
  std::lock_guard<std::mutex> lock(g_prompts_mutex);
  json res = {{"ok", true}, {"prompts", g_pending_prompts}};
  return res.dump();
}

void clear_pending_approvals() {
  std::lock_guard<std::mutex> lock(g_prompts_mutex);
  g_pending_prompts.clear();
}

void remove_pending_approval(const std::string &request_id) {
  if (request_id.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lock(g_prompts_mutex);
  g_pending_prompts.erase(
      std::remove_if(g_pending_prompts.begin(), g_pending_prompts.end(),
                     [&](const json &item) {
                       return item.value("request_id", "") == request_id;
                     }),
      g_pending_prompts.end());
}

std::string set_approval_callback(FfiSdkEngine *engine,
                                  FfiApprovalCallback callback,
                                  void *user_data) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (!callback) {
    return wallet_helpers::make_error_json("Approval callback cannot be null");
  }

  char *raw = e_wallet_set_approval_callback(engine, callback, user_data);
  return wallet_helpers::consume_ffi_string(raw);
}

std::string submit_approval_decision(FfiSdkEngine *engine,
                                     const std::string &request_id,
                                     int64_t decision) {
  if (!engine) {
    return wallet_helpers::make_error_json("Wallet engine is not initialized");
  }
  if (request_id.empty()) {
    return wallet_helpers::make_error_json("Request ID cannot be empty");
  }
  if (decision != 0 && decision != 1) {
    return wallet_helpers::make_error_json(
        "Approval decision must be 1 (Approve) or 0 (Reject)");
  }

  remove_pending_approval(request_id);
  char *raw = e_wallet_submit_approval_decision(engine, request_id.c_str(),
                                                static_cast<int32_t>(decision));
  return wallet_helpers::consume_ffi_string(raw);
}

std::string simulate_prompt_effect(const std::string &prompt_json) {
  try {
    json parsed = json::parse(prompt_json);

    auto getStringField = [&](const std::string &key,
                              const std::string &fallback) -> std::string {
      if (parsed.contains(key) && !parsed[key].is_null() &&
          parsed[key].is_string()) {
        return parsed[key].get<std::string>();
      }
      return fallback;
    };

    std::string reqId = getStringField("request_id", "");
    std::string sessId = getStringField("session_id", "");
    std::string dapp = getStringField(
        "dapp_identity", getStringField("dapp_name", "External dApp"));
    std::string kind = getStringField("request_kind", "contract_call");
    std::string summary = getStringField("summary", "No summary provided");
    std::string effects = getStringField("effects", "");
    std::string verStatus = getStringField("verification_status", "Verified");

    json warnings =
        (parsed.contains("warnings") && parsed["warnings"].is_array())
            ? parsed["warnings"]
            : json::array();

    // Construct high-level human-readable interpretation
    std::string humanDesc = "[" + dapp + "] ";
    if (kind == "native_transfer") {
      humanDesc += "requests native asset transfer: " + summary;
    } else if (kind == "token_transfer") {
      humanDesc += "requests fungible token transfer: " + summary;
    } else if (kind == "generic_call" || kind == "contract_call") {
      humanDesc += "requests smart contract execution: " + summary;
    } else {
      humanDesc += summary;
    }

    std::string stateDiff = !effects.empty()
                                ? effects
                                : "Account state transition pending execution";
    std::string gasFee = getStringField("gas_fee", "0.001 LEZ (34,000 gas)");

    json simDetails = {{"request_id", reqId},
                       {"session_id", sessId},
                       {"dapp_identity", dapp},
                       {"dapp_name", dapp},
                       {"request_kind", kind},
                       {"summary", summary},
                       {"effect", humanDesc},
                       {"effects", effects},
                       {"gas_fee", gasFee},
                       {"state_diff", stateDiff},
                       {"verification_status", verStatus},
                       {"warnings", warnings},
                       {"human_description", humanDesc}};

    json simulated = {{"ok", true},
                      {"request_id", reqId},
                      {"session_id", sessId},
                      {"dapp_identity", dapp},
                      {"dapp_name", dapp},
                      {"request_kind", kind},
                      {"summary", summary},
                      {"effect", humanDesc},
                      {"effects", effects},
                      {"gas_fee", gasFee},
                      {"state_diff", stateDiff},
                      {"verification_status", verStatus},
                      {"warnings", warnings},
                      {"human_description", humanDesc},
                      {"simulation", simDetails}};

    return simulated.dump();
  } catch (const std::exception &e) {
    return wallet_helpers::make_error_json(
        std::string("Failed to parse prompt JSON: ") + e.what());
  }
}

} // namespace wallet_approval
