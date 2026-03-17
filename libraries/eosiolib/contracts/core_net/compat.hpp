#pragma once
#include <core_net/eosio.hpp>

// Convenience macros wrapping the [[core_net::*]] attributes.
#define CORE_NET_CONTRACT [[core_net::contract]]
#define CORE_NET_ACTION   [[core_net::action]]
#define CORE_NET_TABLE    [[core_net::table]]
#define CORE_NET_CALL     [[core_net::call]]

namespace core_net { using namespace core_net; }
