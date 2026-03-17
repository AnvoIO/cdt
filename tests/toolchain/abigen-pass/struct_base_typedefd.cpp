/*
 * Regression test for https://github.com/EOSIO/eosio.cdt/issues/601.
 *
 * Verifies that a struct can inherit from a typedef'd class/struct.
 */

#include <core_net/eosio.hpp>

using namespace eosio;

struct foo {
   int value;
};

using bar = foo;

struct baz : bar {
};

class [[core_net::contract]] struct_base_typedefd : public contract {
public:
   using contract::contract;

   [[core_net::action]]
   void hi(baz b) {
      print(b.value);
   }
};
