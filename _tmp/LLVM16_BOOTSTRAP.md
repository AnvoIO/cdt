# LLVM 16 Migration — Status

## Status
Branch `llvm16-migration` at `/opt/dev/cdt`. LLVM 16.0.6 builds, all CDT tools compile, **wasm-ld linking works** — contracts compile and link to valid WASM with the `apply` export.

**Remaining blocker:** `eosio-pp` (post-processor) segfaults on the LLVM 16 WASM output. This is a separate binary from wasm-ld that strips BSS segments and does other post-processing on the .wasm file. It needs to be debugged/fixed for the full cdt-cpp pipeline to work end-to-end.

## What Was Fixed (wasm-ld crash)
Three bugs in the LLVM 16 port of the synthetic apply dispatch function:

### Bug 1: Dangling signature reference
`Driver.cpp` created the synthetic apply function with a **local** `WasmSignature` variable, but `InputFunction` stores the signature as a `const WasmSignature &` (reference). When the local went out of scope, the reference dangled, causing a crash in `TypeSection::registerType` when writing the WASM output.

**Fix:** Made the signature `static` so it persists for the lifetime of the link.

### Bug 2: GC'd runtime symbols
The dispatch function calls `eosio_assert_code` and `eosio_set_contract_name`, but these were not marked live before the `--gc-sections` pass. They were stripped, then `getFunctionIndex()` crashed on the dead symbols.

**Fix:** Added a loop in `Driver.cpp` to mark `eosio_assert_code`, `eosio_set_contract_name`, and `__cxa_finalize` as live before the GC pass.

### Bug 3: Missing hasFunctionIndex guards
Several `getFunctionIndex()` calls in `createDispatchFunction()` had no `hasFunctionIndex()` check, causing assertions/crashes on symbols without assigned indexes.

**Fix:** Added `hasFunctionIndex()` guards, changed unsafe `cast<>` to `dyn_cast_or_null<>`.

## Changed Files
- `cdt-llvm-extensions/lld/wasm/Driver.cpp` — static signature, mark runtime symbols live
- `cdt-llvm-extensions/lld/wasm/Writer.cpp` — safety guards, remove debug output

## Next Steps
1. **Fix eosio-pp** — the post-processor crashes on LLVM 16 WASM output. Debug with:
   ```bash
   /opt/dev/cdt/build/bin/eosio-pp /tmp/test.wasm -o /tmp/test_pp.wasm
   ```
   The WASM file is valid (verified by section parsing), so eosio-pp likely has format assumptions that changed between LLVM 9→16.

2. Run toolchain tests end-to-end once eosio-pp is fixed
3. Test on ARM: push branch, pull on ubuntu@34.213.225.55, rebuild, test
4. Apply rebrand (core_net:: headers, docs) on top of the llvm16 branch
5. Merge to main

## How to Rebuild After Changes
```bash
cd /opt/dev/cdt
cp cdt-llvm-extensions/lld/wasm/Writer.cpp llvm/lld/wasm/Writer.cpp
cp cdt-llvm-extensions/lld/wasm/Driver.cpp llvm/lld/wasm/Driver.cpp
cd build/llvm/llvm && make -j$(nproc) lld
# Important: copy the rebuilt binary to the install location
cp /opt/dev/cdt/build/llvm/llvm/bin/lld /opt/dev/cdt/build/bin/lld
cp /opt/dev/cdt/build/bin/lld /opt/dev/cdt/build/bin/wasm-ld
```

## Test Contract (at /tmp/test_llvm16_hello.cpp)
```cpp
#include <eosio/eosio.hpp>
using namespace eosio;
class [[eosio::contract]] hello : public contract {
public:
   using contract::contract;
   [[eosio::action]]
   void hi(name user) { print("Hello, ", user); }
};
```

## AWS ARM Machine
- `ssh -i ~/.ssh/id_ed25519 ubuntu@34.213.225.55`
- CDT repo at `/home/ubuntu/cdt`
- 8 cores, 15GB RAM, Ubuntu 24.04 aarch64
