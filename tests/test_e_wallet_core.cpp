#include <iostream>
#include <cassert>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "e_wallet_core_impl.h"

using json = nlohmann::json;

void test_uninitialized_guard() {
    std::cout << "[Test 1] Testing uninitialized engine guards...\n";
    EWalletCoreImpl wallet;
    assert(!wallet.isInitialized());

    std::string res = wallet.createWallet("password123");
    auto j = json::parse(res);
    assert(j.contains("ok") && j["ok"] == false);
    assert(j.contains("error"));
    std::cout << "  ✓ Uninitialized guards passed: " << j["error"] << "\n";
}

void test_engine_lifecycle_and_accounts() {
    std::cout << "[Test 2] Testing engine lifecycle and account management...\n";

    std::string testDir = "/tmp/test_wallet_core_" + std::to_string(time(nullptr));
    std::filesystem::create_directories(testDir);

    EWalletCoreImpl wallet;
    std::string initRes = wallet.initEngine(testDir, "testpass123");
    auto initJson = json::parse(initRes);
    assert(initJson["ok"] == true);
    assert(wallet.isInitialized());
    std::cout << "  ✓ Engine initialized successfully\n";

    // Query SDK info
    std::string infoRes = wallet.getSdkInfo();
    auto infoJson = json::parse(infoRes);
    assert(infoJson["ok"] == true);
    std::cout << "  ✓ SDK info verified: " << infoJson.dump() << "\n";

    // Create fresh wallet
    std::string createRes = wallet.createWallet("testpass123");
    auto createJson = json::parse(createRes);
    assert(createJson["ok"] == true);
    assert(createJson.contains("mnemonic"));
    std::string mnemonic = createJson["mnemonic"].get<std::string>();
    assert(!mnemonic.empty());
    std::cout << "  ✓ Wallet created with mnemonic words count > 0\n";

    // Create public account
    std::string pubAccRes = wallet.createPublicAccount("Main Spending");
    auto pubAccJson = json::parse(pubAccRes);
    assert(pubAccJson["ok"] == true);
    assert(pubAccJson.contains("account_id"));
    std::string pubAccountId = pubAccJson["account_id"].get<std::string>();
    std::cout << "  ✓ Public account created: " << pubAccountId << "\n";

    // Create private account
    std::string privAccRes = wallet.createPrivateAccount("Shielded Vault");
    auto privAccJson = json::parse(privAccRes);
    assert(privAccJson["ok"] == true);
    assert(privAccJson.contains("account_id"));
    std::string privAccountId = privAccJson["account_id"].get<std::string>();
    std::cout << "  ✓ Private account created: " << privAccountId << "\n";

    // List all accounts
    std::string listRes = wallet.listAllAccounts();
    auto listJson = json::parse(listRes);
    assert(listJson["ok"] == true);
    assert(listJson.contains("accounts"));
    assert(listJson["accounts"].is_array());
    assert(listJson["accounts"].size() >= 2);
    std::cout << "  ✓ Listed " << listJson["accounts"].size() << " accounts successfully\n";

    // Update label
    std::string labelRes = wallet.setAccountLabel(pubAccountId, "Primary Public Account", false);
    auto labelJson = json::parse(labelRes);
    assert(labelJson["ok"] == true);
    std::cout << "  ✓ Account label updated successfully\n";

    // Lock wallet
    std::string lockRes = wallet.lockWallet();
    auto lockJson = json::parse(lockRes);
    assert(lockJson["ok"] == true);
    std::cout << "  ✓ Wallet locked successfully\n";

    // Unlock wallet
    std::string unlockRes = wallet.unlockWallet("testpass123");
    auto unlockJson = json::parse(unlockRes);
    assert(unlockJson["ok"] == true);
    std::cout << "  ✓ Wallet unlocked successfully\n";

    // Cleanup temp directory
    std::filesystem::remove_all(testDir);
    std::cout << "  ✓ Test cleanup completed\n";
}

int main() {
    std::cout << "RUNNING CORE MODULE INTEGRATION TESTS\n";
    test_uninitialized_guard();
    test_engine_lifecycle_and_accounts();
    std::cout << "ALL TESTS PASSED!\n";
    return 0;
}
