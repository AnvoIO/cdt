#include <core_net/core_net.hpp>
#include <vector>

using namespace core_net;

struct test_res {
   int a;
   float b;
   name  c;
};

class [[clang::annotate("core_net::contract")]] action_results_test : public contract {
   public:
   using contract::contract;

   [[clang::annotate("core_net::action")]]
   void action1() {}

   [[clang::annotate("core_net::action")]]
   uint32_t action2() { return 42; }

   [[clang::annotate("core_net::action")]]
   std::string action3() { return "foo"; }

   [[clang::annotate("core_net::action")]]
   std::vector<name> action4() { return {"dan"_n}; }

   [[clang::annotate("core_net::action")]]
   test_res action5() { return {4, 42.4f, "bucky"_n}; }
};
