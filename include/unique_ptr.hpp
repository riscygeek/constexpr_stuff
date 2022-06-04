#ifndef FILE_UNIQUE_PTR_HPP
#define FILE_UNIQUE_PTR_HPP
#include <cstddef>
#include <utility>

namespace cx {
    template<class T>
    class unique_ptr {
    public:
        using pointer = T *;
        using element_type = T;
    private:
        T *ptr;
    public:
        constexpr unique_ptr() noexcept = default;
        constexpr unique_ptr(std::nullptr_t) noexcept {}
        constexpr explicit unique_ptr(pointer p) noexcept : ptr{p} {}

        template<class U>
        constexpr unique_ptr(unique_ptr<U> &&p) noexcept requires std::convertible_to<U*, T*>
                : ptr{p.release()} {}

        constexpr ~unique_ptr() noexcept { reset(); }

        constexpr unique_ptr &operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }
        template<class U>
        constexpr unique_ptr &operator=(unique_ptr<U> &&p) noexcept requires std::convertible_to<U*, T*> {
            reset(p.release());
            return *this;
        }

        constexpr pointer get() const noexcept { return ptr; }
        constexpr explicit operator bool() const noexcept { return get() != nullptr; }

        constexpr pointer release() noexcept { return std::exchange(ptr, nullptr); }
        constexpr void reset(pointer p = pointer{}) noexcept { delete release(); ptr = p; }
        constexpr void swap(unique_ptr &p) noexcept { std::swap(ptr, p.ptr); }

        constexpr typename std::add_lvalue_reference_t<T> operator*() const noexcept(noexcept(*std::declval<pointer>())) { return *get(); }
        constexpr pointer operator->() const noexcept { return get(); }
    };


    template<class T>
    class unique_ptr<T[]> {
    public:
        using pointer = T *;
        using element_type = T;
    private:
        T *ptr;
    public:
        constexpr unique_ptr() noexcept = default;
        constexpr unique_ptr(std::nullptr_t) noexcept {}
        constexpr explicit unique_ptr(pointer p) noexcept : ptr{p} {}

        template<class U>
        constexpr unique_ptr(unique_ptr<U> &&p) noexcept requires std::convertible_to<U(*)[], T(*)[]>
                : ptr{p.release()} {}

        constexpr ~unique_ptr() noexcept { reset(); }

        constexpr unique_ptr &operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }
        template<class U>
        constexpr unique_ptr &operator=(unique_ptr<U> &&p) noexcept requires std::convertible_to<U(*)[], T(*)[]> {
            reset(p.release());
            return *this;
        }

        constexpr pointer get() const noexcept { return ptr; }
        constexpr explicit operator bool() const noexcept { return get() != nullptr; }

        constexpr pointer release() noexcept { return std::exchange(ptr, nullptr); }
        constexpr void reset(pointer p = pointer{}) noexcept { delete release(); }
        constexpr void swap(unique_ptr &p) noexcept { std::swap(ptr, p.ptr); }

        constexpr T &operator[](std::size_t i) const noexcept { return get()[i]; }
    };

    template<class T, class... Args>
    constexpr unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

template<class T>
constexpr void std::swap(cx::unique_ptr<T> &a, cx::unique_ptr<T> &b) noexcept {
    a.swap(b);
}

#endif // FILE_UNIQUE_PTR_HPP
