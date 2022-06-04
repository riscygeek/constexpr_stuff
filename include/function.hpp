#ifndef FILE_FUNCTION_HPP
#define FILE_FUNCTION_HPP
#include <functional>
#include "unique_ptr.hpp"

namespace cx {
    template<class T>
    class function;

    template<class R, class... Args>
    class function<R(Args...)> {
    private:
        struct function_base {
            constexpr function_base() noexcept = default;
            function_base(const function_base &) = delete;
            constexpr function_base(function_base &&) = default;
            constexpr virtual ~function_base() = default;

            function_base &operator=(const function_base &) = delete;
            function_base &operator=(function_base &&) = delete;

            constexpr virtual R operator()(Args&&... args) const = 0;
            constexpr virtual unique_ptr<function_base> clone() const = 0;
            constexpr virtual const std::type_info &target_type() const noexcept = 0;
            constexpr virtual void *get() const noexcept = 0;
        };
        template<class T>
        struct function_impl : function_base {
            T object;

            constexpr function_impl(T &&object) : object(object) {}
            ~function_impl() override = default;

            constexpr R operator()(Args&&... args) const override {
                return object(std::forward<Args>(args)...);
            }
            constexpr unique_ptr<function_base> clone() const override {
                return make_unique<function_impl>(object);
            }
            constexpr const std::type_info &target_type() const noexcept override {
                return typeid(T);
            }
            constexpr void *get() const noexcept override {
                return &object;
            }
        };
        unique_ptr<function_base> base{};
    public:
        using result_type = R;
        constexpr function() noexcept = default;
        constexpr function(std::nullptr_t) noexcept {}
        constexpr function(const function &f) : base{f.base->clone()} {}
        constexpr function(function &&f) noexcept : base{std::move(f.base)} {}
        template<class F>
        constexpr function(F &&f) noexcept requires std::invocable<F, Args...> : base{make_unique<function_impl<F>>(f)} {}

        constexpr function &operator=(const function &f) {
            base = f.base->clone();
            return *this;
        }
        constexpr function &operator=(function &&f) {
            base = std::move(f.base);
            return *this;
        }
        constexpr function &operator=(std::nullptr_t) {
            base.reset();
            return *this;
        }
        template<class F>
        constexpr function &operator=(F &&f) requires std::invocable<F, Args...> {
            base = make_unique<function_impl<F>>(f);
            return *this;
        }

        constexpr void swap(function &f) noexcept {
            std::swap(base, f.base);
        }

        constexpr explicit operator bool() const noexcept { return (bool)base; }
        constexpr R operator()(Args... args) const {
            if (base) {
                return (*base)(std::forward<Args>(args)...);
            } else {
                throw std::bad_function_call();
            }
        }

        constexpr const std::type_info &target_info() const noexcept {
            return base ? base->target_type() : typeid(void);
        }
        // Can't be constexpr!
        template<class T>
        T *target() noexcept {
            return target_info() == typeid(T) ? static_cast<T *>(base->get()) : nullptr;
        }
    };
}

template<class T>
constexpr void std::swap(cx::function<T> &a, cx::function<T> &b) noexcept {
    a.swap(b);
}

#endif // FILE_FUNCTION_HPP
