#include <core_net/core_net.hpp>
using namespace core_net;

class [[clang::annotate("core_net::contract")]] multi_index_example : public contract {
   public:
      using contract::contract;
      multi_index_example( name receiver, name code, datastream<const char*> ds )
         : contract(receiver, code, ds), testtab(receiver, receiver.value) {}

      struct [[clang::annotate("core_net::table")]] test_table {
         name test_primary;
         name secondary;
         uint64_t datum;
         uint64_t primary_key()const { return test_primary.value; }
         uint64_t by_secondary()const { return secondary.value; }
      };

      typedef core_net::multi_index<"testtaba"_n, test_table,
         core_net::indexed_by<"secid"_n,
         core_net::const_mem_fun<test_table, uint64_t, &test_table::by_secondary>>>
         test_tables;

      test_tables testtab;

      [[clang::annotate("core_net::action")]] 
      void set(name user);
      [[clang::annotate("core_net::action")]] 
      void print( name user );
      [[clang::annotate("core_net::action")]] 
      void bysec( name secid );
      [[clang::annotate("core_net::action")]] 
      void mod( name user, uint32_t n );
      [[clang::annotate("core_net::action")]] 
      void del( name user );

      using set_action = action_wrapper<"set"_n, &multi_index_example::set>;
      using print_action = action_wrapper<"print"_n, &multi_index_example::print>;
      using bysec_action = action_wrapper<"bysec"_n, &multi_index_example::bysec>;
      using mod_action = action_wrapper<"mod"_n, &multi_index_example::mod>;
      using del_action = action_wrapper<"del"_n, &multi_index_example::del>;
};
