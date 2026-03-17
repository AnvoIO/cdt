#pragma once

#include "name.hpp"

namespace core_net {
   namespace internal_use_do_not_use {
      extern "C" uint64_t core_net_contract_name;
   }

   inline name current_context_contract() { return name{internal_use_do_not_use::core_net_contract_name}; }
}
