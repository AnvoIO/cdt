#include <core_net/call.hpp>
#include <core_net/core_net.hpp>

struct person_info {
   std::string first_name;
   std::string street;
};

class [[core_net::contract]] sync_call_addr_book_callee : public core_net::contract {
public:
   sync_call_addr_book_callee(core_net::name receiver, core_net::name code, core_net::datastream<const char*> ds): contract(receiver, code, ds) {}

   [[core_net::call]]
   void upsert(core_net::name user, std::string first_name, std::string street);

   [[core_net::call]]
   person_info get(core_net::name user);

   using upsert_read_only_func = core_net::call_wrapper<"upsert"_i, &sync_call_addr_book_callee::upsert, core_net::access_mode::read_only>;
   using upsert_func = core_net::call_wrapper<"upsert"_i, &sync_call_addr_book_callee::upsert>;
   using get_func = core_net::call_wrapper<"get"_i, &sync_call_addr_book_callee::get>;

private:
   struct [[core_net::table]] person {
      core_net::name key;
      std::string first_name;
      std::string street;

      uint64_t primary_key() const { return key.value; }
   };

   using address_index = core_net::multi_index<"people"_n, person>;
};
