#include <core_net/core_net.hpp>

using namespace core_net;

class [[core_net::contract]] action_results_test : public contract {
   public:
   using contract::contract;

   [[core_net::action]]
   void action1() {}

   [[core_net::action]]
   uint32_t action2() { return 42; }

   [[core_net::action]]
   std::string action3() { return "foo"; }
};
