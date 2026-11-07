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

std::string propose_contract_call(FfiSdkEngine* engine,
                                  const std::string& sessionId,
                                  const std::string& programId,
                                  const std::string& payerId,
                                  const std::string& mentionsJson,
                                  const std::string& calldataHex,
                                  const std::string& description)
{
    if (!engine) {
        return wallet_helpers::make_error_json("Engine is not initialized");
    }

    const char* descPtr = description.empty() ? nullptr : description.c_str();

    char* raw = lez_wallet_propose_contract_call(
        engine,
        sessionId.c_str(),
        programId.c_str(),
        payerId.c_str(),
        mentionsJson.c_str(),
        calldataHex.c_str(),
        descPtr
    );
    return wallet_helpers::consume_ffi_string(raw);
}

} // namespace wallet_contracts
