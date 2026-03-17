// Verifies that the dispatching code is self-contained

class [[core_net::contract]] minimal_tests {
 public:
   template<typename N, typename DS>
   explicit constexpr minimal_tests(const N&, const N&, const DS&) {}
   [[core_net::action]] void test1() {}
};
