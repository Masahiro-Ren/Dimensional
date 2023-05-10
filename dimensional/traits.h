#ifndef TRAITS_H
#define TRAITS_H

#include <type_traits>

template<bool B, typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

template<typename U, typename T>
constexpr bool Same() { return std::is_same<U, T>::value; }

template<typename U, typename T>
constexpr bool Convertible() { return std::is_convertible<U, T>::value; }

constexpr bool All() { return true; }

template<typename... Args>
constexpr bool All(bool b, Args... args)
{
    return b && All(args...);
}

constexpr bool Some() { return false; }

template<typename... Args>
constexpr bool Some(bool b, Args... args)
{
    return b || Some(args...);
}

#endif