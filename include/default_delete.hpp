#ifndef FILE_DEFAULT_DELETE_HPP
#define FILE_DEFAULT_DELETE_HPP
#include <concepts>

namespace cx {
    template<class T>
    struct default_delete {
        constexpr default_delete() noexcept = default;

        template<class U>
        constexpr default_delete(const default_delete<U> &) noexcept requires std::convertible_to<U *, T*> {}

        constexpr void operator()(T *ptr) const noexcept { delete ptr; }
    };

    template<class T>
    struct default_delete<T[]> {
        constexpr default_delete() noexcept = default;

        template<class U>
        constexpr default_delete(const default_delete<U> &) noexcept requires std::convertible_to<U(*)[], T(*)[]> {}

        constexpr void operator()(T *ptr) const noexcept { delete[] ptr; }
    };
}

#endif // FILE_DEFAULT_DELETE_HPP