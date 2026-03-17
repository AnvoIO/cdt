#include <core_net/core_net.hpp>
using namespace core_net;

CONTRACT hello : public contract {
   public:
      using contract::contract;

      ACTION hi( name nm ) {
         print_f("Name : %\n", nm);
      }
};
