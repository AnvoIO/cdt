#include <core_net/eosio.hpp>
#include <vector>

using namespace eosio;

struct test_res {
   int a;
   float b;
   name  c;
};

class [[core_net::contract]] action_results_test : public contract {
   public:
   using contract::contract;

   [[core_net::action]]
   void action1() {}

   [[core_net::action]]
   uint32_t action2() { return 42; }

   [[core_net::action]]
   std::string action3() { return "foo"; }

   [[core_net::action]]
   std::vector<name> action4() { return {"dan"_n}; }

   [[core_net::action]]
   test_res action5() { return {4, 42.4f, "bucky"_n}; }
};
