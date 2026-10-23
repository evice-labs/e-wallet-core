## Evice Wallet Core

> Logos Basecamp Core Module wrapping the Evice LEZ Wallet SDK via modern C++20 and C-ABI FFI. Provides robust capabilities for keystore management, multi-account derivation, asset transfers, dApp Provider SDK handshakes, interactive transaction approvals, and plain-language effect simulation.

## 1. System Architecture & Design Pattern

Following the idiomatic patterns of the Logos Ecosystem (such as `logos-delivery-module`), `e-wallet-core` is architected using a **domain-modular coordinator pattern** (Facade / Mediator pattern). The master coordinator (`EWalletCoreImpl`) acts as the entry point and lifecycle manager, delegating domain-specific logic to modular sub-systems:

```
e-wallet-core/
├── lib/
│   ├── lez_wallet_sdk.h          # C-ABI header definitions from Rust SDK
│   └── liblez_wallet_sdk.so      # Pre-compiled C-ABI shared library
├── src/
│   ├── e_wallet_core_impl.h      # Master Coordinator class declaration (LogosModuleContext)
│   ├── e_wallet_core_impl.cpp    # Master Coordinator implementation (API routing & lifecycle)
│   ├── wallet_engine.h           # Domain: Engine lifecycle & encrypted storage
│   ├── wallet_engine.cpp
│   ├── wallet_accounts.h         # Domain: Public & private account derivation & balances
│   ├── wallet_accounts.cpp
│   ├── wallet_transfers.h        # Domain: Native & token program transfers
│   ├── wallet_transfers.cpp
│   ├── wallet_sessions.h         # Domain: dApp Provider SDK handshake & sessions
│   ├── wallet_sessions.cpp
│   ├── wallet_approval.h         # Domain: Interactive approval prompts & effect simulations
│   ├── wallet_approval.cpp
│   └── wallet_helpers.h          # RAII memory management, JSON helpers & hex utilities
├── tests/
│   └── test_e_wallet_core.cpp    # End-to-end integration test runner
├── metadata.json                 # Logos Module descriptor & build configuration
├── CMakeLists.txt                # Logos module builder CMake definition
└── flake.nix                     # Nix packaging configuration
```

### Core Design Principles
* **Pure Modern C++20**: Standard library types (`std::string`, `std::vector`, `uint64_t`, `int64_t`, `std::unique_ptr`). No hard dependencies on heavyweight GUI frameworks (Qt plugin bindings and LIDL RPC glue are generated automatically by `logos-module-builder`).
* **Master Coordinator (`EWalletCoreImpl`)**: Inherits from `LogosModuleContext` (`#include <logos_module_context.h>`), allowing seamless lifecycle integration within Logos Basecamp. Coordinates active engine handles and dispatches incoming RPC calls.
* **Separation of Concerns**: Each business domain lives in its own translation unit under dedicated namespaces (`wallet_engine`, `wallet_accounts`, `wallet_transfers`, `wallet_sessions`, `wallet_approval`).
* **RAII Memory Safety**: C-ABI string allocations dynamically allocated by Rust FFI (`*mut c_char`) are immediately captured by `wallet_helpers::consume_ffi_string()`, ensuring zero memory leaks and automatic deallocation via `lez_wallet_free_string()`.
* **Defensive Error Handling**: Every public method validates engine handle initialization and argument validity upfront, returning structured JSON error payloads instead of risking null-pointer dereferences or segmentation faults.

## 2. Implemented Capability Domains

### Domain 1: Engine Lifecycle & Encrypted Storage (`wallet_engine`)
Manages the underlying RocksDB/JSON secure storage, master BIP39 seeds, and engine encryption states.

| Method | Parameters | Description |
| :--- | :--- | :--- |
| `initEngine` | `dataDir: string, password: string` | Initializes engine instance and opens persistent storage. |
| `createWallet` | `password: string` | Generates a fresh BIP39 mnemonic and derives master root credentials. |
| `restoreWallet`| `mnemonic: string, password: string` | Restores wallet keystore from a 12 or 24-word recovery phrase. |
| `lockWallet` | *(none)* | Clears sensitive decryption keys and sessions from volatile memory. |
| `unlockWallet` | `password: string` | Authenticates and decrypts keystore credentials. |
| `getSdkInfo` | *(none)* | Queries SDK runtime version, capability flags, and target network. |
| `isInitialized`| *(none)* | Returns `true` if engine is active and unlocked; otherwise `false`. |

### Domain 2: Multi-Account Management & Balances (`wallet_accounts`)
Derives and organizes transparent and shielded accounts, persisting account metadata across sessions.

| Method | Parameters | Description |
| :--- | :--- | :--- |
| `createPublicAccount` | `label: string` | Derives a standard transparent LEZ account address. |
| `createPrivateAccount`| `label: string` | Derives a shielded zero-knowledge account for private transactions. |
| `listAllAccounts` | *(none)* | Returns a JSON array of all registered accounts, labels, and types. |
| `setAccountLabel` | `accountId: string, label: string, isPrivate: bool` | Updates human-readable label for an account. |
| `getBalance` | `sessionId: string, accountId: string` | Returns native balance for an account authorized under the active session. |

### Domain 3: Asset Transfers & Transaction Lifecycle (`wallet_transfers`)
Orchestrates transfer proposal, input validation, and transaction tracking.

| Method | Parameters | Description |
| :--- | :--- | :--- |
| `proposeNativeTransfer` | `sessionId: string, fromAccount: string, toAccountOrName: string, amountStr: string` | Proposes native asset transfer. Validates positive amount and prevents self-transfers before queuing. |
| `proposeTokenTransfer` | `sessionId: string, fromHolding: string, toHolding: string, definitionId: string, amountStr: string` | Proposes transfer of fungible token program assets. |
| `getRequestStatus` | `requestId: string` | Queries current state of an action (`pending_approval`, `approved`, `rejected`, `submitted`, `cancelled`). |
| `cancelRequest` | `requestId: string` | Cancels a pending request prior to execution. |

### Domain 4: dApp Provider SDK & Session Security (`wallet_sessions`)
Provides standard handshake protocols and granular, scoped access controls for external dApps and plugins.

| Method | Parameters | Description |
| :--- | :--- | :--- |
| `connectDApp` | `callerIdentity: string, apiVersion: uint64` | Negotiates protocol handshake and returns an isolated `session_id`. |
| `disconnectDApp` | `sessionId: string` | Gracefully closes an active session. |
| `requestAccounts` | `sessionId: string` | Triggers account discovery workflow for the connected session. |
| `grantAccountAccess` | `sessionId: string, accountId: string, grantKind: string` | Grants scoped permissions (`"read"` for balances, `"propose"` for transactions). |
| `listSessionAccounts` | `sessionId: string` | Lists accounts accessible to the requesting session. |
| `listSessions` | *(none)* | Returns all currently active sessions and their metadata. |
| `revokeSession` | `sessionId: string` | Forcefully revokes session authorizations. |

### Domain 5: Interactive Approvals & Plain-Language Effect Simulation (`wallet_approval`)
Implements human-in-the-loop security, providing clear previews of transaction consequences before signing.

| Method | Parameters | Description |
| :--- | :--- | :--- |
| `submitApprovalDecision`| `requestId: string, decision: int64` | Resolves deferred modal approval (`1` for Approve, `0` for Reject). |
| `simulatePromptEffect` | `promptJson: string` | Synthesizes plain-language summary of intent, caller identity, target assets, verification status, and security warnings. |
| `setApprovalCallback` *(protected / in-process)* | `callback: FfiApprovalCallback, userData: void*` | In-process C-ABI hook for host UI callback registration (`1` = Approve, `0` = Reject, `-1` = Deferred Prompt). |

## 3. Vendored Libraries & Dependency Resolution

Following the standard design pattern of `logos-delivery-module`:

* **`lib/lez_wallet_sdk.h`**: Vendored C-ABI function declarations and struct layouts for the Evice LEZ Wallet Rust SDK.
* **`lib/liblez_wallet_sdk.so`**: Vendored pre-compiled C-ABI shared library implementing cryptography, state machines, and RPC bindings.
* **Logos SDK Headers & Packages**: System headers such as `<logos_module_context.h>` and dependencies (`nlohmann_json`, `pcsclite`) are resolved automatically via Nix and `logos-module-builder` (declared in `metadata.json`), keeping the repository clean and avoiding redundant in-tree vendoring.

## 4. Building & Running Integration Tests

### Building the Logos Basecamp Plugin Package via Nix
To build the complete Logos Basecamp plugin package (`e_wallet_core_plugin.so`, LIDL contracts, and QML bindings):

```bash
nix build .# --no-link -L
```

### Running the Integration Test Suite via Nix Environment
To compile and execute the complete integration test suite within the pinned Nix environment:

```bash
nix develop --command bash -c "g++ -std=c++20 \
  tests/test_e_wallet_core.cpp \
  src/*.cpp \
  -Isrc \
  -Ilib \
  -I\$LOGOS_CPP_SDK_ROOT/include \
  -Llib \
  -llez_wallet_sdk \
  -lpcsclite \
  -Wl,-rpath,lib \
  -o tests/test_e_wallet_core_runner && ./tests/test_e_wallet_core_runner && rm tests/test_e_wallet_core_runner"
```

### Test Suite Execution Plan
The integration test runner executes 4 sequential verification suites:
1. **Uninitialized Engine Guards**: Asserts that all public methods fail safely with descriptive JSON errors when called prior to `initEngine()`.
2. **Engine Lifecycle & Multi-Account Derivation**: Tests wallet creation, mnemonic restoration, public/private account derivation, and lock/unlock cycles.
3. **dApp Provider SDK & Session Security**: Tests connection handshakes, scoped account access grants (`"read"` / `"propose"`), balance query gating, and session revocation.
4. **Asset Transfers, Interactive Approvals & Effect Simulation**: Tests transfer validation (zero amount & self-transfer rejection), approval prompt callback interception, plain-language simulation decoding, and deferred approval submission.

## 5. CI/CD Workflows

Continuous Integration workflows are located in `.github/workflows/`:
* **`ci-linux.yml`**: Validates C-ABI symbol exports on Ubuntu, builds the complete Nix core module package, and executes the native integration test suite inside the pinned Nix environment.
* **`ci-macos.yml`**: Validates building the core module package on macOS.
* **`testnet-smoke.yml`**: Scheduled weekly smoke tests to ensure ongoing compatibility with the active testnet profile.

## 6. License

Dual-licensed under either:
* **Apache License, Version 2.0** ([LICENSE-APACHE](LICENSE-APACHE))
* **MIT License** ([LICENSE-MIT](LICENSE-MIT))
