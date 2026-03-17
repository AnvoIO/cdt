#include <core_net/eosio.hpp>

using namespace eosio;

class [[core_net::contract]] ricardian_contract_test : public contract {
  public:
      using contract::contract;
      
      [[core_net::action]]
      void test() {
      }
};