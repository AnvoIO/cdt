/* WASM new/delete operators and new_handler — avoids constinit (C++20) */

#include <new>
#include <cstdlib>
#include <__memory/aligned_alloc.h>

#include "include/atomic_support.h"

static std::new_handler __new_handler_val = nullptr;

namespace std {

new_handler set_new_handler(new_handler handler) noexcept {
    return __libcpp_atomic_exchange(&__new_handler_val, handler);
}

new_handler get_new_handler() noexcept {
    return __libcpp_atomic_load(&__new_handler_val);
}

#ifndef __GLIBCXX__
const nothrow_t nothrow{};
#endif

#ifndef LIBSTDCXX
void __throw_bad_alloc() {
    _VSTD::abort();
}
#endif

} // namespace std

#if !defined(__GLIBCXX__) && !defined(_LIBCPP_ABI_VCRUNTIME) && \
    !defined(_LIBCPP_DISABLE_NEW_DELETE_DEFINITIONS)

_LIBCPP_WEAK void* operator new(std::size_t size) _THROW_BAD_ALLOC {
    if (size == 0) size = 1;
    void* p;
    while ((p = ::malloc(size)) == nullptr) {
        std::new_handler nh = std::get_new_handler();
        if (nh) nh();
        else break;
    }
    return p;
}

_LIBCPP_WEAK void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return ::operator new(size);
}

_LIBCPP_WEAK void* operator new[](std::size_t size) _THROW_BAD_ALLOC {
    return ::operator new(size);
}

_LIBCPP_WEAK void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return ::operator new(size);
}

_LIBCPP_WEAK void operator delete(void* ptr) noexcept { ::free(ptr); }
_LIBCPP_WEAK void operator delete(void* ptr, const std::nothrow_t&) noexcept { ::operator delete(ptr); }
_LIBCPP_WEAK void operator delete(void* ptr, std::size_t) noexcept { ::operator delete(ptr); }
_LIBCPP_WEAK void operator delete[](void* ptr) noexcept { ::operator delete(ptr); }
_LIBCPP_WEAK void operator delete[](void* ptr, const std::nothrow_t&) noexcept { ::operator delete[](ptr); }
_LIBCPP_WEAK void operator delete[](void* ptr, std::size_t) noexcept { ::operator delete[](ptr); }

#if !defined(_LIBCPP_HAS_NO_LIBRARY_ALIGNED_ALLOCATION)
_LIBCPP_WEAK void* operator new(std::size_t size, std::align_val_t alignment) _THROW_BAD_ALLOC {
    if (size == 0) size = 1;
    void* p;
    while ((p = std::__libcpp_aligned_alloc(static_cast<std::size_t>(alignment), size)) == nullptr) {
        std::new_handler nh = std::get_new_handler();
        if (nh) nh();
        else break;
    }
    return p;
}

_LIBCPP_WEAK void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    return ::operator new(size, alignment);
}

_LIBCPP_WEAK void* operator new[](std::size_t size, std::align_val_t alignment) _THROW_BAD_ALLOC {
    return ::operator new(size, alignment);
}

_LIBCPP_WEAK void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    return ::operator new(size, alignment);
}

_LIBCPP_WEAK void operator delete(void* ptr, std::align_val_t) noexcept { std::__libcpp_aligned_free(ptr); }
_LIBCPP_WEAK void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept { ::operator delete(ptr, alignment); }
_LIBCPP_WEAK void operator delete(void* ptr, std::size_t, std::align_val_t alignment) noexcept { ::operator delete(ptr, alignment); }
_LIBCPP_WEAK void operator delete[](void* ptr, std::align_val_t alignment) noexcept { ::operator delete(ptr, alignment); }
_LIBCPP_WEAK void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept { ::operator delete[](ptr, alignment); }
_LIBCPP_WEAK void operator delete[](void* ptr, std::size_t, std::align_val_t alignment) noexcept { ::operator delete[](ptr, alignment); }
#endif // !_LIBCPP_HAS_NO_LIBRARY_ALIGNED_ALLOCATION

#endif
