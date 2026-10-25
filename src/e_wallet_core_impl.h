#pragma once

#include <string>
#include <cstdint>
#include <logos_module_context.h>

// Forward declaration of FFI engine and callback types
struct FfiSdkEngine;
typedef int32_t (*FfiApprovalCallback)(const char *prompt_json, void *user_data);

/**
 * @brief Universal Logos Module implementing E-Wallet Core capabilities.
 *
 * Wraps the Evice LEZ Wallet SDK via C-ABI FFI.
 * The Qt plugin glue and QML bindings are generated automatically by logos-module-builder.
 * This class serves as the master coordinator, delegating domain operations
 * to specialised sub-modules (wallet_engine, wallet_accounts, wallet_transfers,
 * wallet_sessions, and wallet_approval).
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

    // Domain 3: Asset Transfers (wallet_transfers)
    std::string proposeNativeTransfer(const std::string& sessionId,
                                      const std::string& fromAccount,
                                      const std::string& toAccountOrName,
                                      const std::string& amountStr);
    std::string proposeTokenTransfer(const std::string& sessionId,
                                     const std::string& fromHolding,
                                     const std::string& toHolding,
                                     const std::string& definitionId,
                                     const std::string& amountStr);
    std::string getRequestStatus(const std::string& requestId);
    std::string cancelRequest(const std::string& requestId);

    // Domain 4: dApp Provider SDK & Sessions (wallet_sessions)
    std::string connectDApp(const std::string& callerIdentity, uint64_t apiVersion);
    std::string disconnectDApp(const std::string& sessionId);
    std::string requestAccounts(const std::string& sessionId);
    std::string listSessionAccounts(const std::string& sessionId);
    std::string grantAccountAccess(const std::string& sessionId,
                                   const std::string& accountId,
                                   const std::string& grantKind);
    std::string listSessions();
    std::string revokeSession(const std::string& sessionId);

    // Domain 5: Interactive Approvals & Effect Simulation (wallet_approval)
    std::string submitApprovalDecision(const std::string& requestId, int64_t decision);
    std::string simulatePromptEffect(const std::string& promptJson);

    // Domain 6: Identity Accountability (wallet_identity)
    std::string requestIdentityAccess(const std::string& sessionId, const std::string& grantKind);
    std::string getIdentityCommitment(const std::string& sessionId);
    std::string getIdentityCommitmentBytes(const std::string& sessionId);
    std::string deriveScopedNullifier(const std::string& sessionId, const std::string& scopeId);
    std::string signIdentityAttestation(const std::string& sessionId, const std::string& scopeId, const std::string& messageHex);
    std::string prepareIdentityRegistration(const std::string& sessionId, const std::string& scopeId, const std::string& nodePubkeysJson, uint64_t thresholdK);

    // Domain 7: Human-Readable LNS Names (wallet_names)
    std::string resolveName(const std::string& sessionId, const std::string& name);

    // Domain 8: Smart Contract Invocations (wallet_contracts)
    std::string proposeTransaction(const std::string& sessionId, const std::string& mentionsJson, const std::string& calldataHex, const std::string& description);

protected:
    // In-process host & test hook for C-ABI callback registration (not an RPC wire method)
    std::string setApprovalCallback(FfiApprovalCallback callback, void* userData);

private:
    FfiSdkEngine* m_engine = nullptr;
    std::string m_dataDir;
    bool m_initialized = false;
};
