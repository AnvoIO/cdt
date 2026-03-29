#include <core_net/core_net.hpp>
#include <core_net/call.hpp>

// Because this contract does not tag any functions by `call` attribute,
// `sync_call` entry point is not generated.
// Any sync calls to this contract will return a status indicating
// sync calls are not supported by the receiver.
class [[clang::annotate("core_net::contract")]] sync_call_not_supported : public core_net::contract{
public:
   using contract::contract;

   [[clang::annotate("core_net::action")]]
   void voidfunc();

   [[clang::annotate("core_net::action")]]
   int intfunc();

   using void_func = core_net::call_wrapper<"voidfunc"_i, &sync_call_not_supported::voidfunc>;  // default behavior: abort when called
   using void_no_op_func = core_net::call_wrapper<"voidfunc"_i, &sync_call_not_supported::voidfunc, core_net::access_mode::read_write, core_net::support_mode::no_op>; // no op when called
   
   using int_func = core_net::call_wrapper<"intfunc"_i, &sync_call_not_supported::intfunc>;  // default behavior: abort when called
   using int_no_op_func = core_net::call_wrapper<"intfunc"_i, &sync_call_not_supported::intfunc, core_net::access_mode::read_write, core_net::support_mode::no_op>; // no op when called
};
