#pragma once
#include <ostream>
class DepthCounter
{
    static int depth;

public:
    DepthCounter()
    {
        ++depth;
    }

    friend std::ostream &operator<<(std::ostream &out, const DepthCounter &obj)
    {
        for (int i = 1; i < DepthCounter::depth; i++)
            out << '\t';
        return out;
    }

    ~DepthCounter()
    {
        --depth;
    }

    int get_depth() const
    {
        return depth;
    }
};