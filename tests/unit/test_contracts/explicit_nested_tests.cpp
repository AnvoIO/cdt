#include <core_net/core_net.hpp>

    
using namespace std;
using namespace core_net;
    
CONTRACT explicit_nested_tests : public contract {
   public:
   using contract::contract;

   explicit_nested_tests(name receiver, name code, datastream<const char *> ds):
      contract(receiver, code, ds),test_table(receiver, receiver.value){}
    
    ACTION nothing() {
     std::array<std::vector<std::optional<float>>, 4> arrv4;
     std::vector<std::array<std::optional<float>, 4>> varr4;
   }; 
   typedef std::optional<float> opt_float;
   typedef std::vector<opt_float> vec_opt_float; 
   typedef struct _mystruct{
      int field0;
      float field1;
      std::vector<std::vector<std::string>> field2;
   }mystruct;

   TABLE testdata {
      uint64_t  id;
      std::map<uint64_t, std::optional<int> > data; 
      std::vector<std::optional<std::pair<uint64_t,float>>> data_vec;
      std::vector<std::pair<uint64_t, opt_float >> data_vec2;
      vec_opt_float data_vec3;
      std::tuple<uint64_t, std::optional<float>, std::vector<int>> tup1;
      std::variant<uint64_t, std::optional<std::pair<int, float>>, std::vector<int>> var1;
      std::vector<std::vector<_mystruct>> vvmys;
      auto primary_key() const -> decltype(id) { return id; }

      CORE_NET_SERIALIZE(testdata, (id)(data)(data_vec)(data_vec2)(data_vec3)(tup1)(var1)(vvmys));
   };
   using test_data_idx = multi_index<"testdata"_n, testdata>;

   [[core_net::action]]
    //usage: cleos -v push action core vvstr '[[["abc", "cde"],["def","fgh"]]]' -p core@active
   std::vector<std::vector<std::string>> vvstr(std::vector<std::vector<std::string>> input) {
      std::vector<std::vector<std::string>> output = input;
      for(auto & row : output) {
         for(auto & word : row) {
            core_net::cout << " " << word;
         }
         core_net::cout << "\n";
      }
      return output;
   } 
   [[core_net::action]]
   // usage : cleos -v push action core vvvstr '[[[["abc", "cde"],["def","fgh"]]]]' -p core@active
   std::vector<std::vector<std::vector<std::string>>> vvvstr(std::vector<std::vector<std::vector<std::string>>> input) {
      std::vector<std::vector<std::vector<std::string>>> output = input;
      for(auto & vecvec : output){
         for(auto & row : vecvec) {
            for(auto & word : row) {
               core_net::cout << " " << word;
            }
            core_net::cout << "\n";
         }
      }
      return output;
   } 

   [[core_net::action]]
   //usage : cleos -v push action core mapo '[[{"key":1,"value":2.0},{"key":2,"value":3.9}]]'  -p core@active
   std::map<uint64_t, std::optional<float> >  mapo(std::map<uint64_t, std::optional<float>>  input){
      std::map<uint64_t, std::optional<float>> output = input;
      return output;
   }

   [[core_net::action]]
   // usage : cleos -v push action core vecop '[[{"first":1,"second":2.0},{"first":2,"second":3.9}]]'  -p core@active
   std::vector<std::optional<std::pair<uint64_t,float>>>  vecop(std::vector<std::optional<std::pair<uint64_t,float>>>  input){
      std::vector<std::optional<std::pair<uint64_t,float>>> output = input;
      return output;
   }

   [[core_net::action]]
    // usage : cleos -v push action core vecpo '[[{"first":1,"second":2.0},{"first":2,"second":3.9}]]'  -p core@active
   std::vector<std::pair<uint64_t, opt_float >>  vecpo(std::vector<std::pair<uint64_t, opt_float >>  input){
      std::vector<std::pair<uint64_t, opt_float >> output = input;
      return output;
   }

   [[core_net::action]]
   // usage : cleos -v push action core vecpoe '[[{"first":1,"second":2.0},{"first":2,"second":3.9}]]'  -p core@active
   std::vector<std::pair<uint64_t, std::optional<float> >>  vecpoe(std::vector<std::pair<uint64_t, std::optional<float> >>  input){
      std::vector<std::pair<uint64_t, std::optional<float> >> output = input;
      return output;
   }

   [[core_net::action]]
   // usage : cleos -v push action core tup '[{"field_0":1,"field_1":2.0,"field_2":[4,5,6,7]}]'  -p core@active
   std::tuple<uint64_t, std::optional<float>, std::vector<int>>  tup(std::tuple<uint64_t, std::optional<float>, std::vector<int>>  input){
      std::tuple<uint64_t, std::optional<float>, std::vector<int>> output = input;
      return output;
   }

   [[core_net::action]]
    // usage : cleos -v push action core var '[["uint64",8]]'  -p core@active
    // usage : cleos -v push action core var '[["B_optional_B_pair_int32_float32_E_E", {"first":1,"second":2.0}]]'  -p core@active
    // usage : cleos -v push action core var '[["B_vector_int32_E", [2,3,4]]]'  -p core@active
   std::variant<uint64_t, std::optional<std::pair<int, float>>, std::vector<int>>  var(std::variant<uint64_t, std::optional<std::pair<int, float>>, std::vector<int>>  input){
      std::variant<uint64_t, std::optional<std::pair<int, float>>, std::vector<int>> output = input;
      return output;
   }

   [[core_net::action]]  // sdl means set deque list , as we support them should test them as well.
   // usage : cleos -v push action core sdlstr '[[[["abc", "cde"],["def","fgh"]]]]' -p core@active
   std::set<std::deque<std::list<std::string>>> sdlstr(std::set<std::deque<std::list<std::string>>> input) {
      std::set<std::deque<std::list<std::string>>> output = input;
      for(auto & deqlist : output){
         for(auto & row : deqlist) {
            for(auto & word : row) {
               core_net::cout << " " << word;
            }
            core_net::cout << "\n";
         }
      }
      return output;
   }

   typedef std::vector<std::vector<std::string>>  vecvecstr;
   [[core_net::action]]   // this test is for the whole type is not explicit nested by the inside type is a alias of explicit nested.
   // usage : cleos -v push action core vivvstr '[[[["abc", "cde"],["def","fgh"]]]]' -p core@active
   std::vector<vecvecstr> vivvstr(std::vector<vecvecstr> input) {
      std::vector<vecvecstr> output = input;
      for(auto & vecvec : output){
         for(auto & row : vecvec) {
            for(auto & word : row) {
               core_net::cout << " " << word;
            }
            core_net::cout << "\n";
         }
      }
      return output;
   }

   typedef vector<vector<vector<int> > > myvec3;
   typedef set<myvec3> myset;

   [[core_net::action]]   // this test is for the whole type is not explicit nested by the inside type is a alias of explicit nested.
   // usage : cleos -v push action core sivvvi '[[[[[1, 2],[3,4]]]]]' -p core@active
   myset sivvvi(myset input) {
      myset output = input;
      for(auto & vvvi : output){
         for(auto & vecvec : vvvi){
            for(auto & row : vecvec) {
               for(auto & num : row) {
                  core_net::cout << " " << num;
               }
               core_net::cout << "\n";
            }
         }
      }
      return output;
   }

   [[core_net::action]]
   std::vector<std::vector<_mystruct>>  vvmystr(){
      std::vector<std::vector<_mystruct>> output = {{{56,5.6, {{"test"}} }, {78,7.8, {{"passed"}} }}};
      return output;
   }

   [[core_net::action]]   // this test is for the put simple data to table, so as to check the the data read from table is the same
   // usage : cleos -v push action core putd '[core]' -p core@active
   //             cleos -v get table core core testdata
   void putd(name user) {
      require_auth(user);
      test_data_idx _tests(get_self(), get_self().value);

      std::map<uint64_t, std::optional<int> > data;
      data[12] = 34;
      std::vector<std::optional<std::pair<uint64_t,float>>> data_vec = {{{12,1.2}}, {{34, 3.4}}};
      std::vector<std::pair<uint64_t, opt_float >> data_vec2 = {{56, {5.6}},{78,{7.8}}};
      vec_opt_float data_vec3 = {{1.1}, {2.2}, {3.3}};
      std::tuple<uint64_t, std::optional<float>, std::vector<int>> tup1 = {12, {3.4}, {5,6,7}};
      std::vector<int> tempvec = {8,9,10};
      std::variant<uint64_t, std::optional<std::pair<int, float>>, std::vector<int>> var1 = tempvec;
      std::vector<std::vector<_mystruct>> vvmys = {{{12,1.2, {{"test"}}}, {34,3.4,{{"passed"}} }}};
      _tests.emplace(user, [&](auto& t) {
         t.id = user.value + std::time(0);  // primary key can't be same
         t.data = data;
         t.data_vec = data_vec;
         t.data_vec2 = data_vec2;
         t.data_vec3 = data_vec3;
         t.tup1 = tup1;
         t.var1 = var1;
         t.vvmys = vvmys;
      });
      auto it = _tests.begin();
      auto ite = _tests.end();
      while(it != ite){
         core_net::cout << "id = " << it->id << "\n";
         core_net::cout << "data : " << "\n";
         for(auto & p : it->data){
            core_net::cout << p.first << ":" << p.second.value() << "\n";
         }
         core_net::cout << "data_vec : \n";
         for(auto & opt : it->data_vec){
            if(opt.has_value()) core_net::cout << opt.value().first << ":" << opt.value().second << "\n";
            else core_net::cout << "no value in optional \n";
         }
         core_net::cout << "data_vec2 : \n";
         for(auto & p : it->data_vec2){
            core_net::cout << p.first << ":" << p.second.value() << "\n";
         }
         core_net::cout << "data_vec3 : \n";
         for(auto & opt : it->data_vec3){
            if(opt.has_value()) core_net::cout << opt.value() << "\n";
            else core_net::cout << "no value in optional \n";
         }
         core_net::cout << "tup1 : \n";
         auto [a, b, c] = it->tup1;
         core_net::cout << a << ",   " << b.value() << ",   " ;
         for(auto & v : c){ core_net::cout << v  << " ";}
         core_net::cout << "\n";

         core_net::cout << "var1 : \n";
         for(auto & v : std::get<2>(it->var1)) { core_net::cout << v  << " "; };
         core_net::cout << "\n";

         core_net::cout << "vvmys: \n";
         for(auto & vmys : it->vvmys){
            for(auto & mys : vmys){
               core_net::cout << mys.field0 << "," << mys.field1 << mys.field2[0][0] <<  "\n";
            }
         }
         core_net::cout << "\n\n";
         ++it;
      }
   }

   test_data_idx test_table;
};
