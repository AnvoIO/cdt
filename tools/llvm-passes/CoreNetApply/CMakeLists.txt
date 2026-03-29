# CoreNetApply pass: injects ctor/dtor calls into apply()/wasm_entry functions.
# Built as a MODULE (shared library plugin) loaded via -fpass-plugin=.

if(NOT LLVM_REQUIRES_RTTI)
  if(NOT LLVM_REQUIRES_EH)
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CoreNetApply.exports)
  endif()
endif()

add_library(LLVMCoreNetApply MODULE CoreNetApply.cpp)
target_include_directories(LLVMCoreNetApply PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(LLVMCoreNetApply PRIVATE ${LLVM_DEFINITIONS})
set_target_properties(LLVMCoreNetApply PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
)
target_compile_options(LLVMCoreNetApply PRIVATE -fno-rtti)
add_custom_command(TARGET LLVMCoreNetApply POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LLVMCoreNetApply> ${CMAKE_BINARY_DIR}/bin/)
