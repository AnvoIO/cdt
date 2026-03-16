---
content_title: CDT Migration Guide
---

## CDT 5.0 Changes (Anvo Network)

CDT 5.0 introduces the following changes as part of the Anvo Network project:

1. **`core_net::` namespace**: New contracts can use `#include <core_net/core_net.hpp>` and `using namespace core_net;`. The legacy `eosio::` namespace headers remain available for backward compatibility.
2. **Multi-architecture support**: Native compilation now supports both x86_64 and AArch64 (Apple Silicon and Linux ARM64).
3. **Compiler attributes unchanged**: `[[eosio::contract]]`, `[[eosio::action]]`, and `[[eosio::table]]` remain the same — these are compiler-level attributes and have not been renamed.
4. **SDK macros unchanged**: `EOSIO_DISPATCH`, `EOSIO_TEST_BEGIN`, `EOSIO_TEST_END`, and `EOSIO_TEST` retain their original names.
5. **Node software**: CDT 5.0 targets [Anvo Core](https://github.com/Anvo-Network/core) (formerly Spring / nodeos). Use core-cli (formerly cleos) for contract deployment and interaction.

### Compatibility mode: when to use CDT 4.x instead

Chains still running with the original EOSIO/Antelope protocol naming — using `eosio.*` system accounts (`eosio.token`, `eosio.system`, etc.) and the `eosio::` intrinsic names — should continue to use [AntelopeIO CDT 4.x](https://github.com/AntelopeIO/cdt/releases). This includes EOS, Telos, WAX, UX Network, and any other chain that has not migrated to the Anvo Network protocol naming.

CDT 5.x is specifically for contracts targeting Anvo Network, which uses `core.*` system accounts and the `core_net::` namespace. The WASM bytecode produced by both CDT versions is identical — only the SDK headers and build system differ — so contracts compiled with either version execute correctly on any compatible chain.

---

## Legacy: EOSIO.CDT to CDT 3.0

The section below documents the original migration from the legacy EOSIO.CDT to CDT 3.0.

For that release all of the naming of namespaces and file paths remained as `eosio`, so there was no need to migrate away from that namespace at that time.

But there are a few differences.

1. The tool names have change prefix. I.e. tools like `eosio-cpp` are now `cdt-cpp`.
2. The CMake toolchain file has changed its name from `EosioWasmToolchain.cmake` to `CDTWasmToolchain.cmake`.
3. The library path for the CMake package has changed from `/usr/local/lib/cmake/eosio.cdt` to `/usr/local/lib/cmake/cdt` (or alternatively from `/usr/lib/cmake/eosio.cdt` to `/usr/lib/cmake/cdt` if installing from the Debian package). Additionally, the name to use with CMake `find_package` has similarly changed: use `find_package(cdt)` now instead of `find_package(eosio.cdt)`.
