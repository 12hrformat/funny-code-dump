#include <iostream>
#include <type_traits>

template<int N>
struct evil {
    static constexpr int value =
        evil<N - 1>::value + evil<N - 1>::value;
};

template<>
struct evil<0> {
    static constexpr int value = 1;
};

int main() {
    std::cout
        << evil<
            []{
                int x = 0;
                for(int i=0;i<100;i++)
                    x += i;
                return x % 10;
            }()
        >::value;
}
