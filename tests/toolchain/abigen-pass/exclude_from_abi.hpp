#pragma once

#include <core_net/core_net.hpp>
#include <core_net/name.hpp>
#include <core_net/singleton.hpp>
#include <core_net/multi_index.hpp>



struct [[clang::annotate("core_net::table")]] out_of_class {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef core_net::multi_index<"mi.config55"_n, out_of_class> out_of_class_index;
using uout_of_class_index = core_net::multi_index<"mi.config551"_n, out_of_class>;

typedef core_net::singleton<"smpl.conf55"_n, core_net::name> smpl_config55;
typedef core_net::singleton<"config55"_n, out_of_class> config55;
typedef smpl_config55 smpl_config551;
typedef config55 config551;
using  smpl_conf551 = core_net::singleton<"smpl.conf551"_n, core_net::name>;
using  config552 = core_net::singleton<"config552"_n, out_of_class>;
using smpl_conf552 = smpl_conf551;
using config553 = config551;

class [[clang::annotate("core_net::contract", "singleton_contract_simple2")]] singleton_contract_simple2 : public core_net::contract {
    public:
        using core_net::contract::contract;
        
    [[clang::annotate("core_net::action")]]
        void whatever() {};
    
    struct [[clang::annotate("core_net::table")]] inside_class {
        uint64_t id;
        uint64_t primary_key() const { return id; }
    };
    typedef core_net::singleton<"smpl.conf552"_n, core_net::name> smpl_conf552;
    typedef core_net::singleton<"config552"_n, inside_class> config552;
    typedef smpl_conf552 smpl_conf553;
    typedef config552 config553;
    using smpl_conf554 = core_net::singleton<"smpl.conf554"_n, core_net::name>;
    using config554 = core_net::singleton<"config554"_n, inside_class>;
    using smpl_conf555 = smpl_conf554;
    using config555 = config554;



    typedef core_net::multi_index<"mi.config553"_n, inside_class> inside_class_index;
    using uinside_class_index = core_net::multi_index<"mi.config554"_n, inside_class>;
};
