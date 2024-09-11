#include <iterator> // for std::forward_iterator_tag and other iterator properties

namespace ORAM
{
    template <std::unsigned_integral IndexType,
              typename ValueType>
    class ObliviousRAM;

    template <std::unsigned_integral IndexType, typename ValueType>
    class ObliviousRAMIterator
    {
    public:
        using value_type = ValueType;
        using pointer = ValueType *;
        using reference = ValueType &;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        ObliviousRAM<IndexType, ValueType> *oram;
        IndexType index;

    public:
        // default constructor
        ObliviousRAMIterator() : oram(nullptr), index(0) {}

        ObliviousRAMIterator(ObliviousRAM<IndexType, ValueType> *oram,
                             IndexType index = 0) : oram(oram), index(index) {}
        // Dereference operators
        reference operator*() const
        {
            assert(false);
            return (*oram)[index];
        }

        // Increment and decrement
        ObliviousRAMIterator &operator++()
        {
            ++index;
            return *this;
        }
        ObliviousRAMIterator operator++(int)
        {
            ObliviousRAMIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        ObliviousRAMIterator &operator--()
        {
            --index;
            return *this;
        }
        ObliviousRAMIterator operator--(int)
        {
            ObliviousRAMIterator tmp = *this;
            --(*this);
            return tmp;
        }

        // Arithmetic operations
        ObliviousRAMIterator operator+(difference_type n) const
        {
            return ObliviousRAMIterator(oram, index + n);
        }
        ObliviousRAMIterator &operator+=(difference_type n)
        {
            index += n;
            return *this;
        }
        ObliviousRAMIterator operator-(difference_type n) const
        {
            return ObliviousRAMIterator(oram, index - n);
        }
        difference_type operator-(const ObliviousRAMIterator &other) const
        {
            return difference_type(index) - difference_type(other.index);
        }
        ObliviousRAMIterator &operator-=(difference_type n)
        {
            index -= n;
            return *this;
        }

        // Comparison operators
        bool operator==(const ObliviousRAMIterator &other) const
        {
            return oram == other.oram && index == other.index;
        }
        bool operator!=(const ObliviousRAMIterator &other) const
        {
            return !(oram == other.oram && index == other.index);
        }
        bool operator<(const ObliviousRAMIterator &other) const
        {
            return oram == other.oram && index < other.index;
        }
        bool operator<=(const ObliviousRAMIterator &other) const
        {
            return oram == other.oram && index <= other.index;
        }
        bool operator>(const ObliviousRAMIterator &other) const
        {
            return oram == other.oram && index > other.index;
        }
        bool operator>=(const ObliviousRAMIterator &other) const
        {
            return oram == other.oram && index >= other.index;
        }

        // Subscript
        reference operator[](difference_type n) const { return (*oram)[index + n]; }
    };

    template <std::unsigned_integral IndexType, typename ValueType>
    class ObliviousRAMIteratorReverse
    {
    public:
        using value_type = ValueType;
        using pointer = ValueType *;
        using reference = ValueType &;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        ObliviousRAM<IndexType, ValueType> *oram;
        IndexType index;

    public:
        // default constructor
        ObliviousRAMIteratorReverse() : oram(nullptr), index(0)
        {
            std::cout << "() index: " << index << std::endl;
        }

        ObliviousRAMIteratorReverse(ObliviousRAM<IndexType, ValueType> *oram,
                                    IndexType index = 0) : oram(oram), index(index)
        {
            std::cout << "( ) index: " << index << std::endl;
        }
        // Dereference operators
        reference operator*() const { return (*oram)[index]; }

        // Increment and decrement
        ObliviousRAMIteratorReverse &operator++()
        {
            --index;
            return *this;
        }
        ObliviousRAMIteratorReverse operator++(int)
        {
            ObliviousRAMIteratorReverse tmp = *this;
            ++(*this);
            return tmp;
        }
        ObliviousRAMIteratorReverse &operator--()
        {
            ++index;
            return *this;
        }
        ObliviousRAMIteratorReverse operator--(int)
        {
            ObliviousRAMIteratorReverse tmp = *this;
            --(*this);
            return tmp;
        }

        // Arithmetic operations
        ObliviousRAMIteratorReverse operator+(difference_type n) const
        {
            return ObliviousRAMIteratorReverse(oram, index - n);
        }
        ObliviousRAMIteratorReverse &operator+=(difference_type n)
        {
            index -= n;
            return *this;
        }
        ObliviousRAMIteratorReverse operator-(difference_type n) const
        {
            return ObliviousRAMIteratorReverse(oram, index + n);
        }
        difference_type operator-(const ObliviousRAMIteratorReverse &other) const
        {
            return difference_type(other.index) - difference_type(index);
        }
        ObliviousRAMIteratorReverse &operator-=(difference_type n)
        {
            index += n;
            return *this;
        }

        // Comparison operators
        bool operator==(const ObliviousRAMIteratorReverse &other) const
        {
            return oram == other.oram && index == other.index;
        }
        bool operator!=(const ObliviousRAMIteratorReverse &other) const
        {
            return !(oram == other.oram && index == other.index);
        }
        bool operator<(const ObliviousRAMIteratorReverse &other) const
        {
            return oram == other.oram && index > other.index;
        }
        bool operator<=(const ObliviousRAMIteratorReverse &other) const
        {
            return oram == other.oram && index >= other.index;
        }
        bool operator>(const ObliviousRAMIteratorReverse &other) const
        {
            return oram == other.oram && index < other.index;
        }
        bool operator>=(const ObliviousRAMIteratorReverse &other) const
        {
            return oram == other.oram && index <= other.index;
        }

        // Subscript
        reference operator[](difference_type n) const { return (*oram)[index + n]; }
    };
}
