#include "sync_call_callee.hpp"
#include <core_net/print.hpp>

[[core_net::call]]
uint32_t sync_call_callee::return_ten() {
   return 10;
}

[[core_net::call]]
uint32_t sync_call_callee::echo_input(uint32_t in) {
   return in;
}

[[core_net::call]]
void sync_call_callee::void_func() {
   core_net::print("I am a void function");
}

[[core_net::action, core_net::call]]
uint32_t sync_call_callee::sum(uint32_t a, uint32_t b, uint32_t c) {
   return a + b + c;
}

[[core_net::call]]
struct1_t sync_call_callee::pass_single_struct(struct1_t s) {
   return s;
}

[[core_net::call]]
struct1_t sync_call_callee::pass_multi_structs(struct1_t s1, int32_t m, struct2_t s2) {
   return { .a = s1.a * m + s2.c, .b = s1.b * m + s2.d };
}

bool sync_call_callee::issynccall() {
   return is_sync_call();
}
