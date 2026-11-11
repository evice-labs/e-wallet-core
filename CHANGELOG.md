## Changelog

All notable changes to the Evice Wallet Core Module are documented in this file.

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

