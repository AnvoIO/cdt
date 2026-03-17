#include <core_net/eosio.hpp>
#include <core_net/privileged.hpp>

using namespace eosio;
extern "C" __attribute__((weak)) __attribute__((eosio_wasm_import)) void set_resource_limit(int64_t, int64_t, int64_t);

class [[core_net::contract]] warn_action_read_only : public contract {
  public:
      using contract::contract;
      
      [[core_net::action, core_net::read_only]]
      void test1( name user ) {
	      set_resource_limit(user.value, 0, 0);
      }
};
