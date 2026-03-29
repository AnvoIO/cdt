#include <core_net/call.hpp>
#include <core_net/core_net.hpp>

class [[clang::annotate("core_net::contract")]] sync_call_test : public core_net::contract {
public:
   using contract::contract;

   [[clang::annotate("core_net::call")]]
   uint32_t noparam() {
      return 10;
   }
   
   [[clang::annotate("core_net::call")]]
   uint32_t withparam(uint32_t in) {
      return in;
   }
   
   [[clang::annotate("core_net::call")]]
   void voidfunc() {
      int i = 10;
   }
   
   [[clang::annotate("core_net::call")]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c) {
      return a + b + c;
   }
};
