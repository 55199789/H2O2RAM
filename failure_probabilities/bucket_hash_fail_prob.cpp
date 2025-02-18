#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include "omp.h"

using namespace boost::multiprecision;
typedef number<backends::cpp_bin_float<512>> cpp_bin_float_512;
static cpp_bin_float_512 log2_factorial(size_t n)
{
    static const std::array<cpp_bin_float_512, 1 << 20> log2_factorial = []()
    {
        std::array<cpp_bin_float_512, 1 << 20> ret;
        ret[0] = 0;
        for (uint32_t i = 1; i < (1 << 20); ++i)
        {
            ret[i] = ret[i - 1] + log2(i);
        }
        return ret;
    }();
    if (n < 2)
        return 0;
    if (n < log2_factorial.size())
        return log2_factorial[n];
    // n * math.log2(n) - n * math.log2(math.e) + 0.5 * math.log2(2 * math.pi * n)
    // return n * log2(n / exp(1)) + 0.5 * log2(2 * M_PI * n);
    return n * log2(n / exp(1)) + (2 * log2(n) + 2 + log2(1 + 2 * n)) / 6.0 + 0.5 * log2(M_PI);
}

static cpp_bin_float_512 log2_nCr(size_t n, size_t r)
{
    if (r == n)
        return 0;
    if (r > n)
        return -10000;
    return log2_factorial(n) - (log2_factorial(r) + log2_factorial(n - r));
}

static cpp_bin_float_512 bin_load(size_t n, size_t m, size_t k)
{
    if (n == k)
        return boost::multiprecision::pow(cpp_bin_float_512(1.0 / m), k);
    if (k == 0)
        return boost::multiprecision::pow(cpp_bin_float_512(1 - 1.0 / m), n);
    cpp_bin_float_512 ret = log2_nCr(n, k) + (n - k) * std::log2(m - 1) - n * std::log2(m);
    return boost::multiprecision::pow(2, ret);
}

static cpp_bin_float_512 fail_prob(size_t n, size_t m, size_t k)
{
    cpp_bin_float_512 st = bin_load(n, m, k);
    cpp_bin_float_512 ret = st;
    for (size_t t = k + 1; t <= n; ++t)
    {
        st *= (n - t + 1.0) / t / (m - 1);
        ret += st;
    }
    return m * ret;
}

static size_t compute_bucket_size(size_t n, size_t m, int delta_inv_log2)
{
    size_t left = 2;
    size_t right = n;
    while (left < right)
    {
        size_t mid = (left + right) / 2;
        if (fail_prob(n, m, mid) <= std::pow(2, -delta_inv_log2))
        {
            right = mid;
        }
        else
        {
            left = mid + 1;
        }
    }
    return right;
}

static cpp_bin_float_512 fail_prob_lt(size_t n, size_t Z, size_t eps_inv)
{
    constexpr size_t N = 2;
    size_t m = 4ll * n / Z;
    cpp_bin_float_512 st = bin_load(n, m, 0);
    cpp_bin_float_512 ret = st;
    size_t ub = Z - Z / (2 * eps_inv);
    ub /= N;
    for (size_t t = 0; t < ub; ++t)
    {
        // st *= cpp_bin_float_512(n - t + 1.0) / cpp_bin_float_512(t) / cpp_bin_float_512(m - 1);
        st = bin_load(n, m, t);
        // std::cout << n << " balls into " << m << " bins, " << t << " balls in a bin, prob: " << st << std::endl;
        ret += st;
    }
    return m * ret;
}

int main(int argc, char **argv)
{
    int n = 8192;
    int bucket_num = 1;
    int delta_inv_log2 = 64;
    int step = 1;
    int T = 2 * n + 1;
    {
        std::vector<int> data(n);
        std::ofstream file("bucket_size_neg_log2_delta_64.txt");
        int cnt = n / 64;
#pragma omp parallel for
        for (int i = 1; i < T; i++)
        {
            data[i - 1] = compute_bucket_size(n, i, 64);
            // output progress
            if (omp_get_thread_num() == 0)
            {
                if (i % cnt == 0)
                    std::cout << "i: " << i << std::endl;
            }
        }
        for (int i = 1; i < T; i++)
            file << n << " " << i << " " << data[i - 1] << std::endl;
    }
    {
        std::vector<int> data(n);
        std::ofstream file("bucket_size_neg_log2_delta_128.txt");
        int cnt = n / 64;
#pragma omp parallel for
        for (int i = 1; i < T; i++)
        {
            data[i - 1] = compute_bucket_size(n, i, 128);
            // output progress
            if (omp_get_thread_num() == 0)
            {
                if (i % cnt == 0)
                    std::cout << "i: " << i << std::endl;
            }
        }
        for (int i = 1; i < T; i++)
            file << n << " " << i << " " << data[i - 1] << std::endl;
    }
    {
        std::vector<int> data(n);
        std::ofstream file("bucket_size_neg_log2_delta_256.txt");
        int cnt = n / 64;
#pragma omp parallel for
        for (int i = 1; i < T; i++)
        {
            data[i - 1] = compute_bucket_size(n, i, 256);
            // output progress
            if (omp_get_thread_num() == 0)
            {
                if (i % cnt == 0)
                    std::cout << "i: " << i << std::endl;
            }
        }
        for (int i = 1; i < T; i++)
            file << n << " " << i << " " << data[i - 1] << std::endl;
    }
    return 0;
}