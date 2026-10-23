#include <iostream>
#include <cassert>
#include <filesystem>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include "e_wallet_core_impl.h"

using json = nlohmann::json;

// Global prompt capture for testing callbacks
static std::string g_capturedPrompt;

extern "C" int32_t mock_sync_reject_cb(const char* prompt_json, void* user_data) {
    (void)user_data;
    if (prompt_json) {
        g_capturedPrompt = prompt_json;
    }
    return 0; // Reject immediately
}

extern "C" int32_t mock_deferred_cb(const char* prompt_json, void* user_data) {
    (void)user_data;
    if (prompt_json) {
        g_capturedPrompt = prompt_json;
    }
    return -1; // Deferred modal (async user prompt)
}

class TestableWalletCore : public EWalletCoreImpl {
public:
    using EWalletCoreImpl::setApprovalCallback;
};

void test_uninitialized_guard() {
    std::cout << "[Test 1] Testing uninitialized engine guards...\n";
    TestableWalletCore wallet;
    assert(!wallet.isInitialized());

    // 1. Wallet creation guard
    std::string res1 = wallet.createWallet("password123");
    auto j1 = json::parse(res1);
    assert(j1["ok"] == false);
    assert(j1.contains("error"));

    // 2. Transfer guard
    std::string res2 = wallet.proposeNativeTransfer("sess_1", "acc_1", "acc_2", "100");
    auto j2 = json::parse(res2);
    assert(j2["ok"] == false);

    // 3. dApp Connect guard
    std::string res3 = wallet.connectDApp("caller_app", 1);
    auto j3 = json::parse(res3);
    assert(j3["ok"] == false);

    // 4. Approval callback guard
    std::string res4 = wallet.setApprovalCallback(mock_sync_reject_cb, nullptr);
    auto j4 = json::parse(res4);
    assert(j4["ok"] == false);

    std::cout << "  ✓ All uninitialized guards passed\n";
}

void test_engine_lifecycle_and_accounts() {
    std::cout << "[Test 2] Testing engine lifecycle and account management...\n";

    std::string testDir = "/tmp/test_wallet_core_" + std::to_string(time(nullptr)) + "_1";
    std::filesystem::create_directories(testDir);

    EWalletCoreImpl wallet;
    std::string initRes = wallet.initEngine(testDir, "testpass123");
    auto initJson = json::parse(initRes);
    assert(initJson["ok"] == true);
    assert(wallet.isInitialized());

    // Query SDK info
    std::string infoRes = wallet.getSdkInfo();
    auto infoJson = json::parse(infoRes);
    assert(infoJson["ok"] == true);

    // Create fresh wallet
    std::string createRes = wallet.createWallet("testpass123");
    auto createJson = json::parse(createRes);
    assert(createJson["ok"] == true);
    assert(createJson.contains("mnemonic"));

    // Create public account
    std::string pubAccRes = wallet.createPublicAccount("Main Spending");
    auto pubAccJson = json::parse(pubAccRes);
    assert(pubAccJson["ok"] == true);
    std::string pubAccountId = pubAccJson["account_id"].get<std::string>();

    // Create private account
    std::string privAccRes = wallet.createPrivateAccount("Shielded Vault");
    auto privAccJson = json::parse(privAccRes);
    assert(privAccJson["ok"] == true);

    // List all accounts
    std::string listRes = wallet.listAllAccounts();
    auto listJson = json::parse(listRes);
    assert(listJson["ok"] == true);
    assert(listJson["accounts"].size() >= 2);

    // Update label
    std::string labelRes = wallet.setAccountLabel(pubAccountId, "Primary Public Account", false);
    auto labelJson = json::parse(labelRes);
    assert(labelJson["ok"] == true);

    // Lock and unlock
    assert(json::parse(wallet.lockWallet())["ok"] == true);
    assert(json::parse(wallet.unlockWallet("testpass123"))["ok"] == true);

    std::filesystem::remove_all(testDir);
    std::cout << "  ✓ Engine lifecycle and accounts passed\n";
}

void test_dapp_provider_sessions() {
    std::cout << "[Test 3] Testing dApp Provider SDK handshake and session security...\n";

    std::string testDir = "/tmp/test_wallet_core_" + std::to_string(time(nullptr)) + "_2";
    std::filesystem::create_directories(testDir);

    EWalletCoreImpl wallet;
    assert(json::parse(wallet.initEngine(testDir, "testpass123"))["ok"] == true);
    assert(json::parse(wallet.createWallet("testpass123"))["ok"] == true);

    std::string accRes = wallet.createPublicAccount("DApp Account");
    std::string accountId = json::parse(accRes)["account_id"].get<std::string>();

    // 1. Validation: Connect with empty caller identity should fail
    auto emptyConn = json::parse(wallet.connectDApp("", 1));
    assert(emptyConn["ok"] == false);

    // 2. Connect dApp session
    std::string connRes = wallet.connectDApp("basecamp_dex_v1", 1);
    auto connJson = json::parse(connRes);
    assert(connJson["ok"] == true);
    assert(connJson.contains("session_id"));
    std::string sessionId = connJson["session_id"].get<std::string>();
    std::cout << "  ✓ dApp connected with session ID: " << sessionId << "\n";

    // 3. List active sessions
    std::string sessionsRes = wallet.listSessions();
    auto sessionsJson = json::parse(sessionsRes);
    assert(sessionsJson["ok"] == true);
    assert(sessionsJson["sessions"].is_array());
    assert(!sessionsJson["sessions"].empty());
    std::cout << "  ✓ Verified active sessions count: " << sessionsJson["sessions"].size() << "\n";

    // 4. Request accounts
    std::string reqAccRes = wallet.requestAccounts(sessionId);
    assert(json::parse(reqAccRes)["ok"] == true);

    // 5. Grant invalid permission type should be rejected
    auto invalidGrant = json::parse(wallet.grantAccountAccess(sessionId, accountId, "invalid_role"));
    assert(invalidGrant["ok"] == false);

    // 6. Grant 'propose' permission to account
    std::string grantRes = wallet.grantAccountAccess(sessionId, accountId, "propose");
    assert(json::parse(grantRes)["ok"] == true);
    std::cout << "  ✓ Granted 'propose' permission for account\n";

    // 7. List session permitted accounts
    std::string listPermRes = wallet.listSessionAccounts(sessionId);
    auto listPermJson = json::parse(listPermRes);
    assert(listPermJson["ok"] == true);
    assert(listPermJson.contains("accounts"));

    // 8. Query balance for permitted account
    std::string balRes = wallet.getBalance(sessionId, accountId);
    auto balJson = json::parse(balRes);
    assert(balJson["ok"] == true);
    assert(balJson.contains("balance"));
    std::cout << "  ✓ Balance retrieved for granted account: " << balJson["balance"] << "\n";

    // 9. Revoke session
    std::string revokeRes = wallet.revokeSession(sessionId);
    assert(json::parse(revokeRes)["ok"] == true);
    std::cout << "  ✓ Session revoked successfully\n";

    // 10. Disconnect session
    std::string disconnRes = wallet.disconnectDApp(sessionId);
    auto disconnJson = json::parse(disconnRes);
    assert(disconnJson.contains("ok"));

    std::filesystem::remove_all(testDir);
    std::cout << "  ✓ dApp Provider SDK sessions passed\n";
}

void test_transfers_and_approvals() {
    std::cout << "[Test 4] Testing asset transfers, interactive approvals, and effect simulation...\n";

    std::string testDir = "/tmp/test_wallet_core_" + std::to_string(time(nullptr)) + "_3";
    std::filesystem::create_directories(testDir);

    TestableWalletCore wallet;
    assert(json::parse(wallet.initEngine(testDir, "testpass123"))["ok"] == true);
    assert(json::parse(wallet.createWallet("testpass123"))["ok"] == true);

    std::string aliceId = json::parse(wallet.createPublicAccount("Alice"))["account_id"];
    std::string bobId = json::parse(wallet.createPublicAccount("Bob"))["account_id"];

    // Connect dApp and grant propose permissions
    std::string sessId = json::parse(wallet.connectDApp("swap_service", 1))["session_id"];
    assert(json::parse(wallet.grantAccountAccess(sessId, aliceId, "propose"))["ok"] == true);

    // --- Sub-test A: Transfer Input Validations ---
    // Zero amount transfer rejection
    auto zeroTransfer = json::parse(wallet.proposeNativeTransfer(sessId, aliceId, bobId, "0"));
    assert(zeroTransfer["ok"] == false);
    std::cout << "  ✓ Zero transfer rejected: " << zeroTransfer["error"] << "\n";

    // Self transfer rejection
    auto selfTransfer = json::parse(wallet.proposeNativeTransfer(sessId, aliceId, aliceId, "100"));
    assert(selfTransfer["ok"] == false);
    std::cout << "  ✓ Self transfer rejected: " << selfTransfer["error"] << "\n";

    // --- Sub-test B: Synchronous Rejection via Approval Callback ---
    assert(json::parse(wallet.setApprovalCallback(mock_sync_reject_cb, nullptr))["ok"] == true);

    std::string propRes1 = wallet.proposeNativeTransfer(sessId, aliceId, bobId, "500");
    auto propJson1 = json::parse(propRes1);
    assert(propJson1["ok"] == true);
    assert(propJson1.contains("request_id"));
    std::string reqId1 = propJson1["request_id"].get<std::string>();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::string statusRes1 = wallet.getRequestStatus(reqId1);
    auto statusJson1 = json::parse(statusRes1);
    assert(statusJson1["ok"] == true);
    assert(statusJson1["status"] == "rejected");
    std::cout << "  ✓ Interactive rejection prompt handled: status is " << statusJson1["status"] << "\n";

    // --- Sub-test C: Deferred Approval & Effect Simulation ---
    assert(json::parse(wallet.setApprovalCallback(mock_deferred_cb, nullptr))["ok"] == true);
    g_capturedPrompt.clear();

    std::string propRes2 = wallet.proposeNativeTransfer(sessId, aliceId, bobId, "250");
    auto propJson2 = json::parse(propRes2);
    assert(propJson2["ok"] == true);
    std::string reqId2 = propJson2["request_id"].get<std::string>();

    // Wait for async tokio worker to deliver prompt to callback
    for (int i = 0; i < 50 && g_capturedPrompt.empty(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    assert(!g_capturedPrompt.empty());
    std::cout << "  ✓ Captured approval prompt: " << g_capturedPrompt << "\n";

    // Simulate and decode the prompt effects (LP-0021 usability requirement)
    std::string simRes = wallet.simulatePromptEffect(g_capturedPrompt);
    auto simJson = json::parse(simRes);
    assert(simJson["ok"] == true);
    assert(simJson["request_kind"] == "native_transfer");
    assert(simJson.contains("human_description"));
    std::cout << "  ✓ Human-readable effect simulation: " << simJson["human_description"] << "\n";

    // Submit deferred approval decision: Approve (1)
    std::string decisionRes = wallet.submitApprovalDecision(reqId2, 1);
    assert(json::parse(decisionRes)["ok"] == true);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::string statusRes2 = wallet.getRequestStatus(reqId2);
    auto statusJson2 = json::parse(statusRes2);
    assert(statusJson2["ok"] == true);
    std::cout << "  ✓ Deferred approval decision resolved request status: " << statusJson2["status"] << "\n";

    // --- Sub-test D: Request Cancellation ---
    std::string propRes3 = wallet.proposeNativeTransfer(sessId, aliceId, bobId, "75");
    std::string reqId3 = json::parse(propRes3)["request_id"];

    std::string cancelRes = wallet.cancelRequest(reqId3);
    auto cancelJson = json::parse(cancelRes);
    assert(cancelJson["ok"] == true);
    std::cout << "  ✓ Pending request cancelled successfully\n";

    std::filesystem::remove_all(testDir);
    std::cout << "  ✓ Transfers, interactive approvals, and effect simulation passed\n";
}

int main() {
    std::cout << "RUNNING CORE MODULE INTEGRATION TESTS\n";
    test_uninitialized_guard();
    test_engine_lifecycle_and_accounts();
    test_dapp_provider_sessions();
    test_transfers_and_approvals();
    std::cout << "ALL TESTS PASSED!\n";
    return 0;
}
