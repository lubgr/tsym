#ifndef TSYM_INCLUDE_TSYM_TRAITS_H
#define TSYM_INCLUDE_TSYM_TRAITS_H

#include <type_traits>

namespace tsym {
    namespace detail {
        namespace traits {
            struct OpTest {
                template <class Vec, class ArgType>
                static auto unaryCallOp(int)
                  -> decltype(std::declval<Vec&>()(std::declval<ArgType>()), std::true_type());
                template <class Vec, class ArgType> static auto unaryCallOp(...) -> std::false_type;

                template <class Vec, class ArgType>
                static auto unaryArrayOp(int)
                  -> decltype(std::declval<Vec&>()[std::declval<ArgType>()], std::true_type());
                template <class, class> static auto unaryArrayOp(...) -> std::false_type;

                template <class Matrix, class ArgType>
                static auto binaryCallOp(int) -> decltype(
                  std::declval<Matrix&>()(std::declval<ArgType>(), std::declval<ArgType>()), std::true_type());
                template <class, class> static auto binaryCallOp(...) -> std::false_type;

                template <class Matrix, class ArgType>
                static auto binaryArrayOp(int) -> decltype(
                  std::declval<Matrix&>()[std::declval<ArgType>()][std::declval<ArgType>()], std::true_type());
                template <class, class> static auto binaryArrayOp(...) -> std::false_type;
            };

            template <class T, class ArgType> struct UnaryCallOp : decltype(OpTest::unaryCallOp<T, ArgType>(0)) {};

            template <class T, class ArgType> struct UnaryArrayOp : decltype(OpTest::unaryArrayOp<T, ArgType>(0)) {};

            template <class T, class ArgType> struct BinaryCallOp : decltype(OpTest::binaryCallOp<T, ArgType>(0)) {};

            template <class T, class ArgType> struct BinaryArrayOp : decltype(OpTest::binaryArrayOp<T, ArgType>(0)) {};
        }

        template <class T, class ArgType>
        inline constexpr bool hasConstUnaryCallOp = traits::UnaryCallOp<std::add_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasConstUnaryArrayOp = traits::UnaryArrayOp<std::add_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasConstBinaryCallOp = traits::BinaryCallOp<std::add_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasConstBinaryArrayOp = traits::BinaryArrayOp<std::add_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasUnaryCallOp = traits::UnaryCallOp<std::remove_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasUnaryArrayOp = traits::UnaryArrayOp<std::remove_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasBinaryCallOp = traits::BinaryCallOp<std::remove_const_t<T>, ArgType>::value;

        template <class T, class ArgType>
        inline constexpr bool hasBinaryArrayOp = traits::BinaryArrayOp<std::remove_const_t<T>, ArgType>::value;
    }
}

#endif
