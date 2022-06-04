#ifndef FILE_SHARED_PTR_HPP
#define FILE_SHARED_PTR_HPP
#include "function.hpp"

namespace cx {
    namespace detail {
        template<class T>
        struct shared_ptr_control_block {
            T *ptr;
            std::size_t n_shared;
            std::size_t n_weak;
            //function<void(T*)> deleter;
        };
    }

    // TODO: array variant, allocator, deleter, weak_ptr
    template<class T>
    class shared_ptr {
    private:
        using cb_t = detail::shared_ptr_control_block<T>;
        T *ptr{};
        cb_t *cb{};
    public:
        using element_type = std::remove_extent_t<T>;

        constexpr shared_ptr() noexcept = default;
        constexpr shared_ptr(std::nullptr_t) noexcept {}
        template<class U>
        constexpr shared_ptr(U *p) requires std::convertible_to<U*, T*>
                : ptr{p}, cb{new_cb(p)} {}
        constexpr shared_ptr(const shared_ptr &p) noexcept
                : ptr{p.ptr}, cb{p.cb} { inc(cb); }
        constexpr shared_ptr(shared_ptr &&p) noexcept
                : ptr{std::exchange(p.ptr, nullptr)},
                  cb{std::exchange(p.cb, nullptr)} {}

        template<class U>
        constexpr shared_ptr(unique_ptr<U> &&p) requires std::convertible_to<U*, T*>
                : cb(new_cb(p.release())) {}

        constexpr ~shared_ptr() { reset(); }

        template<class U>
        constexpr shared_ptr &operator=(const shared_ptr<U> &p) noexcept requires std::convertible_to<U*, T*> {
            if (&p != this) {
                reset();
                cb = p.cb;
                ptr = p.ptr;
                inc(cb);
            }
            return *this;
        }
        template<class U>
        constexpr shared_ptr &operator=(shared_ptr<U> &&p) noexcept requires std::convertible_to<U*, T*> {
            reset();
            cb = std::exchange(p.cb, nullptr);
            return *this;
        }

        template<class U>
        constexpr shared_ptr &operator=(unique_ptr<U> &&p) requires std::convertible_to<U*, T*> {
            reset(p.release());
            return *this;
        }

        constexpr void reset() noexcept {
            if (cb) {
                --cb->n_shared;
                --cb->n_weak;
                if (cb->n_shared == 0) {
                    delete cb->ptr;
                    cb->ptr = nullptr;
                }
                if (cb->n_weak == 0)
                    delete cb;
            }
            ptr = nullptr;
            cb = nullptr;
        }
        template<class U>
        constexpr void reset(U *p) requires std::convertible_to<U*, T*> {
            reset();
            ptr = p;
            cb = new cb_t{p.release(), 1, 1};
        }

        constexpr void swap(shared_ptr &p) noexcept {
            std::swap(ptr, p.ptr);
            std::swap(cb, p.cb);
        }

        constexpr element_type *get() const noexcept { return cb != nullptr ? ptr : nullptr; }
        constexpr element_type &operator*() const noexcept { return *get(); }
        constexpr element_type *operator->() const noexcept { return get(); }
        constexpr long use_count() const noexcept { return cb != nullptr ? static_cast<long>(cb->n_shared) : 0; }
        constexpr explicit operator bool() const noexcept { return cb != nullptr; }


    private:
        static constexpr cb_t *new_cb(T *ptr) {
            return new cb_t{ ptr, 1, 1 };
        }
        static constexpr void inc(cb_t *cb) {
            ++cb->n_shared;
            ++cb->n_weak;
        }
    };

    template<class T, class... Args>
    constexpr shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    constexpr shared_ptr<T> make_shared_for_overwrite() {
        return shared_ptr<T>(new T);
    }
}

#endif // FILE_SHARED_PTR_HPP
