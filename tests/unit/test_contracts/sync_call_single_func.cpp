#include <core_net/call.hpp>
#include <core_net/print.hpp>
#include <core_net/core_net.hpp>

class [[clang::annotate("core_net::contract")]] sync_call_single_func : public core_net::contract{
public:
   using contract::contract;

   [[clang::annotate("core_net::call")]]
   uint32_t return_ten() {
      return 10;
   }

   // Do NOT add any more functions tagged as `core_net::call` in this contract.
   // It is used to test a contract having only one function tagged as `core_net::call`.
};
