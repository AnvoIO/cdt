#include <core_net/core_net.hpp>
#include <core_net/action.hpp>
#include <core_net/name.hpp>

#include "get_code_hash_table.hpp"

class [[core_net::contract]] get_code_hash_tests : public contract {
public:
   using contract::contract;

   using hash_table = multi_index<name("code.hash"), code_hash>;

   // Read the old code's hash from database and verify new code's hash differs
   [[core_net::action]]
   void theaction() {
      require_auth(get_self());
      hash_table hashes(get_self(), get_self().value);

      auto hash = get_code_hash(get_self());
      check(hash != checksum256(), "Code hash should not be null");

      auto record = hashes.get(0, "Unable to find recorded hash");
      check(hash != record.hash, "Code hash has not changed");
      core_net::print("Old hash: ", record.hash, "; new hash: ", hash);
   }
};

