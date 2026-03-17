#pragma once
#include "action.hpp"
#include "../../core/core_net/print.hpp"
#include "multi_index.hpp"
#include "dispatcher.hpp"
#include "contract.hpp"

#ifndef CORE_NET_NATIVE
static_assert( sizeof(long) == sizeof(int), "unexpected size difference" );
#endif
