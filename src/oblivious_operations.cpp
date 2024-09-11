#include "oblivious_operations.hpp"

// void ORAM::oblivious_swap(uint8_t &left, uint8_t &right, bool flag)
// {
//     uint8_t mask = ~((uint8_t)flag - 1);
//     uint8_t *left_ptr = (uint8_t *)&left;
//     uint8_t *right_ptr = (uint8_t *)&right;
//     *left_ptr ^= *right_ptr;
//     *right_ptr ^= *left_ptr & mask;
//     *left_ptr ^= *right_ptr;
// }
