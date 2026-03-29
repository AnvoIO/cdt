# CoreNetApply pass: injects ctor/dtor calls into apply()/wasm_entry functions.
# Built as a MODULE (shared library plugin) loaded via -fpass-plugin=.

if(NOT LLVM_REQUIRES_RTTI)
  if(NOT LLVM_REQUIRES_EH)
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CoreNetApply.exports)
  endif()
endif()

# Use a distinct target name to avoid collision with IMPORTED targets from the
# LLVM build tree cmake config, but produce the expected output filename.
add_library(cdt_pass_apply MODULE CoreNetApply.cpp)
target_include_directories(cdt_pass_apply PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(cdt_pass_apply PRIVATE ${LLVM_DEFINITIONS})
set_target_properties(cdt_pass_apply PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
  OUTPUT_NAME "LLVMCoreNetApply"
)
target_compile_options(cdt_pass_apply PRIVATE -fno-rtti)
add_custom_command(TARGET cdt_pass_apply POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cdt_pass_apply> ${CMAKE_BINARY_DIR}/bin/)
