/*
 * Regression test for upstream issue 601.
 *
 * Verifies that a struct can inherit from a typedef'd class/struct.
 */

#include <core_net/core_net.hpp>

using namespace core_net;

struct foo {
   int value;
};

using bar = foo;

struct baz : bar {
};

class [[clang::annotate("core_net::contract")]] struct_base_typedefd : public contract {
public:
   using contract::contract;

   [[clang::annotate("core_net::action")]]
   void hi(baz b) {
      print(b.value);
   }
};
