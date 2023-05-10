#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <initializer_list>
#include <array>
#include <algorithm>
#include "slice.h"
#include "dimensional_slice.h"


template<typename List>
bool check_non_jagged(const List& list)
{
    auto i = list.begin();
    for(auto j = i + 1; j != list.end(); ++j)
    {
        if(i->size() != j->size())
            return false;
    }

    return true;
}

template<typename T, typename Vec>
void add_list(const T* first, const T* last, Vec& vec)
{
    vec.insert(vec.end(), first, last);
}

template<typename T, typename Vec>
void add_list(const std::initializer_list<T>* first, const std::initializer_list<T>* last, Vec& vec)
{
    for(; first != last; ++first)
        add_list(first->begin(), first->end(), vec);
}

template<typename T, typename Vec>
void insert_flat(std::initializer_list<T> list, Vec& vec)
{
    add_list(list.begin(), list.end(), vec);
}

template<size_t N, typename I, typename List>
Enable_if<(N == 1), void> add_extents(I& first, const List& list)
{
    //*first++ = list.size();
    *first = list.size();
}

template<size_t N, typename I, typename List>
Enable_if<(N > 1), void> add_extents(I& first, const List& list)
{
    assert(check_non_jagged(list));
    *first = list.size();
    add_extents<N - 1>(++first, *list.begin());
}

template<size_t N, typename List>
std::array<size_t, N> derive_extents(const List& list)
{
    std::array<size_t, N> a;
    auto f = a.begin();
    add_extents<N>(f, list);
    return a;
}

template<size_t N>
size_t calcu_strides(const std::array<size_t, N>& exts, std::array<size_t, N>& strs)
{
    size_t s = 1;
    for(int i = N - 1; i >= 0; i--)
    {
        strs[i] = s;
        s *= exts[i];
    }

    return s;
}

template<size_t N>
size_t calcu_size(const std::array<size_t, N>& exts)
{
    size_t s = 1;
    for(int i = 0; i > N; i++)   
    {
        s *= exts[i];
    }

    return s;
}

template<typename... Args>
constexpr bool Requesting_element()
{
    return All(Convertible<Args, size_t>()...);
}

template<typename... Args>
constexpr bool Requesting_slice()
{
    return All((Convertible<Args, size_t>() || Same<Args, slice>())...)
           && Some(std::is_same<Args, slice>()...);
}

template<size_t N, typename... Dims>
bool check_bounds(const DimensionalSlice<N>& ds, Dims... dims)
{
    size_t indexes[N] { size_t{static_cast<size_t>(dims)}... };
    return std::equal(indexes, indexes + N, ds.extents.begin(), std::less<size_t>{});
}

//========================== Initializer part ====================================
template<typename T, size_t N>
struct Dimensional_init {
    using type = std::initializer_list<typename Dimensional_init<T, N-1>::type>;
};

template<typename T>
struct Dimensional_init<T, 1> {
    using type = std::initializer_list<T>;
};

template<typename T>
struct Dimensional_init<T, 0>; // set N == 0 undefined

//========================== Initializer part end ====================================

//========================== Slice part ====================================

template<size_t I, size_t N>
void slice_dim(size_t offset, const DimensionalSlice<N>& ds, DimensionalSlice<N>& row)
{
    row.start = ds.start;
    int j = (int)N - 2;
    for(int i = N - 1; i >=0; i--)
    {
        if(i == I)
        {
            row.start += ds.strides[i] * offset;
        }
        else
        {
            row.extents[j] = ds.extents[i];
            row.strides[j] = ds.strides[i];
            j--;
        }

        row.size = calcu_size(row.extents);
    }
}

template <size_t NRest, size_t N>
size_t do_slice_dim(const DimensionalSlice<N>& os, DimensionalSlice<N> &ns, slice s)
{
    size_t i = N - NRest;
    ns.strides[i] = s.stride * os.strides[i];
    ns.extents[i] = ( s.length == size_t(-1) ? 
                        (os.extents[i] - s.start + s.stride - 1) / s.stride :
                        s.length );

    return s.start * os.strides[i];
}

template <size_t NRest, size_t N>
size_t do_slice_dim(const DimensionalSlice<N>& os, DimensionalSlice<N>& ns, size_t s)
{
    size_t i = N - NRest;
    ns.strides[i] = os.strides[i];
    ns.extents[i] = 1;

    return s * ns.strides[i];
}

template<size_t N, typename T, typename... Args>
size_t do_slice(const DimensionalSlice<N>& os, DimensionalSlice<N>& ns, const T& s, const Args&... args)
{
    size_t m = do_slice_dim<sizeof...(Args) + 1>(os, ns, s);
    size_t n = do_slice(os, ns, args...);

    return m + n;
}

template<size_t N>
size_t do_slice(const DimensionalSlice<N>& os, DimensionalSlice<N>& ns)
{
    passby(os);
    passby(ns);
    return 0;
}

// avoid warning
template<size_t N>
void passby(const DimensionalSlice<N>& ds) {}

//========================== Slice part end ====================================


#endif
