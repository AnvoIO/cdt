# Anvo Network CDT Roadmap

## Completed

### Apple Silicon / AArch64 Support
Native compilation now supports both x86_64 and AArch64, covering Apple Silicon (macOS) and Linux ARM64. This enables developers on modern hardware to build and test contracts natively without cross-compilation.

### `core_net::` Namespace
CDT ships a new `core_net::` namespace alongside the legacy `eosio::` namespace. New contracts can use `#include <core_net/core_net.hpp>` and `using namespace core_net;`. The legacy `eosio::` headers remain available for backward compatibility. Compiler attributes (`[[eosio::contract]]`, `[[eosio::action]]`, `[[eosio::table]]`) are unchanged.

### Vanilla Clang/LLVM
Removed custom extensions to Clang/LLVM where possible, allowing CDT to track closer to upstream LLVM releases. This simplifies maintenance and enables future LLVM upgrades.

### Anvo Network Rebrand
Rebranded from the upstream Antelope CDT. CDT now targets [Anvo Core](https://github.com/Anvo-Network/core) and ships under the [Anvo-Network](https://github.com/Anvo-Network/cdt) GitHub organization.

## In Progress / Planned

### LLVM Modernization
Evaluate upgrading from the current Clang 9 base to a newer LLVM release (16+). A newer LLVM would bring improved optimizations, better diagnostics, and broader C++20/23 support for contract code.

### C++20 Contract Compilation
Allow contracts to compile with C++20 language features. This removes the need for workarounds in upstream dependencies that assume C++20 support.
