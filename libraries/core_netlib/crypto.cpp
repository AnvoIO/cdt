/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "core/core_net/crypto.hpp"
#include "core/core_net/datastream.hpp"

#include <cstring>

extern "C" {
   struct __attribute__((aligned (16))) capi_checksum160 { uint8_t hash[20]; };
   struct __attribute__((aligned (16))) capi_checksum256 { uint8_t hash[32]; };
   struct __attribute__((aligned (16))) capi_checksum512 { uint8_t hash[64]; };
      void assert_sha256( const char* data, uint32_t length, const capi_checksum256* hash );

      void assert_sha1( const char* data, uint32_t length, const capi_checksum160* hash );

      void assert_sha512( const char* data, uint32_t length, const capi_checksum512* hash );

      void assert_ripemd160( const char* data, uint32_t length, const capi_checksum160* hash );

      void sha256( const char* data, uint32_t length, capi_checksum256* hash );

      void sha1( const char* data, uint32_t length, capi_checksum160* hash );

      void sha512( const char* data, uint32_t length, capi_checksum512* hash );

      void ripemd160( const char* data, uint32_t length, capi_checksum160* hash );

      int recover_key( const capi_checksum256* digest, const char* sig,
                    size_t siglen, char* pub, size_t publen );

      void assert_recover_key( const capi_checksum256* digest, const char* sig,
                            size_t siglen, const char* pub, size_t publen );
}

namespace core_net {

   void assert_sha256( const char* data, uint32_t length, const core_net::checksum256& hash ) {
      auto hash_data = hash.extract_as_byte_array();
      ::assert_sha256( data, length, reinterpret_cast<const ::capi_checksum256*>(hash_data.data()) );
   }

   void assert_sha1( const char* data, uint32_t length, const core_net::checksum160& hash ) {
      auto hash_data = hash.extract_as_byte_array();
      ::assert_sha1( data, length, reinterpret_cast<const ::capi_checksum160*>(hash_data.data()) );
   }

   void assert_sha512( const char* data, uint32_t length, const core_net::checksum512& hash ) {
      auto hash_data = hash.extract_as_byte_array();
      ::assert_sha512( data, length, reinterpret_cast<const ::capi_checksum512*>(hash_data.data()) );
   }

   void assert_ripemd160( const char* data, uint32_t length, const core_net::checksum160& hash ) {
      auto hash_data = hash.extract_as_byte_array();
      ::assert_ripemd160( data, length, reinterpret_cast<const ::capi_checksum160*>(hash_data.data()) );
   }

   core_net::checksum256 sha256( const char* data, uint32_t length ) {
      ::capi_checksum256 hash;
      ::sha256( data, length, &hash );
      return {hash.hash};
   }

   core_net::checksum160 sha1( const char* data, uint32_t length ) {
      ::capi_checksum160 hash;
      ::sha1( data, length, &hash );
      return {hash.hash};
   }

   core_net::checksum512 sha512( const char* data, uint32_t length ) {
      ::capi_checksum512 hash;
      ::sha512( data, length, &hash );
      return {hash.hash};
   }

   core_net::checksum160 ripemd160( const char* data, uint32_t length ) {
      ::capi_checksum160 hash;
      ::ripemd160( data, length, &hash );
      return {hash.hash};
   }

   core_net::public_key recover_key( const core_net::checksum256& digest, const core_net::signature& sig ) {
      auto digest_data = digest.extract_as_byte_array();

      auto sig_data = core_net::pack(sig);

      char optimistic_pubkey_data[256];
      size_t pubkey_size = ::recover_key( reinterpret_cast<const capi_checksum256*>(digest_data.data()),
                                          sig_data.data(), sig_data.size(),
                                          optimistic_pubkey_data, sizeof(optimistic_pubkey_data) );

      core_net::public_key pubkey;
      if ( pubkey_size <= sizeof(optimistic_pubkey_data) ) {
         core_net::datastream<const char*> pubkey_ds( optimistic_pubkey_data, pubkey_size );
         pubkey_ds >> pubkey;
      } else {
         constexpr static size_t max_stack_buffer_size = 512;
         void* pubkey_data = (max_stack_buffer_size < pubkey_size) ? malloc(pubkey_size) : alloca(pubkey_size);

         ::recover_key( reinterpret_cast<const capi_checksum256*>(digest_data.data()),
                        sig_data.data(), sig_data.size(),
                        reinterpret_cast<char*>(pubkey_data), pubkey_size );
         core_net::datastream<const char*> pubkey_ds( reinterpret_cast<const char*>(pubkey_data), pubkey_size );
         pubkey_ds >> pubkey;

         if( max_stack_buffer_size < pubkey_size ) {
            free(pubkey_data);
         }
      }
      return pubkey;
   }

   void assert_recover_key( const core_net::checksum256& digest, const core_net::signature& sig, const core_net::public_key& pubkey ) {
      auto digest_data = digest.extract_as_byte_array();

      auto sig_data = core_net::pack(sig);
      auto pubkey_data = core_net::pack(pubkey);

      ::assert_recover_key( reinterpret_cast<const capi_checksum256*>(digest_data.data()),
                            sig_data.data(), sig_data.size(),
                            pubkey_data.data(), pubkey_data.size() );
   }
}
