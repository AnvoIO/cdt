# Build CDT LLVM transform passes as standalone shared-library plugins.
# These are loaded at compile time via -fpass-plugin= and do not require
# patching the LLVM source tree.

add_subdirectory(CoreNetApply)
add_subdirectory(CoreNetSoftfloat)
