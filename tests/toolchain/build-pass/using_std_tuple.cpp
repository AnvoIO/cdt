/*
 * Regression test for upstream issue 558
 *
 * Verifies that a class/function can be used from the std namespace
 */

#include <core_net/core_net.hpp>
#include <core_net/print.hpp>
#include <tuple>

using std::tuple;
using namespace core_net;

class[[clang::annotate("core_net::contract", "hello")]] hello : public contract
{
public:
   using contract::contract;

   [[clang::annotate("core_net::action")]] void hi(name user) {
      require_auth(user);
      print("Hello, ", user);
   }

   struct [[clang::annotate("core_net::table")]] greeting {
      uint64_t id;
      tuple<int, int> t;
      uint64_t primary_key() const { return id; }
   };
   typedef multi_index<"greeting"_n, greeting> greeting_index;
};
