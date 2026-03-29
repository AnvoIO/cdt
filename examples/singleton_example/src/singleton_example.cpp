#include <singleton_example.hpp>

[[clang::annotate("core_net::action")]]
void singleton_example::set( name user, uint64_t value ) {
   auto entry_stored = singleton_instance.get_or_create(user, testtablerow);
   entry_stored.primary_value = user;
   entry_stored.secondary_value = value;
   singleton_instance.set(entry_stored, user);
}

[[clang::annotate("core_net::action")]]
void singleton_example::get( ) {
   if (singleton_instance.exists())
      core_net::print(
         "Value stored for: ",
         name{singleton_instance.get().primary_value.value},
         " is ",
         singleton_instance.get().secondary_value,
         "\n");
   else
      core_net::print("Singleton is empty.\n");
}
