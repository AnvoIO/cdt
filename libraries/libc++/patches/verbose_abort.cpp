/* WASM-compatible verbose_abort: trigger trap instead of fprintf to stderr */

#include <__config>
#include <__verbose_abort>
#include <cstdlib>

_LIBCPP_BEGIN_NAMESPACE_STD

_LIBCPP_WEAK
void __libcpp_verbose_abort(char const* format, ...) {
  std::abort();
}

_LIBCPP_END_NAMESPACE_STD
