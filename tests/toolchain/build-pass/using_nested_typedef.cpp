/*
 * Regression test for upstream issue 600
 *
 * Verifies that nested typedefs build.
 */

#include <core_net/core_net.hpp>
using namespace core_net;

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
