/* Minimal chrono implementation for WASM smart contracts.
 * Only provides from_time_t(); no clock_gettime or platform clocks. */

#include "chrono"
#include <time.h>

_LIBCPP_BEGIN_NAMESPACE_STD

namespace chrono
{
   system_clock::time_point
   system_clock::from_time_t(time_t t) _NOEXCEPT
   {
      return system_clock::time_point(seconds(t));
   }
} // namespace chrono

_LIBCPP_END_NAMESPACE_STD
