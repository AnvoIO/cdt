#include <core_net/eosio.hpp>
#include <core_net/tester.hpp>

using namespace eosio::native;

EOSIO_TEST_BEGIN(print_test)
   CHECK_PRINT("27", [](){ core_net::print((uint8_t)27); });
   CHECK_PRINT("34", [](){ core_net::print((int)34); });
   CHECK_PRINT([](std::string s){return s[0] == 'a';},  [](){ core_net::print((char)'a'); });
   CHECK_PRINT([](std::string s){return s[0] == 'b';},  [](){ core_net::print((int8_t)'b'); });
   CHECK_PRINT("202", [](){ core_net::print((unsigned int)202); });
   CHECK_PRINT("-202", [](){ core_net::print((int)-202); });
   CHECK_PRINT("707", [](){ core_net::print((unsigned long)707); });
   CHECK_PRINT("-707", [](){ core_net::print((long)-707); });
   CHECK_PRINT("909", [](){ core_net::print((unsigned long long)909); });
   CHECK_PRINT("-909", [](){ core_net::print((long long)-909); });
   CHECK_PRINT("404", [](){ core_net::print((uint32_t)404); });
   CHECK_PRINT("-404", [](){ core_net::print((int32_t)-404); });
   CHECK_PRINT("404000000", [](){ core_net::print((uint64_t)404000000); });
   CHECK_PRINT("-404000000", [](){ core_net::print((int64_t)-404000000); });
   CHECK_PRINT("0x0066000000000000", [](){ core_net::print((uint128_t)102); });
   CHECK_PRINT("0xffffff9affffffffffffffffffffffff", [](){ core_net::print((int128_t)-102); });
EOSIO_TEST_END

int main(int argc, char** argv) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(print_test);
   return has_failed();
}
