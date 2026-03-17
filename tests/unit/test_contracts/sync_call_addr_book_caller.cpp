#include "sync_call_addr_book_callee.hpp"

#include <core_net/eosio.hpp>
#include <core_net/call.hpp>

class [[core_net::contract]] sync_call_addr_book_caller : public core_net::contract{
public:
   using contract::contract;

   // Insert an entry using read_only, which will fail
   [[core_net::action]]
   void upsertrdonly(core_net::name user, std::string first_name, std::string street) {
      sync_call_addr_book_callee::upsert_read_only_func{"callee"_n}(user, first_name, street);
   }

   // Insert an entry
   [[core_net::action]]
   void upsert(core_net::name user, std::string first_name, std::string street) {
      sync_call_addr_book_callee::upsert_func{"callee"_n}(user, first_name, street);
   }

   // Read an entry
   [[core_net::action]]
   person_info get(core_net::name user) {
      auto user_info = sync_call_addr_book_callee::get_func{"callee"_n}(user);
      core_net::check(user_info.first_name == "alice", "first name not alice");
      core_net::check(user_info.street == "123 Main St.", "street not 123 Main St.");
      return user_info;
   }
};
