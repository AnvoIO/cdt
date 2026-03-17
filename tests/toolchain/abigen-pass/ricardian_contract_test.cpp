#include <core_net/core_net.hpp>

using namespace core_net;

class [[core_net::contract]] ricardian_contract_test : public contract {
  public:
      using contract::contract;
      
      [[core_net::action]]
      void test() {
      }
};