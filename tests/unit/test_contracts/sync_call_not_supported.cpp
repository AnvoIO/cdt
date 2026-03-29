#include "sync_call_not_supported.hpp"

[[clang::annotate("core_net::action")]]
void sync_call_not_supported::voidfunc() {
}

[[clang::annotate("core_net::action")]]
int sync_call_not_supported::intfunc() {
   return 1;
}
