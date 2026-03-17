#include <core_net/asset.hpp>
#include <core_net/eosio.hpp>
#include <core_net/name.hpp>
#include <core_net/singleton.hpp>

#include "exclude_from_abi.hpp"
 
using namespace eosio;

struct [[core_net::table]] out_of_class2 {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef core_net::multi_index<"mi.config5"_n, out_of_class2> out_of_class_index51;
using uout_of_class_index51 = core_net::multi_index<"mi.config51"_n, out_of_class2>;

struct [[core_net::table, core_net::contract("singleton_contract")]] out_of_class3 {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef core_net::multi_index<"mi.config52"_n, out_of_class3> out_of_class_index52;

typedef core_net::singleton<"smpl.conf5"_n, core_net::name> smpl_config5;
typedef core_net::singleton<"config5"_n, out_of_class2> config5;
typedef smpl_config5 smpl_config51;
typedef config5 config51;
using  smpl_conf51 = core_net::singleton<"smpl.conf51"_n, core_net::name>;
using  config52 = core_net::singleton<"config52"_n, out_of_class2>;
using smpl_conf52 = smpl_conf51;
using config53 = config51;

class [[core_net::contract("singleton_contract")]] singleton_contract : public contract {
    public:
        using contract::contract;
        
    [[core_net::action]]
        void whatever() {};
 
    struct [[core_net::table]] tbl_config {
        uint64_t y;
        uint64_t x;
    };
    
    typedef core_net::singleton<"config"_n, tbl_config>    config;
    typedef core_net::singleton<"smpl.config"_n, name>     smpl_config;
    using smpl_config2 = smpl_config5;
    typedef config551 config2; //from exclude_from_abi.hpp
};
