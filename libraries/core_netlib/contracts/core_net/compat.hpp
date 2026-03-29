#pragma once
#include <core_net/core_net.hpp>

// Convenience macros wrapping CDT contract annotations.
#define CORE_NET_CONTRACT [[clang::annotate("core_net::contract")]]
#define CORE_NET_ACTION   [[clang::annotate("core_net::action")]]
#define CORE_NET_TABLE    [[clang::annotate("core_net::table")]]
#define CORE_NET_CALL     [[clang::annotate("core_net::call")]]
