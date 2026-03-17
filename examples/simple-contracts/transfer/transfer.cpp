#include <core_net/core_net.hpp>

using namespace core_net; 

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[core_net::action]] void fixtransfer() {
            action(
                permission_level{get_self(), name("active")},
                name("core.token"),
                name("transfer"),
                std::make_tuple(get_self(), name("webaccount1"), "1.000 EOS", "llvm16 fixed trans")
            ).send();
      }
};

// CORE_NET_DISPATCH(enf, (fixtransfer))

