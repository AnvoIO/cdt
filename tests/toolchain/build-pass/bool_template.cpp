#include <core_net/core_net.hpp>

using namespace core_net;

namespace test {
   using _Bool = int32_t;
}

using My_Bool = float;

class [[core_net::contract]] bool_template : public core_net::contract {
   public:
      using contract::contract;

      [[core_net::action]] void test1(std::optional<bool> a) {}
      [[core_net::action]] void test2(std::variant<uint64_t, bool> a) {}
      [[core_net::action]] void test3(bool a) {}

      [[core_net::action]] void test4(test::_Bool a) {}
      [[core_net::action]] void test5(My_Bool a) {}
};
