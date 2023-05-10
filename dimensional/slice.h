#ifndef SLICE_H
#define SLICE_H

#include <cstdlib>

struct slice { 

    slice() :start(-1), length(-1), stride(1) {}
    explicit slice(size_t s) :start(s), length(-1), stride(1) {}
    slice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) {}

    size_t operator()(size_t i) const { return start + i * stride; }

    static slice all;

    size_t start;
    size_t length;
    size_t stride;
};

#endif