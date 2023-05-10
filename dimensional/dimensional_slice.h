#ifndef DIMENSIONAL_SLICE_H
#define DIMENSIONAL_SLICE_H

#include "traits.h"
#include "auxiliary.h"
#include <cassert>
#include <numeric>

template<size_t N>
struct DimensionalSlice {

    DimensionalSlice();
    DimensionalSlice(size_t s, std::initializer_list<size_t> exts);
    DimensionalSlice(size_t s, std::initializer_list<size_t> exts, std::initializer_list<size_t> strs);

    template<typename... Dims>
    DimensionalSlice(Dims... dims);

    template<typename... Dims>
    size_t operator()(Dims... dims) const;
    
    size_t size;
    size_t start;
    std::array<size_t, N> extents;
    std::array<size_t, N> strides;
};

template<size_t N>
DimensionalSlice<N>::DimensionalSlice() :size(1), start(0)
{
    std::fill(extents.begin(), extents.end(), 0);
    std::fill(strides.begin(), strides.end(), 1);
}

template<size_t N>
DimensionalSlice<N>::DimensionalSlice(size_t s, std::initializer_list<size_t> exts)
: start(s)
{
    assert(exts.size() == N);
    std::copy(exts.begin(), exts.end(), extents.begin());
    size = calcu_strides(extents, strides);
}

template<size_t N>
DimensionalSlice<N>::DimensionalSlice(size_t s, std::initializer_list<size_t> exts, std::initializer_list<size_t> strs)
: start(s)
{
    assert(exts.size() == N);
    std::copy(exts.begin(), exts.end(), extents.begin());
    std::copy(strs.begin(), strs.end(), strides.begin());
    size = calcu_size(extents);
}

template<size_t N>
template<typename... Dims>
DimensionalSlice<N>::DimensionalSlice(Dims... dims)
: start(0)
{
    static_assert(sizeof...(Dims) == N, "DimensionalSlice(Dims...) : dimension mismatch");

    size_t args[N] { size_t(dims)... };

    std::copy(args, args + N, extents.begin());
    size = calcu_strides(extents, strides);
}

template<size_t N>
template<typename... Dims>
size_t DimensionalSlice<N>::operator()(Dims... dims) const
{
    static_assert(sizeof...(Dims) == N, "DimensionalSlice::operator() : dimension mismatch");

    size_t args[N] { size_t(dims)... };

    return start + std::inner_product(args, args + N, strides.begin(), size_t(0));
}


#endif