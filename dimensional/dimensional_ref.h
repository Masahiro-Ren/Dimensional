#ifndef DIMENSIONAL_REF_H
#define DIMENSIONAL_REF_H

#include "dimensional_base.h"
#include <vector>

template<typename T, size_t N>
class DimensionalRef : public DimensionalBase<T, N> {
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    DimensionalRef() = delete;
    DimensionalRef(DimensionalRef&&) = default;
    DimensionalRef(DimensionalRef const&) = default;
    // construct from DimensionalSlice and data pointer
    DimensionalRef(const DimensionalSlice<N>& s, T* p) : DimensionalBase<T, N>{s}, ptr{p} {}
    ~DimensionalRef() = default;
    // assign
    DimensionalRef& operator=(DimensionalRef&&) = default;
    DimensionalRef& operator=(DimensionalRef const&) = default;

    // construct from DimensionalRef
    //template<typename U>
    //DimensionalRef(const DimensionalRef<U, N>& dr);
    // assign from DimensionalRef
    //template<typename U>
    //DimensionalRef& operator=(const DimensionalRef<U, N>& dr);

    size_t size() const { return this->desc.size; }
    T* data() { return ptr; }
    const T* data() const { return ptr; }

public:
    using DimensionalBase<T, N>::operator();
    // dim(s1, s2, s3) slices index operation
    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), DimensionalRef<T, N>>
    operator()(const Args&... args);

    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), DimensionalRef<const T, N>>
    operator()(const Args&... args) const;

    // d[i] row access
    DimensionalRef<T, N - 1> operator[](size_t i) { return row(i); }
    DimensionalRef<const T, N - 1> operator[](size_t i) const { return row(i); }

    // row access
    DimensionalRef<T, N - 1> row(size_t n);
    DimensionalRef<const T, N - 1> row(size_t n) const;
    // col access
    DimensionalRef<T, N - 1> col(size_t n);
    DimensionalRef<const T, N - 1> col(size_t n) const;

private:
    T* ptr;
};

template<typename T, size_t N>
template<typename... Args>
Enable_if<Requesting_slice<Args...>(), DimensionalRef<T, N>> DimensionalRef<T, N>::operator()(const Args&... args)
{
    DimensionalSlice<N> ds;
    ds.start = this->desc.start + do_slice<N>(this->desc, ds, args...);
    ds.size = calcu_size<N>(ds.extents);
    return {ds, data()};
}

template<typename T, size_t N>
template<typename... Args>
Enable_if<Requesting_slice<Args...>(), DimensionalRef<const T, N>> DimensionalRef<T, N>::operator()(const Args&... args) const
{
    DimensionalSlice<N> ds;
    ds.start = this->desc.start + do_slice(this->desc, ds, args...);
    ds.size = calcu_size(ds.extents);
    return {ds, data()};
}

// row
template<typename T, size_t N>
DimensionalRef<T, N - 1> DimensionalRef<T, N>::row(size_t n)
{
    assert(n < this->n_rows());
    DimensionalSlice<N - 1> row;
    slice_dim<0>(n, this->desc, row);

    return {row, ptr};
}

template<typename T, size_t N>
DimensionalRef<const T, N - 1> DimensionalRef<T, N>::row(size_t n) const
{
    assert(n < this->n_rows());
    DimensionalSlice<N - 1> row;
    slice_dim<0>(n, this->desc, row);

    return {row, ptr};
}

// col
template<typename T, size_t N>
DimensionalRef<T, N - 1> DimensionalRef<T, N>::col(size_t n)
{
    assert(n < this->n_cols());
    DimensionalSlice<N - 1> col;
    slice_dim<1>(n, this->desc, col);

    return {col, ptr};
}

template<typename T, size_t N>
DimensionalRef<const T, N - 1> DimensionalRef<T, N>::col(size_t n) const
{
    assert(n < this->n_col());
    DimensionalSlice<N - 1> col;
    slice_dim<1>(n, this->desc, col);

    return {col, ptr};
}

#endif