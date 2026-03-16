#pragma once
#include <eosio/eosio.hpp>

// Convenience macros wrapping the LLVM-hardcoded [[eosio::*]] attributes.
// Developers can write CORE_NET_CONTRACT instead of [[eosio::contract]], etc.
#define CORE_NET_CONTRACT [[eosio::contract]]
#define CORE_NET_ACTION   [[eosio::action]]
#define CORE_NET_TABLE    [[eosio::table]]
#define CORE_NET_DISPATCH EOSIO_DISPATCH

namespace core_net { using namespace eosio; }
