#include <core_net/core_net.hpp>
#include <core_net/print.hpp>
using namespace core_net;

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[clang::annotate("core_net::action")]] void declare() {
         print("Hello World!\n");    
      }

};

extern "C" {
   [[clang::annotate("core_net::wasm_entry")]]
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
   }
} 
