#include <core_net/core_net.hpp>
using namespace core_net;

class [[clang::annotate("core_net::contract")]] send_inline : public contract {
   public:
      using contract::contract;

      [[clang::annotate("core_net::action")]]
      void test( name user, name inline_code );

      using test_action = action_wrapper<"test"_n, &send_inline::test>;
};
