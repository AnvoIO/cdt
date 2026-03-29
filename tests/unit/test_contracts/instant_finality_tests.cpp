#include <core_net/core_net.hpp>
#include <core_net/instant_finality.hpp>

class [[clang::annotate("core_net::contract")]] instant_finality_tests : public core_net::contract{
public:
    using contract::contract;

    [[clang::annotate("core_net::action")]]
    void setfinalizer(const core_net::finalizer_policy& finalizer_policy) {
        core_net::set_finalizers(finalizer_policy);
    }
};
