#pragma once
#include <algorithm>
#include "oram.hpp"
template <typename T>
class MinHeap
{
private:
    ORAM::ObliviousRAM<uint32_t, T> heap;

    // initially i = size() - 1, which is public
    void heapify_up(uint32_t i)
    {
        while (i != 0)
        {
            T heap_i = heap[i];
            T &heap_parent = heap[parent(i)];

            ORAM::obliSwap(heap_i, heap_parent,
                           heap_parent > heap_i);
            heap[i] = heap_i;
            // std::swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    // initially i = size() - 1, which is public
    void heapify_down(uint32_t i)
    {
        // heap size is public
        if (i >= size())
            return;
        uint32_t left = left_child(i);
        uint32_t right = right_child(i);
        uint32_t smallest = i;
        T heap_i = heap[i];
        ORAM::CMOV(left < size() && heap[left] < heap_i, smallest, left);
        // smallest = ORAM::oblivious_select(smallest, left, left < size() && heap[left] < heap_i);
        T heap_smallest = heap[smallest];
        ORAM::CMOV(right < size() && heap[right] < heap_smallest, smallest, right);
        // smallest = ORAM::oblivious_select(smallest, right, right < size() && heap[right] < heap_smallest);
        T &heap_smallest_ = heap[smallest];
        ORAM::obliSwap(heap_i, heap_smallest_, smallest != i);
        heap[i] = heap_i;
        heapify_down(ORAM::oblivious_select(smallest, right, smallest == i));
    }

    inline uint32_t parent(uint32_t i) const { return (i - 1) / 2; }
    inline uint32_t left_child(uint32_t i) const { return 2 * i + 1; }
    inline uint32_t right_child(uint32_t i) const { return 2 * i + 2; }

public:
    inline size_t size() const { return heap.size(); }

    inline bool empty() const { return heap.empty(); }

    void push(T key)
    {
        heap.push(key);
        heapify_up(size() - 1);
    }

    void pop()
    {
        heap[0] = heap[size() - 1];
        heap.pop_back();
        heapify_down(0);
    }

    T top() const
    {
        if (!empty())
            return heap[0];
        return T();
        // return -1; // or throw an exception
    }
};