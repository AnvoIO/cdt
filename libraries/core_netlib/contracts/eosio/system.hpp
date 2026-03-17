#pragma once
#include <core_net/system.hpp>
namespace eosio {
   using namespace core_net;
   // Legacy alias: old code may call eosio::eosio_exit()
   inline void eosio_exit( int32_t code ) { core_net_exit(code); }
}
