#pragma once
#include "oram.hpp"

template <typename node_type, typename size_type = uint32_t>
class OAVL_Tree
{
    // child index pointer class
    typedef struct tag_child_type
    {
        size_type left;
        size_type right;
    } child_type;

    ORAM::ObliviousRAM<size_type, child_type> child_;
    ORAM::ObliviousRAM<size_type, std::int8_t> balance_;
    ORAM::ObliviousRAM<size_type, node_type> nodes;
    ORAM::ObliviousRAM<size_type, size_type> parent_;
    size_type size_; // actual size
    size_type root_; // root node
};