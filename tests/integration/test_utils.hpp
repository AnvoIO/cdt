#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <string_view>
#include <vector>
#include <optional>
#include <charconv>

inline core_net::chain::version_t extract_version_from_json_abi(std::vector<char> abi) {
   std::string_view sv(abi.data(), abi.size());

   constexpr const char* pattern = R"("version": "core_net::abi/)";
   if (auto pos = sv.find(pattern); pos != std::string_view::npos) {
      return core_net::chain::version_t{sv.substr(pos + strlen(pattern))};
   }
   return {};
}
