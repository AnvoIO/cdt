#include <core_net/core_net.hpp>

using namespace core_net;

class [[clang::annotate("core_net::contract")]] action_results_test : public contract {
   public:
   using contract::contract;

   [[clang::annotate("core_net::action")]]
   void action1() {}

   [[clang::annotate("core_net::action")]]
   uint32_t action2() { return 42; }

   [[clang::annotate("core_net::action")]]
   std::string action3() { return "foo"; }
};
