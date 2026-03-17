# What's New in CDT 5.x

CDT 5.x is the first release of the Anvo Network Contract Development Toolkit, forked from AntelopeIO CDT 4.1. It brings a complete rebrand to the Anvo Network identity while maintaining full backward compatibility with existing Antelope-ecosystem contracts.

## Highlights

- **LLVM 16** — Upgraded from LLVM 9, bringing modern C++17/20 support and improved optimization
- **`core_net::` primary namespace** — All SDK types, attributes, and macros use `core_net::` as primary
- **`core.*` system accounts** — Default system account is `core` (not `eosio`)
- **`--system-account` flag** — Compile-time switch for targeting Anvo vs legacy Antelope chains
- **Sync call support** — New `[[core_net::call]]` attribute for synchronous cross-contract calls
- **AArch64 native support** — Full ARM64/AArch64 native compilation and testing

## Breaking Changes

### WASM Intrinsic Function Rename

Contracts compiled with CDT 5.x import new host function names:

| CDT 5.x (new) | CDT 4.x (old) |
|---|---|
| `core_net_assert` | `eosio_assert` |
| `core_net_assert_message` | `eosio_assert_message` |
| `core_net_assert_code` | `eosio_assert_code` |
| `core_net_exit` | `eosio_exit` |
| `core_net_set_contract_name` | `eosio_set_contract_name` |
| `_core_net_f32_add`, `_core_net_f64_add`, ... | `_eosio_f32_add`, `_eosio_f64_add`, ... |

**Impact:** Contracts compiled with CDT 5.x using `core_net/` headers will NOT run on nodes that only export the old `eosio_*` names. They require Anvo Core (which exports both) or a compatible node.

**Mitigation:** Contracts compiled with `eosio/` compat headers continue to use the old names and work on all Antelope nodes.

### System Account Name Change

The default system account in the linker dispatch is now `core` instead of `eosio`. Contracts compiled with CDT 5.x (default settings) expect the system account to be `core`.

**Mitigation:** Pass `--system-account=eosio` when compiling for legacy Antelope chains.

## New Features

### Sync Call Support

CDT 5.x adds the `[[core_net::call]]` attribute for synchronous cross-contract function calls:

```cpp
class [[core_net::contract]] mycontract : public core_net::contract {
   [[core_net::call]]
   uint32_t get_value(uint32_t key);
};
```

### `--system-account` Compiler Flag

Control which system account the dispatch function uses:

```sh
# Anvo Network (default)
cdt-cpp -o mycontract.wasm mycontract.cpp

# Legacy Antelope (EOS, Telos, WAX, etc.)
cdt-cpp --system-account=eosio -o mycontract.wasm mycontract.cpp
```

### LLVM 16 Upgrade

CDT 5.x is built on LLVM 16 (up from LLVM 9 in CDT 4.x), bringing:
- Modern C++17 standard library support in WASM
- Improved code generation and optimization
- Better diagnostics and error messages
- Modules-ts support

## Namespace and Branding Changes

All internal identifiers have been ported from `eosio` to `core_net`:

| Area | CDT 5.x (primary) | CDT 4.x (legacy, still works) |
|------|-------------------|-------------------------------|
| Namespace | `core_net::` | `eosio::` |
| Headers | `<core_net/core_net.hpp>` | `<eosio/eosio.hpp>` |
| Attributes | `[[core_net::action]]` | `[[eosio::action]]` |
| Macros | `CORE_NET_DISPATCH` | `EOSIO_DISPATCH` |
| Test macros | `CORE_NET_TEST_BEGIN` | `EOSIO_TEST_BEGIN` |
| System accounts | `"core"_n`, `"core.token"_n` | `"eosio"_n`, `"eosio.token"_n` |
| ABI version | `core_net::abi/1.2` | `eosio::abi/1.1` |
| CLI tools | `core-cli` | `cleos` |
| Wallet daemon | `core-wallet` | `keosd` |
| Node daemon | `core_netd` | `nodeos` |

All legacy names remain available through backward-compatibility aliases. The `eosio/` header directory, `eosio::` namespace, `[[eosio::*]]` attributes, and `EOSIO_*` macros all continue to work and compile to identical WASM bytecode.

## Tool Renames

| CDT 5.x | CDT 4.x | Purpose |
|---------|---------|---------|
| `core-net-pp` | `eosio-pp` | Post-processing tool |
| `core-net-wasm2wast` | `eosio-wasm2wast` | WASM to WAT converter |
| `core-net-wast2wasm` | `eosio-wast2wasm` | WAT to WASM converter |

Legacy tool names are available as symlinks.

## Migration Guide

### For contract developers on Anvo Network

Use `core_net/` headers and `core.*` account names:

```cpp
#include <core_net/core_net.hpp>
using namespace core_net;

class [[core_net::contract]] mytoken : public contract {
   [[core_net::action]]
   void transfer(name from, name to, asset quantity, std::string memo) {
      // ...
   }
};

CORE_NET_DISPATCH(mytoken, (transfer))
```

### For contract developers on legacy Antelope chains

Use `eosio/` headers and pass the compat flag:

```cpp
#include <eosio/eosio.hpp>
using namespace eosio;

class [[eosio::contract]] mytoken : public contract {
   [[eosio::action]]
   void transfer(name from, name to, asset quantity, std::string memo) {
      // ...
   }
};

EOSIO_DISPATCH(mytoken, (transfer))
```

Compile with: `cdt-cpp --system-account=eosio -o mytoken.wasm mytoken.cpp`

### Mixing styles

You can use `core_net::` headers with `eosio.*` account names, or vice versa. The namespace choice and account name choice are independent.
