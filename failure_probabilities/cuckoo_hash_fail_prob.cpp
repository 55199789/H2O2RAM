#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include "omp.h"

using namespace boost::multiprecision;
using namespace std;


typedef number<backends::cpp_bin_float<512>> mp_float;  // Arbitrary precision floating point

vector<int> partition(int n, int k) {
    int base = n / k;
    int remainder = n % k;
    vector<int> result(k, base);
    for (int i = 0; i < remainder; i++) {
        result[i] += 1;
    }
    return result;
}

mp_float log2_factorial(int n) {
    return n * log2(n) - n * log2(M_E) + 0.5 * log2(2 * M_PI * n);
}

// Log2 of binomial coefficient nCr
mp_float log2_nCr(int n, int r) {
    if (r == n) return 0;
    if (r > n) return -10000;
    return log2_factorial(n) - (log2_factorial(r) + log2_factorial(n - r));
}

mp_float prob(int n, int k) {
    mp_float ret = 0;
    int b = 2 * n;

    for (int t = k + 1; t <= n; t++) {
        vector<int> part = partition(t - 1, k);
        mp_float p1 = log2_nCr(n, t); 

        mp_float p2 = 0;
        for (int i : part) {
            p2 += log2_nCr(b / k, i);
        }
        p2 += log2_factorial(k);
        p2 = min(p2, log2_nCr(b, t - 1));

        mp_float p3 = t * k * log2(ceil(mp_float(t) / k));
        mp_float p4 = -k * t * log2(mp_float(b) / k);

        // Total probability for this t
        mp_float p = p1 + p2 + p3 + p4;

        // Adding the result to the final return value
        ret += mp_float(2) * p;
    }

    return ret;
}

int main() {
    vector<int> n_values;
    for (int i = 5; i < 22; i++) {
        n_values.push_back(static_cast<int>(pow(2, i))); 
    }
    
    vector<int> k_values = {3, 4, 5, 6};

    vector<vector<mp_float>> probabilities(n_values.size(), vector<mp_float>(k_values.size(), 0));

    // Compute probabilities for each n, k
    for (size_t i = 0; i < n_values.size(); ++i) {
        int n = n_values[i];
        cout << "Computing " << n << endl;

        for (size_t j = 0; j < k_values.size(); ++j) {
            int k = k_values[j];
            mp_float prob_value = prob(n, k);

            probabilities[i][j] = prob_value;

            // Check if probability is less than 2^-128 and break if needed
            if (prob_value < mp_float(1) / mp_float(pow(2, 128))) {
                // Fill the remaining with prob_value
                for (size_t k_fill = j; k_fill < k_values.size(); ++k_fill) {
                    probabilities[i][k_fill] = prob_value;
                }
                break;
            }
        }
    }

    for (size_t i = 0; i < n_values.size(); ++i) {
        for (size_t j = 0; j < k_values.size(); ++j) {
            cout << "P(" << n_values[i] << ", " << k_values[j] << ") = " << probabilities[i][j] << endl;
        }
    }

    return 0;
}