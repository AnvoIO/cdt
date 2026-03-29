#include <core_net/core_net.hpp>

using namespace core_net;

class [[clang::annotate("core_net::contract")]] ricardian_contract_test : public contract {
  public:
      using contract::contract;
      
      [[clang::annotate("core_net::action")]]
      void test() {
      }
};