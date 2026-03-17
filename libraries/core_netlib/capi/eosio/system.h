/**
 *  @file
 *  @copyright defined in eos/LICENSE
 *
 *  Legacy compat header: declares the old eosio_* intrinsic names so that
 *  contracts targeting legacy chains still produce WASM imports with the
 *  original names.  Contracts targeting Anvo should include <core_net/system.h>
 *  instead.
 */
#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((eosio_wasm_import))
void  eosio_assert( uint32_t test, const char* msg );

__attribute__((eosio_wasm_import))
void  eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len );

__attribute__((eosio_wasm_import))
void  eosio_assert_code( uint32_t test, uint64_t code );

__attribute__((eosio_wasm_import, noreturn))
void eosio_exit( int32_t code );

__attribute__((eosio_wasm_import))
uint64_t  current_time( void );

__attribute__((eosio_wasm_import))
uint32_t  get_block_num( void );

__attribute__((eosio_wasm_import))
bool is_feature_activated( const struct capi_checksum256* feature_digest );

__attribute__((eosio_wasm_import))
capi_name get_sender( void );

#ifdef __cplusplus
}
#endif
