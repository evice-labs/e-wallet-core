## Changelog

All notable changes to the Evice Wallet Core Module are documented in this file.

## [v0.3.0] - 2026-11-28

### LEZ v0.4.0 & SDK v0.3.0 Synchronization
- **Vendored SDK `v0.3.0` Libraries (`lib/`):**
  - Updated Linux shared library (`lib/x86_64-linux/libe_wallet_sdk.so`) built from `lez-wallet-sdk` `v0.3.0` (`rev 0eca403`), synced with `logos-execution-zone` `v0.4.0` and free of `libpcsclite` linkage.
  - Updated macOS Apple Silicon library (`lib/aarch64-darwin/libe_wallet_sdk.dylib`) from `lez-wallet-sdk` `CI (macOS)` run `46037997043`.
  - Synchronized generated C-ABI header (`lib/e_wallet_sdk.h`) including `e_wallet_get_gas_limit` and `e_wallet_get_transaction_status`.
- **Module Version Bump:**
  - Promoted module version in `metadata.json` from `0.2.4` to `0.3.0`.

## [v0.2.4] - 2026-11-18

### SDK Synchronization & Documentation
- **Vendored SDK `v0.2.4` Libraries (`lib/`):**
  - Updated Linux shared library (`lib/x86_64-linux/libe_wallet_sdk.so`, `651177d`) and macOS arm64 library (`lib/aarch64-darwin/libe_wallet_sdk.dylib`, `4b160a3`) from upstream SDK CI.
  - Updated `README.md` and bumped module version in `metadata.json` from `0.2.3` to `0.2.4`.

## [v0.2.3] - 2026-11-16

### Deferred Approval Queue & Effect Simulation (PR #12)
- **Thread-Safe Deferred Approval Queue (`wallet_approval`, `wallet_engine`, `e_wallet_core_impl`):**
  - Registered `default_approval_callback` automatically on `init_engine` to queue deferred dApp approval prompts.
  - Exposed `listPendingApprovals()` and cleaned up pending queue entries across decision, cancel, lock, and destroy lifecycles.
  - Enriched `simulatePromptEffect()` with `dapp_name`, `effect`, `gas_fee`, `state_diff`, and `contract_call` support, updated `README.md`, and bumped `metadata.json` to `0.2.3` (`67fc6b0`).

## [v0.2.2] - 2026-11-11

### Cross-Platform Compatibility & Windows Support
- **Windows Architecture Vendor Libraries (`lib/x86_64-windows`):**
  - Vendored Windows 64-bit MinGW precompiled C-ABI library artifacts (`e_wallet_sdk.dll`, `libe_wallet_sdk.dll.a`, `e_wallet_sdk.lib`) built from `lez-wallet-sdk` v0.2.2.
  - Follows standard `logos-module-builder` external library layout specification (`lib/<platform>`).
- **Conditional Smartcard Library Resolution (`CMakeLists.txt`):**
  - Implemented CMake condition `if(WIN32) set(SMARTCARD_LIB winscard) else() set(SMARTCARD_LIB pcsclite) endif()` for native Windows PC/SC smartcard subsystem linking (`winscard`) vs Linux (`pcsclite`).
- **Platform Overlays in `metadata.json`:**
  - Configured platform-specific overlays isolating Linux shared libraries (`libpcsclite.so.1`, `libpcsclite.so`) from Windows targets.
  - Added Windows CMake overlay linking `winscard`.
  - Bumped module version to `0.2.2`.

## [v0.2.1] - 2026-11-10

### Features & Reliability
- **Live Transaction Inclusion Querying (`getTransactionStatus`):**
  - Exposed `getTransactionStatus(txHash)` in `EWalletCoreImpl` delegating to `e_wallet_get_transaction_status` in `lez-wallet-sdk`.
  - Enables UI modules to track live on-chain inclusion status without network access in the UI sandbox.

## [v0.2.0] - 2026-11-09

### Initial Stable Release
- Universal C++20 Core Module wrapping Evice Wallet SDK via C-ABI FFI.
- Modular domain coordinators: accounts, transfers, sessions, approvals, identity, names, contracts, engine.
- RAII memory management for FFI string allocations (`consume_ffi_string`).

