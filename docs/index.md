# CDT (Contract Development Toolkit)

Contract Development Toolkit (CDT) is a C/C++ toolchain targeting WebAssembly (WASM) and a set of tools to facilitate development of smart contracts written in C/C++ for [Anvo Network](https://github.com/Anvo-Network/) and all Antelope-compatible blockchains. CDT 5.x is a universal compiler — it supports both `core_net::` headers (for Anvo Network) and `eosio::` headers (for compatibility-mode chains like EOS, Telos, WAX, UX, and Libre), producing byte-identical WASM output from either.

In addition to being a general purpose WebAssembly toolchain, specific features and optimizations are available to support building Anvo Network smart contracts. This new toolchain is built around [Clang 9](https://github.com/Anvo-Network/cdt-llvm), which means that CDT inherits the optimizations and analyses from that version of LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

## Upgrading

If you are upgrading to CDT 5.0 from an earlier version, see the [CDT Migration Guide](./04_upgrading/eosio.cdt-to-cdt.md) for details on the `core_net::` namespace, multi-architecture support, and backward compatibility.

If you are migrating from the legacy [EOSIO.CDT](https://github.com/EOSIO/eosio.cdt), that guide also covers the original EOSIO.CDT to CDT 3.0 migration.

## License

[MIT](https://github.com/Anvo-Network/cdt/blob/main/LICENSE)
