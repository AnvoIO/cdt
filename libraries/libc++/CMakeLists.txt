# libc++ build for CDT WASM smart contracts
# Uses LLVM 16's libc++ from the monorepo submodule with a thin patch overlay.

# Navigate from libraries/libc++/ up to the CDT root, then into llvm/libcxx
set(CDT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
get_filename_component(CDT_ROOT ${CDT_ROOT} ABSOLUTE)
set(LIBCXX_ROOT ${CDT_ROOT}/llvm/libcxx)
set(LIBCXX_PATCHES ${CMAKE_CURRENT_SOURCE_DIR}/patches)

# Apply patches: overlay our files on top of upstream
# __config_site: WASM-specific feature flags
file(COPY_FILE ${LIBCXX_PATCHES}/__config_site ${LIBCXX_ROOT}/include/__config_site)
# __assertion_handler: required by LLVM 18 libc++ (normally CMake-generated)
file(COPY_FILE ${LIBCXX_PATCHES}/__assertion_handler ${LIBCXX_ROOT}/include/__assertion_handler)
# iostream: block usage in smart contracts
file(COPY_FILE ${LIBCXX_PATCHES}/iostream ${LIBCXX_ROOT}/include/iostream)

# Source files from upstream libc++
# With _LIBCPP_HAS_NO_LOCALIZATION, we skip ios/iostream/locale/regex/strstream
SET(LIBCXX_SOURCES
   ${LIBCXX_ROOT}/src/any.cpp
   ${LIBCXX_ROOT}/src/bind.cpp
   ${LIBCXX_ROOT}/src/functional.cpp
   ${LIBCXX_ROOT}/src/hash.cpp
   ${LIBCXX_ROOT}/src/legacy_pointer_safety.cpp
   ${LIBCXX_ROOT}/src/memory.cpp
   ${LIBCXX_ROOT}/src/optional.cpp
   ${LIBCXX_ROOT}/src/random_shuffle.cpp
   ${LIBCXX_ROOT}/src/stdexcept.cpp
   ${LIBCXX_ROOT}/src/string.cpp
   ${LIBCXX_ROOT}/src/system_error.cpp
   ${LIBCXX_ROOT}/src/typeinfo.cpp
   ${LIBCXX_ROOT}/src/valarray.cpp
   ${LIBCXX_ROOT}/src/variant.cpp
   ${LIBCXX_ROOT}/src/vector.cpp
)

# Our patched/custom source files
SET(PATCH_SOURCES
   ${LIBCXX_PATCHES}/chrono.cpp
   ${LIBCXX_PATCHES}/verbose_abort.cpp
   ${LIBCXX_PATCHES}/exception.cpp
   ${LIBCXX_PATCHES}/new.cpp
   ${LIBCXX_PATCHES}/core_net.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-everything")

add_library(c++
            ${LIBCXX_SOURCES}
            ${PATCH_SOURCES})

target_include_directories(c++
                           PUBLIC
                           ${LIBCXX_ROOT}/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/src/internal
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/src/crypt
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/arch/wasm32
                           PRIVATE
                           ${LIBCXX_ROOT}/src)

target_link_libraries(c++ c)
target_compile_definitions(c++ PUBLIC -D_LIBCPP_BUILDING_LIBRARY)
add_custom_command( TARGET c++ POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:c++> ${BASE_BINARY_DIR}/lib )

if (ENABLE_NATIVE_COMPILER)
   add_native_library(native_c++
               ${LIBCXX_SOURCES}
               ${PATCH_SOURCES})

   target_include_directories(native_c++
                              PUBLIC
                              ${LIBCXX_ROOT}/include
                              ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/include
                              ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/src/internal
                              ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/src/crypt
                              ${CMAKE_CURRENT_SOURCE_DIR}/../libc/cdt-musl/arch/wasm32
                              PRIVATE
                              ${LIBCXX_ROOT}/src)

   target_link_libraries(native_c++ native_c)
   target_compile_definitions(native_c++ PUBLIC -D_LIBCPP_BUILDING_LIBRARY)
   target_compile_options(native_c++ PUBLIC -allow-sse)
   add_custom_command( TARGET native_c++ POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:native_c++> ${BASE_BINARY_DIR}/lib )
endif()

# Install headers — copy entire include tree (873 files with modularization)
file(COPY ${LIBCXX_ROOT}/include/ DESTINATION ${BASE_BINARY_DIR}/include/libcxx
     PATTERN "CMakeLists.txt" EXCLUDE
     PATTERN "module.modulemap.in" EXCLUDE)
