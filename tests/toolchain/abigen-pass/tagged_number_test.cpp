#include <core_net/eosio.hpp>

using namespace eosio;

template<uint64_t Tag>
struct TaggedNumber {
    uint64_t value;
};

class [[core_net::contract]] tagged_number_test : public contract {
  public:
      using contract::contract;
      
      [[core_net::action]]
      void test(TaggedNumber<"a.tag"_n.value>) {
      }
};
