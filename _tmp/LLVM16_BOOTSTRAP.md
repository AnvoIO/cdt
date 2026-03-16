# LLVM 16 Migration — Bootstrap for Next Session

## Status
Branch `llvm16-migration` at `/opt/dev/cdt`. LLVM 16.0.6 builds, all CDT tools compile, but wasm-ld crashes when linking contracts.

## The Blocker
`wasm-ld` crashes at `FunctionSymbol::getFunctionIndex()` during the WASM section write phase. The synthetic `apply` function we create in Driver.cpp doesn't get a proper function index because it's not integrated through the standard `WasmSym` pipeline.

Our debug `llvm::errs()` output in `createDispatchFunction()` never appears — the crash happens BEFORE our Writer code runs, during the WASM output phase when it tries to emit the synthetic apply function that has no proper index.

## The Fix (not yet implemented)
Follow exactly how `WasmSym::callCtors` works end-to-end in LLVM 16:

### Step 1: Declare in Symbols.h
```
// File: llvm/lld/wasm/Symbols.h
// Look for: static DefinedFunction *callCtors;
// Add:      static DefinedFunction *applyDispatch;
```

### Step 2: Initialize in Symbols.cpp
```
// File: llvm/lld/wasm/Symbols.cpp
// Look for: DefinedFunction *WasmSym::callCtors;
// Add:      DefinedFunction *WasmSym::applyDispatch;
```

### Step 3: Create in Writer.cpp createSyntheticInitFunctions()
```
// File: cdt-llvm-extensions/lld/wasm/Writer.cpp
// In createSyntheticInitFunctions(), after callCtors creation:
// Create the apply dispatch function with (i64,i64,i64)->void signature
// This runs BEFORE assignIndexes(), so it gets a proper function index
```

### Step 4: Fill body in createDispatchFunction()
```
// In Writer::createDispatchFunction(), use WasmSym::applyDispatch
// instead of looking up "apply" by name
// Call createFunction(WasmSym::applyDispatch, bodyContent)
```

### Step 5: Remove the ad-hoc creation from Driver.cpp
The Driver.cpp synthetic apply creation (lines ~1133-1142) should be replaced with a reference to WasmSym::applyDispatch.

## Key Files to Read
1. `llvm/lld/wasm/Symbols.h` — WasmSym static members (pattern to follow)
2. `llvm/lld/wasm/Symbols.cpp` — WasmSym initialization
3. `cdt-llvm-extensions/lld/wasm/Writer.cpp:1063` — createSyntheticInitFunctions (where to create)
4. `cdt-llvm-extensions/lld/wasm/Writer.cpp:1584` — createDispatchFunction (our dispatch code)
5. `cdt-llvm-extensions/lld/wasm/Driver.cpp:1125` — current synthetic apply (to be refactored)

## Also Need Extension Files For
- `cdt-llvm-extensions/lld/wasm/Symbols.h` — add WasmSym::applyDispatch
- `cdt-llvm-extensions/lld/wasm/Symbols.cpp` — add initialization
- Update `LLVMPatch.txt` to copy these two new files

## How to Test
```bash
cd /opt/dev/cdt
git checkout llvm16-migration
# After changes, patch manually since cmake configure already ran:
cp cdt-llvm-extensions/lld/wasm/Writer.cpp llvm/lld/wasm/Writer.cpp
cp cdt-llvm-extensions/lld/wasm/Driver.cpp llvm/lld/wasm/Driver.cpp
cp cdt-llvm-extensions/lld/wasm/Symbols.h llvm/lld/wasm/Symbols.h
cp cdt-llvm-extensions/lld/wasm/Symbols.cpp llvm/lld/wasm/Symbols.cpp
cd build/llvm/llvm && make -j$(nproc) lld
# Then test:
/opt/dev/cdt/build/bin/cdt-cpp -abigen -contract hello -o /tmp/test.wasm /tmp/test_llvm16_hello.cpp
```

## Test Contract (already at /tmp/test_llvm16_hello.cpp)
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

## After It Links Successfully
1. Verify WASM output is valid: `wasm-objdump -h /tmp/test.wasm`
2. Compare with LLVM 9 output: should have apply export
3. Run CDT unit tests: `cd build && ctest -L unit_tests`
4. Run toolchain tests: `cd build && ctest -L toolchain_tests`
5. Test on ARM: push branch, pull on ubuntu@34.213.225.55, rebuild, test
6. Apply our rebrand (core_net:: headers, docs) on top of the llvm16 branch
7. Merge to main

## AWS ARM Machine
- `ssh -i ~/.ssh/id_ed25519 ubuntu@34.213.225.55`
- CDT repo at `/home/ubuntu/cdt`
- 8 cores, 15GB RAM, Ubuntu 24.04 aarch64
