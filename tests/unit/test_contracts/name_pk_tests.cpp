// Verifies that a table with name-typed primary key works

#include <core_net/multi_index.hpp>
#include <core_net/contract.hpp>

struct [[clang::annotate("core_net::table")]] name_table {
    core_net::name pk;
    int num;

    auto primary_key() const { return pk; }
};
using name_table_idx = core_net::multi_index<"name.pk"_n, name_table>;

class [[clang::annotate("core_net::contract")]] name_pk_tests : public core_net::contract {
 public:
   using core_net::contract::contract;

   [[clang::annotate("core_net::action")]] void write() {
       name_table_idx table(get_self(), 0);
       table.emplace(get_self(), [](auto& row) {
           row.pk = "alice"_n;
           row.num = 2;
       });
       table.emplace(get_self(), [](auto& row) {
           row.pk = "bob"_n;
           row.num = 1;
       });
   }

   [[clang::annotate("core_net::action")]] void read() {
       name_table_idx table(get_self(), 0);
       core_net::check(table.get("alice"_n).num == 2, "num mismatch");
       core_net::check(table.get("bob"_n).num == 1, "num mismatch");
   }
};
