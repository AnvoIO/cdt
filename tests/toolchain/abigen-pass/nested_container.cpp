#include <core_net/eosio.hpp>
#include <tuple>
#include <vector>

using namespace eosio;
using std::map;
using std::string;
using std::tuple;
using std::vector;

class [[core_net::contract]] nested_container : public contract {
public:
   using contract::contract;

    [[core_net::action]] 
    void map2map(map<string, string> m, map<string, map<string, string>> m2m) {}

    [[core_net::action]]
    void settuple2(name user, const tuple  <int, double, string, vector<int> >& tp2) {}
};
