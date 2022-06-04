#include <assert.h>
#include "unique_ptr.hpp"
#include "function.hpp"
#include "shared_ptr.hpp"

struct A {
    constexpr virtual ~A() noexcept = default;
    constexpr int get_value() const noexcept { return 42; }
};

consteval void unique_ptr_tests() {
    auto u1 = cx::make_unique<int>(10);
    auto u2 = cx::make_unique<int>(20);

    assert(*u1 == 10);
    assert((*u1 + *u2) == 30);

    auto u3 = std::move(u2);
    assert(u2.get() == nullptr);

    // Bug in GCC <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105846>
    // auto x1 = cx::make_unique<A>();
}

consteval void function_tests() {
    auto lambda = [](int x) { return x; };
    auto f = cx::function<int(int)>{ lambda };

    assert(f(4) == 4);
    //assert(f.target<decltype(lambda)>() != nullptr);
}

consteval void shared_ptr_tests() {
    auto p = cx::make_shared<int>(10);
    assert(p.use_count() == 1);

    auto p2 = p;
    assert(p.use_count() == 2);

    assert(*p == 10);

    auto p3 = std::move(p2);
    assert(p.use_count() == 2);
    assert(p2.use_count() == 0);
}

int main() {
    unique_ptr_tests();
    function_tests();
    shared_ptr_tests();
}