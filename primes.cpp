#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>

using namespace std;

int largest[10];

//For returning the count and sums of sieve segments
struct prime_vals {
    long long count = 0;
    long long sum = 0;  
};

//For marking multiples of a prime as non prime
void markMultiples(vector<bool>& prime, int start, int end, int p) {
    for (int i = max(p * p, start); i <= end; i += p) {
        prime[i] = false;
    }
}

//Calculating The Sieve of Eratosthenes for segments 
prime_vals countPrimes(const vector<bool>& mainPrimes, int start, int end, int limit) {
    vector<bool> localPrimes(end - start + 1, true);
    prime_vals vals;

    // Initialize local primes using main primes
    for (int p = 2; p <= limit; ++p) {
        if (mainPrimes[p]) {
            // Find the first multiple of p in the segment
            int loLim = (start + p - 1) / p * p;
            for (int j = max(loLim, p * p); j <= end; j += p) {
                localPrimes[j - start] = false;
            }
        }
    }

    //Identifying primes in a given set of integers
    for (int p = start; p <= end; p++) {
        if (localPrimes[p - start]) {
            vals.sum += static_cast<long long>(p);
            vals.count++;
        }
    }

    //If we are in the last segment count back 10 to get the top 10 primes
    if (end == 100000000) {
    int count = 0;
    for (int i = end; i >= start && count < 10; --i) {
        if (localPrimes[i - start]) {
            largest[count] = i;
            count++;
        }
    }
}
    return vals;
}

int main() {
    auto start = chrono::high_resolution_clock::now();
    const int n = 100000000;

    //For keeping track of Primes
    vector<bool> mainPrimes(static_cast<int>(sqrt(n)) + 1, true);
    mainPrimes[0] = mainPrimes[1] = false;

    //Initializing the Sieve 
    int limit = static_cast<int>(sqrt(n));
    for (int p = 2; p <= limit; ++p) {
        if (mainPrimes[p]) {
            markMultiples(mainPrimes, p * p, limit, p);
        }
    }

    //Storing outputs of future threads
    vector<future<prime_vals>> futures;
    int segmentSize = 12500000; 

    //Launching 8 threads since 10e8 / 12500000 = 8
    for (int i = 2; i <= n; i += segmentSize) {
        int end = min(i + segmentSize - 1, n);
        futures.push_back(async(launch::async, countPrimes, ref(mainPrimes), i, end, limit));
    }

    //Adding up sum and count results
    long long totalCount = 0;
    long long totalSum = 0;
    for (auto &f : futures) {
        prime_vals result = f.get();
        totalCount += result.count;
        totalSum += result.sum;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    //prinitng to output file
    ofstream outfile("primes.txt");
    outfile << "Execution Time: " << duration << " ms, Total number of primes: " << totalCount << ", Sum of all primes: " << totalSum << endl;
    outfile << "Top Ten Primes:" << endl;
    
    for(int i = 9; i >= 0; i--)
        outfile << largest[i] << " ";
    
    outfile << endl;

    return 0;
}
