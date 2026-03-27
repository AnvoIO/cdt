# Submodule Dependency Pins

CDT depends on four external submodules. These are pinned to specific commits
and should only be updated deliberately after testing.

**Do not run `git submodule update --remote`** — this would pull untested
upstream changes. Always pin to a specific tested commit.

| Submodule | Upstream | Pinned Commit | Based On | Notes |
|---|---|---|---|---|
| `llvm` | `llvm/llvm-project` | LLVM 16 | LLVM 16 monorepo | CDT extensions via `cdt-llvm-extensions/` overlay |
| `libraries/libc/cdt-musl` | `AntelopeIO/cdt-musl` | `9e6b206` | musl libc ~1.1 | WASM-targeted C standard library |
| `libraries/libc++/cdt-libcxx` | `AntelopeIO/cdt-libcxx` | `dd2d7b0` | libc++ from LLVM 9 era | WASM-targeted C++ standard library |
| `libraries/native/softfloat` | `AnvoIO/berkeley-softfloat-3` | `49495ce` | Berkeley SoftFloat 3e+ | Direct fork of ucb-bar upstream with EOSIO/AntelopeIO additions cherry-picked |

## Updating a submodule

To update a submodule to a new commit:

```bash
cd <submodule-path>
git fetch origin
git checkout <new-commit-hash>
cd /path/to/cdt
git add <submodule-path>
git commit -m "Update <submodule> to <commit> — <reason>"
```

Always rebuild and run the full test suite on both x86_64 and AArch64 after
updating any submodule.
