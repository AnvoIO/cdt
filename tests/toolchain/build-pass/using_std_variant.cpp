/*
 * Regression test for https://github.com/EOSIO/eosio.cdt/issues/558
 *
 * Verifies that a class/function can be used from the std namespace
 */

#include <core_net/eosio.hpp>
#include <core_net/print.hpp>
#include <variant>

using std::variant;
using namespace eosio;

class[[core_net::contract("hello")]] hello : public contract
{
public:
   using contract::contract;

   [[core_net::action]] void hi(name user) {
      require_auth(user);
      print("Hello, ", user);
   }

   struct [[core_net::table]] greeting {
      uint64_t id;
      variant<int32_t, int64_t> t;
      uint64_t primary_key() const { return id; }
   };
   typedef multi_index<"greeting"_n, greeting> greeting_index;
};
