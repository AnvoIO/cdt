# LLVM 16 Migration — Complete

## Status
Branch `llvm16-migration` merged to `main`. Rebrand (`rebrand/core_net`) also merged to `main`. **All toolchain tests pass (29/29).**

LLVM 16.0.6 builds, all CDT tools compile, contracts compile and link to valid WASM, eosio-pp post-processing works, ABI files are generated correctly. Tested on both x86_64 and aarch64.

## Completed
1. LLVM 16 migration — merged to main
2. Rebrand (core_net:: wrapper headers) — merged to main
3. ARM testing — passed on ubuntu@34.213.225.55

## Next
- Sync call port (`feature/sync-call-llvm16`) — see `_tmp/SYNC_CALL_PORT.md`

## AWS ARM Machine
- `ssh -i ~/.ssh/id_ed25519 ubuntu@34.213.225.55`
- CDT repo at `/home/ubuntu/cdt`
- 8 cores, 15GB RAM, Ubuntu 24.04 aarch64
