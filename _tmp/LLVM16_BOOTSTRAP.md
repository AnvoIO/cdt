# LLVM 16 Migration — Complete

## Status
Branch `llvm16-migration` at `/opt/dev/cdt`. **All toolchain tests pass (29/29).**

LLVM 16.0.6 builds, all CDT tools compile, contracts compile and link to valid WASM, eosio-pp post-processing works, ABI files are generated correctly.

## Next Steps
1. Test on ARM: push branch, pull on ubuntu@34.213.225.55, rebuild, test
2. Apply rebrand (core_net:: headers, docs) on top of the llvm16 branch
3. Merge to main

## AWS ARM Machine
- `ssh -i ~/.ssh/id_ed25519 ubuntu@34.213.225.55`
- CDT repo at `/home/ubuntu/cdt`
- 8 cores, 15GB RAM, Ubuntu 24.04 aarch64
