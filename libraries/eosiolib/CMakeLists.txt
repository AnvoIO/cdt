file(GLOB HEADERS "*.hpp"
                  "*.h")

add_library(core_net
            eosiolib.cpp
            crypto.cpp
            base64.cpp
            ${HEADERS})

add_library(core_net_malloc
            malloc.cpp
            ${HEADERS})

add_library(core_net_dsm
            simple_malloc.cpp
            ${HEADERS})

add_library(core_net_cmem
            memory.cpp
            ${HEADERS})


set_target_properties(core_net_malloc PROPERTIES LINKER_LANGUAGE C)

target_include_directories(core_net PUBLIC
                                 ${CMAKE_SOURCE_DIR}/libc/cdt-musl/include
                                 ${CMAKE_SOURCE_DIR}/libc/cdt-musl/src/internal
                                 ${CMAKE_SOURCE_DIR}/libc/cdt-musl/src/crypt
                                 ${CMAKE_SOURCE_DIR}/libc/cdt-musl/arch/eos
                                 ${CMAKE_SOURCE_DIR}/libc++/cdt-libcxx/include
                                 ${CMAKE_SOURCE_DIR})

target_link_libraries( core_net c c++ )
add_custom_command( TARGET core_net POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:core_net> ${BASE_BINARY_DIR}/lib )
add_custom_command( TARGET core_net_malloc POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:core_net_malloc> ${BASE_BINARY_DIR}/lib )
add_custom_command( TARGET core_net_dsm POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:core_net_dsm> ${BASE_BINARY_DIR}/lib )
add_custom_command( TARGET core_net_cmem POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:core_net_cmem> ${BASE_BINARY_DIR}/lib )

if (ENABLE_NATIVE_COMPILER)
   add_native_library(native_core_net
                      eosiolib.cpp
                      crypto.cpp
                      malloc.cpp
                      base64.cpp
                      ${HEADERS})

   add_dependencies( native_core_net core_net )
   add_custom_command( TARGET native_core_net POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:native_core_net> ${BASE_BINARY_DIR}/lib )
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../eosiolib DESTINATION ${BASE_BINARY_DIR}/include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
