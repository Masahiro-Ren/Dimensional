#ifndef DIMENSIONAL_H
#define DIMENSIONAL_H

#include "dimensional_base.h"
#include "dimensional_ref.h"

template<typename T, size_t N>
class Dimensional : public DimensionalBase<T, N> {
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    Dimensional() = default;
    Dimensional(Dimensional&&) = default;
    Dimensional(Dimensional const&) = default;
    // construct from initializer
    Dimensional(Dimensional_initializer<T, N>);
    ~Dimensional() = default;
    // assign
    Dimensional& operator=(Dimensional&&) = default;
    Dimensional& operator=(Dimensional const&) = default;

    // construct from ref
    template<typename U>
        Dimensional(const DimensionalRef<U, N>&);
    // assign from ref
    template<typename U>
        Dimensional& operator=(const DimensionalRef<U, N>&);
    // specify extents
    template<typename... Exts>
        explicit Dimensional(Exts... exts);
    
    // don't use {} except for elements
    template<typename U>
        Dimensional(std::initializer_list<U>) = delete;
    template<typename U>
        Dimensional& operator=(std::initializer_list<U>) = delete;
    
    size_t extent(size_t n) const { return this->desc.extents[n]; }
    size_t size() const { return elems.size(); }
    const DimensionalSlice<N>& descriptor() const { return this->desc; }

    T* data() { return elems.data(); }
    const T* data() const { return elems.data(); }

public:
    // index and slice
    // dim(i, j, k) integer index operation
    using DimensionalBase<T, N>::operator();
    // dim(s1, s2, s3) slice index operation
    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), DimensionalRef<T, N>> 
    operator()(const Args&... args);
    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), DimensionalRef<const T, N>> 
    operator()(const Args&... args) const;

    DimensionalRef<T, N - 1> operator[](size_t i) { return row(i); }
    DimensionalRef<const T, N - 1> operator[](size_t i) const { return row(i); }

    DimensionalRef<T, N - 1> row(size_t n);
    DimensionalRef<const T, N - 1> row(size_t n) const;

    DimensionalRef<T, N - 1> col(size_t n);
    DimensionalRef<const T, N - 1> col(size_t n) const;

private:
    std::vector<T> elems;
};

template<typename T, size_t N>
Dimensional<T, N>::Dimensional(Dimensional_initializer<T, N> init)
{
    this->desc.start = 0;
    this->desc.extents =  derive_extents<N>(init);
    this->desc.size = calcu_strides(this->desc.extents, this->desc.strides);
    elems.reserve(this->desc.size);
    insert_flat(init, elems);

    assert(elems.size() == this->desc.size);
}

template<typename T, size_t N>
    template<typename U>
Dimensional<T, N>::Dimensional(const DimensionalRef<U, N>& x)
    : DimensionalBase<T, N>::desc{x.desc}, elems{x.begin(), x.end()}
{
    static_assert(Convertible<U, T>(), "Dimensional constructor: incompatible element types.");
}

template<typename T, size_t N>
    template<typename U>
Dimensional<T, N>& Dimensional<T, N>::operator=(const DimensionalRef<U, N>& x)
{
    static_assert(Convertible<U, T>(), "Dimensional constructor: incompatible element types.");

    this->desc = x.desc;
    elems.assign(x.begin(), x.end());
    return *this;
}

template<typename T, size_t N>
    template<typename... Exts>
Dimensional<T, N>::Dimensional(Exts... exts)
    :DimensionalBase<T, N>::desc{exts...},
    elems(this->desc.size)
{}

template<typename T, size_t N>
template<typename... Args>
Enable_if<Requesting_slice<Args...>(), DimensionalRef<T, N>> Dimensional<T, N>::operator()(const Args&... args)
{
    DimensionalSlice<N> ds;
    ds.start = this->desc.start + do_slice<N>(this->desc, ds, args...);
    ds.size = calcu_size(ds.extents);
    return {ds, data()};
}

template<typename T, size_t N>
template<typename... Args>
Enable_if<Requesting_slice<Args...>(), DimensionalRef<const T, N>> Dimensional<T, N>::operator()(const Args&... args) const
{
    DimensionalSlice<N> ds;
    ds.start = this->desc.start + do_slice(this->desc, ds, args...);
    ds.size = calcu_size(ds.extents);
    return {ds, data()};
}

// row
template<typename T, size_t N>
DimensionalRef<T, N - 1> Dimensional<T, N>::row(size_t n)
{
    assert(n < this->n_rows());
    DimensionalSlice<N - 1> row;
    slice_dim<0>(n, this->desc, row);

    return {row, data()};
}

template<typename T, size_t N>
DimensionalRef<const T, N - 1> Dimensional<T, N>::row(size_t n) const
{
    assert(n < this->n_rows());
    DimensionalSlice<N - 1> row;
    slice_dim<0>(n, this->desc, row);

    return {row, data()};
}

// col
template<typename T, size_t N>
DimensionalRef<T, N - 1> Dimensional<T, N>::col(size_t n)
{
    assert(n < this->n_cols());
    DimensionalSlice<N - 1> col;
    slice_dim<1>(n, this->desc, col);

    return {col, data()};
}

template<typename T, size_t N>
DimensionalRef<const T, N - 1> Dimensional<T, N>::col(size_t n) const
{
    assert(n < this->n_col());
    DimensionalSlice<N - 1> col;
    slice_dim<1>(n, this->desc, col);

    return {col, data()};
}

#endif