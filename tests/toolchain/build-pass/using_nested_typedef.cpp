/*
 * Regression test for https://github.com/EOSIO/eosio.cdt/issues/600
 *
 * Verifies that nested typedefs build.
 */

#include <core_net/eosio.hpp>
using namespace eosio;

namespace foo {
//using str = std::string;
typedef std::string str;
}

class [[core_net::contract]] using_nested_typedef : public contract {
public:
   using contract::contract;
   [[core_net::action]]
   void hi(foo::str s) {
      print(s);
   }
};
