#ifndef DIMENSIONAL_BASE_H
#define DIMENSIONAL_BASE_H

#include "traits.h"
#include "auxiliary.h"
#include "dimensional_slice.h"

template<typename T, size_t N>
using Dimensional_initializer = typename Dimensional_init<T, N>::type;

template<typename T, size_t N>
class DimensionalBase {
public:
    static constexpr size_t order = N;
    using value_type = T;

    // constructor
    DimensionalBase() = default;
    DimensionalBase(DimensionalBase&&) = default;   // move constructor
    DimensionalBase(DimensionalBase const&) = default;  // copy constructor
    ~DimensionalBase() = default;
    // assign
    DimensionalBase& operator=(DimensionalBase&&) = default;    // move assign
    DimensionalBase& operator=(DimensionalBase const&) = default;   // copy assgin

    // specify the extents
    template<typename... Exts>
    explicit DimensionalBase(Exts... exts) : desc{exts...} {}
    explicit DimensionalBase(const DimensionalSlice<N> &ds) : desc{ds} {}

    // static constexpr size_t order() { return N; }
    size_t extent(size_t n) const { assert(n < order); return desc.extents[n]; }
    const DimensionalSlice<N>& descriptor() const { return desc; }

    virtual size_t size() const = 0;
    virtual T* data() = 0;
    virtual const T* data() const = 0;

public:
    size_t n_rows() const { return desc.extents[0]; }
    size_t n_cols() const { return desc.extents[1]; }

    // d(i, j, k) access with integers
    template<typename... Args>
    Enable_if<Requesting_element<Args...>(), T&> operator()(Args... args);
    template<typename... Args>
    Enable_if<Requesting_element<Args...>(), const T&> operator()(Args... args) const;

protected:
    DimensionalSlice<N> desc;
};

template<typename T, size_t N>
    template<typename... Args>
Enable_if<Requesting_element<Args...>(), T&> 
DimensionalBase<T, N>::operator()(Args... args)
{
    assert(check_bounds(this->desc, args...));

    return *(data() + this->desc(args...));
}

template<typename T, size_t N>
    template<typename... Args>
Enable_if<Requesting_element<Args...>(), const T&> 
DimensionalBase<T, N>::operator()(Args... args) const
{
    assert(check_bounds(this->desc, args...));

    return *(data() + this->desc(args...));
}

#endif