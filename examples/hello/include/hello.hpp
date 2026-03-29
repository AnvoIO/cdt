#include <core_net/core_net.hpp>
using namespace core_net;

class [[clang::annotate("core_net::contract")]] hello : public contract {
   public:
      using contract::contract;

      [[clang::annotate("core_net::action")]] 
      void hi( name nm );
      [[clang::annotate("core_net::action")]] 
      void check( name nm );
      [[clang::annotate("core_net::action")]]
      std::pair<int, std::string> checkwithrv( name nm );

      using hi_action = action_wrapper<"hi"_n, &hello::hi>;
      using check_action = action_wrapper<"check"_n, &hello::check>;
      using checkwithrv_action = action_wrapper<"checkwithrv"_n, &hello::checkwithrv>;
};
