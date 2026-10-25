#include "e_wallet_core_impl.h"
#include "wallet_engine.h"
#include "wallet_accounts.h"
#include "wallet_transfers.h"
#include "wallet_sessions.h"
#include "wallet_approval.h"
#include "wallet_identity.h"
#include "wallet_names.h"
#include "wallet_contracts.h"
#include "wallet_helpers.h"
#include <iostream>

EWalletCoreImpl::EWalletCoreImpl()
    : m_engine(nullptr), m_initialized(false)
{
}

EWalletCoreImpl::~EWalletCoreImpl()
{
    if (m_engine) {
        wallet_engine::destroy_engine(m_engine);
        m_engine = nullptr;
    }
}

void EWalletCoreImpl::onContextReady()
{
    // Context lifecycle hook called when host is ready.
    std::cout << "[e-wallet-core] Module context ready." << std::endl;
}

// DOMAIN 1: ENGINE LIFECYCLE (DELEGATED TO WALLET_ENGINE)

std::string EWalletCoreImpl::initEngine(const std::string& dataDir, const std::string& password)
{
    if (m_engine) {
        wallet_engine::destroy_engine(m_engine);
        m_engine = nullptr;
        m_initialized = false;
    }

    m_dataDir = dataDir;
    m_engine = wallet_engine::init_engine(dataDir, password);
    if (!m_engine) {
        return wallet_helpers::make_error_json("Failed to initialize wallet engine at path: " + dataDir);
    }

    m_initialized = true;
    nlohmann::json res = {
        {"ok", true},
        {"dataDir", dataDir},
        {"message", "Wallet engine initialized successfully"}
    };
    return res.dump();
}

std::string EWalletCoreImpl::createWallet(const std::string& password)
{
    return wallet_engine::create_wallet(m_engine, password);
}

std::string EWalletCoreImpl::restoreWallet(const std::string& mnemonic, const std::string& password)
{
    return wallet_engine::restore_wallet(m_engine, mnemonic, password);
}

std::string EWalletCoreImpl::lockWallet()
{
    return wallet_engine::lock_wallet(m_engine);
}

std::string EWalletCoreImpl::unlockWallet(const std::string& password)
{
    return wallet_engine::unlock_wallet(m_engine, password);
}

std::string EWalletCoreImpl::getSdkInfo()
{
    return wallet_engine::get_sdk_info(m_engine);
}

bool EWalletCoreImpl::isInitialized() const
{
    return m_initialized && (m_engine != nullptr);
}

// DOMAIN 2: MULTI-ACCOUNT MANAGEMENT (DELEGATED TO WALLET_ACCOUNTS)

std::string EWalletCoreImpl::createPublicAccount(const std::string& label)
{
    return wallet_accounts::create_public_account(m_engine, label);
}

std::string EWalletCoreImpl::createPrivateAccount(const std::string& label)
{
    return wallet_accounts::create_private_account(m_engine, label);
}

std::string EWalletCoreImpl::listAllAccounts()
{
    return wallet_accounts::list_all_accounts(m_engine);
}

std::string EWalletCoreImpl::setAccountLabel(const std::string& accountId, const std::string& label, bool isPrivate)
{
    return wallet_accounts::set_account_label(m_engine, accountId, label, isPrivate);
}

std::string EWalletCoreImpl::getBalance(const std::string& sessionId, const std::string& accountId)
{
    return wallet_accounts::get_balance(m_engine, sessionId, accountId);
}

// DOMAIN 3: ASSET TRANSFERS (DELEGATED TO WALLET_TRANSFERS)

std::string EWalletCoreImpl::proposeNativeTransfer(const std::string& sessionId,
                                                  const std::string& fromAccount,
                                                  const std::string& toAccountOrName,
                                                  const std::string& amountStr)
{
    return wallet_transfers::propose_native_transfer(m_engine, sessionId, fromAccount, toAccountOrName, amountStr);
}

std::string EWalletCoreImpl::proposeTokenTransfer(const std::string& sessionId,
                                                 const std::string& fromHolding,
                                                 const std::string& toHolding,
                                                 const std::string& definitionId,
                                                 const std::string& amountStr)
{
    return wallet_transfers::propose_token_transfer(m_engine, sessionId, fromHolding, toHolding, definitionId, amountStr);
}

std::string EWalletCoreImpl::getRequestStatus(const std::string& requestId)
{
    return wallet_transfers::get_request_status(m_engine, requestId);
}

std::string EWalletCoreImpl::cancelRequest(const std::string& requestId)
{
    return wallet_transfers::cancel_request(m_engine, requestId);
}

// DOMAIN 4: DAPP PROVIDER SDK & SESSIONS (DELEGATED TO WALLET_SESSIONS)

std::string EWalletCoreImpl::connectDApp(const std::string& callerIdentity, uint64_t apiVersion)
{
    return wallet_sessions::connect(m_engine, callerIdentity, apiVersion);
}

std::string EWalletCoreImpl::disconnectDApp(const std::string& sessionId)
{
    return wallet_sessions::disconnect(m_engine, sessionId);
}

std::string EWalletCoreImpl::requestAccounts(const std::string& sessionId)
{
    return wallet_sessions::request_accounts(m_engine, sessionId);
}

std::string EWalletCoreImpl::listSessionAccounts(const std::string& sessionId)
{
    return wallet_sessions::list_accounts(m_engine, sessionId);
}

std::string EWalletCoreImpl::grantAccountAccess(const std::string& sessionId,
                                               const std::string& accountId,
                                               const std::string& grantKind)
{
    return wallet_sessions::grant_account(m_engine, sessionId, accountId, grantKind);
}

std::string EWalletCoreImpl::listSessions()
{
    return wallet_sessions::list_sessions(m_engine);
}

std::string EWalletCoreImpl::revokeSession(const std::string& sessionId)
{
    return wallet_sessions::revoke_session(m_engine, sessionId);
}

// DOMAIN 5: INTERACTIVE APPROVALS & EFFECT SIMULATION (DELEGATED TO WALLET_APPROVAL)

std::string EWalletCoreImpl::setApprovalCallback(FfiApprovalCallback callback, void* userData)
{
    return wallet_approval::set_approval_callback(m_engine, callback, userData);
}

std::string EWalletCoreImpl::submitApprovalDecision(const std::string& requestId, int64_t decision)
{
    return wallet_approval::submit_approval_decision(m_engine, requestId, decision);
}

std::string EWalletCoreImpl::simulatePromptEffect(const std::string& promptJson)
{
    return wallet_approval::simulate_prompt_effect(promptJson);
}

// DOMAIN 6: IDENTITY ACCOUNTABILITY (DELEGATED TO WALLET_IDENTITY)

std::string EWalletCoreImpl::requestIdentityAccess(const std::string& sessionId, const std::string& grantKind)
{
    return wallet_identity::request_identity_access(m_engine, sessionId, grantKind);
}

std::string EWalletCoreImpl::getIdentityCommitment(const std::string& sessionId)
{
    return wallet_identity::get_identity_commitment(m_engine, sessionId);
}

std::string EWalletCoreImpl::getIdentityCommitmentBytes(const std::string& sessionId)
{
    return wallet_identity::get_identity_commitment_bytes(m_engine, sessionId);
}

std::string EWalletCoreImpl::deriveScopedNullifier(const std::string& sessionId, const std::string& scopeId)
{
    return wallet_identity::derive_scoped_nullifier(m_engine, sessionId, scopeId);
}

std::string EWalletCoreImpl::signIdentityAttestation(const std::string& sessionId, const std::string& scopeId, const std::string& messageHex)
{
    return wallet_identity::sign_identity_attestation(m_engine, sessionId, scopeId, messageHex);
}

std::string EWalletCoreImpl::prepareIdentityRegistration(const std::string& sessionId, const std::string& scopeId, const std::string& nodePubkeysJson, uint64_t thresholdK)
{
    return wallet_identity::prepare_identity_registration(m_engine, sessionId, scopeId, nodePubkeysJson, thresholdK);
}

// DOMAIN 7: HUMAN-READABLE LNS NAMES (DELEGATED TO WALLET_NAMES)

std::string EWalletCoreImpl::resolveName(const std::string& sessionId, const std::string& name)
{
    return wallet_names::resolve_name(m_engine, sessionId, name);
}

// DOMAIN 8: SMART CONTRACT INVOCATIONS (DELEGATED TO WALLET_CONTRACTS)

std::string EWalletCoreImpl::proposeTransaction(const std::string& sessionId, const std::string& mentionsJson, const std::string& calldataHex, const std::string& description)
{
    return wallet_contracts::propose_transaction(m_engine, sessionId, mentionsJson, calldataHex, description);
}

