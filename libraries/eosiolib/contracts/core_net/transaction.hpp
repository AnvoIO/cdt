/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "action.hpp"
#include "system.hpp"
#include "../../core/core_net/time.hpp"
#include "../../core/core_net/serialize.hpp"

#include <vector>

namespace core_net {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         size_t read_transaction(char*, size_t);

         __attribute__((eosio_wasm_import))
         size_t transaction_size();

         __attribute__((eosio_wasm_import))
         int tapos_block_num();

         __attribute__((eosio_wasm_import))
         int tapos_block_prefix();

         __attribute__((eosio_wasm_import))
         uint32_t expiration();

         __attribute__((eosio_wasm_import))
         int get_action( uint32_t, uint32_t, char*, size_t);

         __attribute__((eosio_wasm_import))
         int get_context_free_data( uint32_t, char*, size_t);
      }
   }

  /**
   *  @defgroup transaction Transaction
   *  @ingroup contracts
   *  @brief Type-safe C++ wrappers for transaction C API
   *
   *  @details See action for the ability to send an inline action.
   *
   *  @note There are some methods from the @ref transactioncapi that can be used directly from C++
   */

  /**
   *  @ingroup transaction
   */
   typedef std::tuple<uint16_t, std::vector<char>> extension;

   /**
    *  @ingroup transaction
    */
   typedef std::vector<extension> extensions_type;

   /**
    *  Class transaction_header contains details about the transaction
    *
    *  @ingroup transaction
    *  @brief Contains details about the transaction
    */

   class transaction_header {
   public:

      /**
       * Construct a new transaction_header
       *
       * @brief Construct a new transaction_header object
       */
      transaction_header( time_point_sec exp = time_point_sec{} )
         :expiration(exp)
      {}

      time_point_sec  expiration;
      uint16_t        ref_block_num = 0UL;
      uint32_t        ref_block_prefix = 0UL;
      unsigned_int    max_net_usage_words = 0UL; /// number of 8 byte words this transaction can serialize into after compressions
      uint8_t         max_cpu_usage_ms = 0UL; /// number of CPU usage units to bill transaction for
      unsigned_int    delay_sec = 0UL; /// number of seconds to delay transaction, default: 0

      EOSLIB_SERIALIZE( transaction_header, (expiration)(ref_block_num)(ref_block_prefix)(max_net_usage_words)(max_cpu_usage_ms)(delay_sec) )
   };

   /**
    *  Class transaction contains the actions, context_free_actions and extensions type for a transaction
    *
    *  @ingroup transaction
    */
   class transaction : public transaction_header {
   public:

      /**
       * Construct a new transaction
       */
      transaction(time_point_sec exp = time_point_sec{}) : transaction_header( exp ) {}

      std::vector<action>  context_free_actions;
      std::vector<action>  actions;
      extensions_type      transaction_extensions;

      EOSLIB_SERIALIZE_DERIVED( transaction, transaction_header, (context_free_actions)(actions)(transaction_extensions) )
   };

   /**
    *  Retrieve the indicated action from the active transaction.
    *
    *  @ingroup transaction
    *  @param type - 0 for context free action, 1 for action
    *  @param index - the index of the requested action
    *  @return the indicated action
    */
   inline action get_action( uint32_t type, uint32_t index ) {
      constexpr size_t max_stack_buffer_size = 512;
      int s = internal_use_do_not_use::get_action( type, index, nullptr, 0 );
      core_net::check( s > 0, "get_action size failed" );
      size_t size = static_cast<size_t>(s);
      char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
      auto size2 = internal_use_do_not_use::get_action( type, index, buffer, size );
      core_net::check( size == static_cast<size_t>(size2), "get_action failed" );
      return core_net::unpack<core_net::action>( buffer, size );
   }

   /**
    *  Access a copy of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return the currently executing transaction
    */
   inline size_t read_transaction(char* ptr, size_t sz) {
      return internal_use_do_not_use::read_transaction( ptr, sz );
   }

   /**
    *  Gets the size of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return size of the currently executing transaction
    */
   inline size_t transaction_size() {
      return internal_use_do_not_use::transaction_size();
   }

   /**
    *  Gets the block number used for TAPOS on the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return block number used for TAPOS on the currently executing transaction
    *  Example:
    *  @code
    *  int tbn = tapos_block_num();
    *  @endcode
    */
   inline int tapos_block_num() {
      return internal_use_do_not_use::tapos_block_num();
   }

   /**
    *  Gets the block prefix used for TAPOS on the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return block prefix used for TAPOS on the currently executing transaction
    *  Example:
    *  @code
    *  int tbp = tapos_block_prefix();
    *  @endcode
    */
   inline int tapos_block_prefix() {
      return internal_use_do_not_use::tapos_block_prefix();
   }

   /**
    *  Gets the expiration of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @brief Gets the expiration of the currently executing transaction.
    *  @return expiration of the currently executing transaction in seconds since Unix epoch
    */
   inline uint32_t expiration() {
      return internal_use_do_not_use::expiration();
   }

   /**
    * Retrieve the signed_transaction.context_free_data[index].
    *
    *  @ingroup transaction
    *  @param index - the index of the context_free_data entry to retrieve
    *  @param buff - output buff of the context_free_data entry
    *  @param size - amount of context_free_data[index] to retrieve into buff, 0 to report required size
    *  @return size copied, or context_free_data[index].size() if 0 passed for size, or -1 if index not valid
    */
   inline int get_context_free_data( uint32_t index, char* buff, size_t size ) {
      return internal_use_do_not_use::get_context_free_data(index, buff, size);
   }
}
