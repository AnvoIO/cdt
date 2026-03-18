/* WASM exception stubs — no exception runtime available.
 * new_handler is provided by new.cpp via new_handler_fallback.ipp. */

#include <exception>
#include <cstdlib>

#include "include/atomic_support.h"

static std::terminate_handler __terminate_handler = nullptr;

namespace std {

terminate_handler set_terminate(terminate_handler func) noexcept {
    return __libcpp_atomic_exchange(&__terminate_handler, func);
}

terminate_handler get_terminate() noexcept {
    return __libcpp_atomic_load(&__terminate_handler);
}

void terminate() noexcept {
    terminate_handler h = get_terminate();
    if (h) (*h)();
    abort();
}

bool uncaught_exception() noexcept { return false; }
int uncaught_exceptions() noexcept { return 0; }

exception::~exception() noexcept {}
const char* exception::what() const noexcept { return "std::exception"; }

bad_exception::~bad_exception() noexcept {}
const char* bad_exception::what() const noexcept { return "std::bad_exception"; }

} // namespace std
