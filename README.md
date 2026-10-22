## Evice Wallet Core

> Logos Basecamp Core Module wrapping the Evice LEZ Wallet SDK (C-ABI FFI) for key management, multi-account, asset transfers, dApp Provider SDK, Identity Accountability, and LNS resolution.

## Architecture & Modular System Design

Unlike monolithic modules, `e-wallet-core` uses a **domain-modular coordinator architecture**:
- **`src/e_wallet_core_impl.{h,cpp}`**: Acts as the master lifecycle coordinator and Universal Interface router, delegating domain operations to dedicated sub-modules.
- **Domain Sub-modules**: Focused, single-responsibility units that interface with the underlying C-ABI FFI:
  1. `wallet_engine` — Engine lifecycle (initialization, BIP39 mnemonic creation, restore, lock, unlock, SDK info).
  2. `wallet_accounts` — Multi-account management (public & private shielded accounts, labeling, balance queries).
  3. `wallet_transfers` — Native & token transfers, pending status queries, request cancellation.
  4. `wallet_sessions` — dApp Provider SDK session handshake, account access permissions, and session revocation.
  5. `wallet_approval` — Interactive approval callback prompts and effect summary simulations.
  6. `wallet_identity` — Identity Accountability (ZK commitments, scoped anti-Sybil nullifiers, attestations, Two-Tier Shamir Secret Sharing).
  7. `wallet_names` — LNS `.lez` name resolution.
  8. `wallet_contracts` — Generic smart contract interaction proposals.
- **`src/wallet_helpers.h`**: RAII string wrappers (`consume_ffi_string`), JSON formatters, and hex encoders for leak-free C-ABI interop.

## Core Capabilities

| Capability Domain | Sub-module | Description |
|-------------------|------------|-------------|
| **Key & Engine Lifecycle** | `wallet_engine` | Encrypted storage initialization, BIP39 mnemonic creation/restoration, memory lock/unlock |
| **Account Management** | `wallet_accounts` | Public & shielded private account derivation, custom labeling, and balance queries |
| **Asset Transfers** | `wallet_transfers` | LEZ native asset and fungible token program transfers, request cancellation |
| **Provider SDK & Sessions** | `wallet_sessions` | dApp connection handshakes, scoped account access permissions, session revocation |
| **Interactive Approvals** | `wallet_approval` | User approval prompt callbacks and transaction effect summaries |
| **Identity Accountability** | `wallet_identity` | Zero-knowledge identity commitments, anti-Sybil scoped nullifiers, on-chain registration |
| **Name Resolution** | `wallet_names` | Human-readable `.lez` domain resolution |
| **Smart Contracts** | `wallet_contracts` | Generic contract invocation proposals with calldata encoding |

## Building & Testing

### Vendored FFI Dependencies

The module links directly against the pre-built C-ABI dynamic library and headers in `lib/`:
- `lib/lez_wallet_sdk.h`
- `lib/liblez_wallet_sdk.so`

### Running Integration Tests

Compile and run the test suite:

```bash
g++ -std=c++20 \
  tests/test_e_wallet_core.cpp \
  src/e_wallet_core_impl.cpp \
  src/wallet_engine.cpp \
  src/wallet_accounts.cpp \
  -Isrc \
  -Ilib \
  -Llib \
  -llez_wallet_sdk \
  -Wl,-rpath,lib \
  -o tests/test_e_wallet_core_runner && ./tests/test_e_wallet_core_runner
```

### Building via Nix (`logos-module-builder`)

```bash
nix build .# --no-link --print-out-paths
```

## License

Dual-licensed under [MIT](LICENSE-MIT) and [Apache-2.0](LICENSE-APACHE).
