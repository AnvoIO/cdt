/*
 * Regression test for upstream issue 602.
 *
 * Verifies that an aliased type can be used as a variant template arg.
 */

#include <core_net/core_net.hpp>

using namespace core_net;

using str = std::string;

class [[core_net::contract]] aliased_type_variant_template_arg : public contract {
public:
   using contract::contract;

   [[core_net::action]]
   void hi(std::variant<uint64_t,str> v) {
      if (std::holds_alternative<uint64_t>(v)) {
      } else if (std::holds_alternative<str>(v)) {
      }
   }
};
