#include <core_net/core_net.hpp>

class [[core_net::contract]] separate_cpp_hpp : public core_net::contract {
public:
   using contract::contract;

   [[core_net::action]] void act();
};
