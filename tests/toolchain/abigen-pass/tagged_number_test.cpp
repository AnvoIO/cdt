#include <core_net/core_net.hpp>

using namespace core_net;

template<uint64_t Tag>
struct TaggedNumber {
    uint64_t value;
};

class [[clang::annotate("core_net::contract")]] tagged_number_test : public contract {
  public:
      using contract::contract;
      
      [[clang::annotate("core_net::action")]]
      void test(TaggedNumber<"a.tag"_n.value>) {
      }
};
