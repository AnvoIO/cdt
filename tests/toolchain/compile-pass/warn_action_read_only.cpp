#include <core_net/core_net.hpp>
#include <core_net/privileged.hpp>

using namespace core_net;
extern "C" __attribute__((weak)) void set_resource_limit(int64_t, int64_t, int64_t);

class [[clang::annotate("core_net::contract")]] warn_action_read_only : public contract {
  public:
      using contract::contract;
      
      [[clang::annotate("core_net::action")]] [[clang::annotate("core_net::read_only")]]
      void test1( name user ) {
	      set_resource_limit(user.value, 0, 0);
      }
};
