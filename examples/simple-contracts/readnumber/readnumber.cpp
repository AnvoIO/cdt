#include <core_net/core_net.hpp>
using namespace core_net;

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[core_net::action]] uint64_t readnumber(){
         return 42;
      }
};
CORE_NET_DISPATCH(enf, (readnumber))
