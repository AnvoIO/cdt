#include <core_net/call.hpp>
#include <core_net/core_net.hpp>

class [[core_net::contract]] sync_call_test : public core_net::contract {
public:
   using contract::contract;

   [[core_net::call]]
   uint32_t noparam() {
      return 10;
   }
   
   [[core_net::call]]
   uint32_t withparam(uint32_t in) {
      return in;
   }
   
   [[core_net::call]]
   void voidfunc() {
      int i = 10;
   }
   
   [[core_net::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c) {
      return a + b + c;
   }
};
