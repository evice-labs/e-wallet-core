#pragma once

#include <string>
#include <cstdint>
#include "logos_module_context.h"

// Forward declaration of FFI engine struct
struct FfiSdkEngine;

/**
 * @brief Universal Logos Module implementing E-Wallet Core capabilities.
 *
 * Wraps the Evice LEZ Wallet SDK via C-ABI FFI.
 * The Qt plugin glue and QML bindings are generated automatically by logos-module-builder.
 * This class serves as the master coordinator, delegating domain logic
 * to specialised sub-modules (wallet_engine, wallet_accounts, etc.).
 */
class EWalletCoreImpl : public LogosModuleContext
{
public:
    EWalletCoreImpl();
    ~EWalletCoreImpl() override;

    // LogosModuleContext Lifecycle Hook
    void onContextReady() override;

    // Domain 1: Engine Lifecycle (wallet_engine)
    std::string initEngine(const std::string& dataDir, const std::string& password);
    std::string createWallet(const std::string& password);
    std::string restoreWallet(const std::string& mnemonic, const std::string& password);
    std::string lockWallet();
    std::string unlockWallet(const std::string& password);
    std::string getSdkInfo();
    bool isInitialized() const;

    // Domain 2: Multi-Account Management (wallet_accounts)
    std::string createPublicAccount(const std::string& label);
    std::string createPrivateAccount(const std::string& label);
    std::string listAllAccounts();
    std::string setAccountLabel(const std::string& accountId, const std::string& label, bool isPrivate);
    std::string getBalance(const std::string& sessionId, const std::string& accountId);

private:
    FfiSdkEngine* m_engine = nullptr;
    std::string m_dataDir;
    bool m_initialized = false;
};
