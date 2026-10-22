#include "e_wallet_core_impl.h"
#include "wallet_engine.h"
#include "wallet_accounts.h"
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
    // Basecamp modules can auto-initialize or await explicit initEngine() calls.
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
