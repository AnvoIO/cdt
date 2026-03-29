#include <core_net/core_net.hpp>
#include <tuple>
#include <vector>

using namespace core_net;
using std::map;
using std::string;
using std::tuple;
using std::vector;

class [[clang::annotate("core_net::contract")]] nested_container : public contract {
public:
   using contract::contract;

    [[clang::annotate("core_net::action")]] 
    void map2map(map<string, string> m, map<string, map<string, string>> m2m) {}

    [[clang::annotate("core_net::action")]]
    void settuple2(name user, const tuple  <int, double, string, vector<int> >& tp2) {}
};
