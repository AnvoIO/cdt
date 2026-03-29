# CoreNetSoftfloat pass: replaces hardware float ops with deterministic software float calls.
# Built as a MODULE (shared library plugin) loaded via -fpass-plugin=.

if(NOT LLVM_REQUIRES_RTTI)
  if(NOT LLVM_REQUIRES_EH)
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CoreNetSoftfloat.exports)
  endif()
endif()

add_library(LLVMCoreNetSoftfloat MODULE CoreNetSoftfloat.cpp)
target_include_directories(LLVMCoreNetSoftfloat PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(LLVMCoreNetSoftfloat PRIVATE ${LLVM_DEFINITIONS})
set_target_properties(LLVMCoreNetSoftfloat PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
)
target_compile_options(LLVMCoreNetSoftfloat PRIVATE -fno-rtti)
add_custom_command(TARGET LLVMCoreNetSoftfloat POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LLVMCoreNetSoftfloat> ${CMAKE_BINARY_DIR}/bin/)
