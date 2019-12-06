// int_to_str_conversion.cpp
// benchmark for int to string conversions
// by liliyayaqiqi
// 2019

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <cinttypes>

template <typename tFunc>
void run(const char* title, tFunc func) {
    const auto start = std::chrono::steady_clock::now();
    (void)func();
    const auto end = std::chrono::steady_clock::now();
    std::cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms\n";
}

std::vector<int> generateTestVec(size_t count) {
    std::vector<int> vec(count);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    std::generate_n(std::begin(vec), count, [&dist, &rng] {return dist(rng); });

    return vec;
}


void benchmark(uint32_t times, uint32_t size) {
    const auto testIntVec = generateTestVec(size);
    std::vector<std::string> testStrVec(testIntVec.size());

    std::string temp(15, ' ');

    //
    // from_chars/to_chars
    //

    run("to_chars", [&]() {
        for (size_t iter = 0; iter < times; ++iter) {
            for (size_t i = 0; i < testIntVec.size(); ++i) {
                const auto res = std::to_chars(temp.data(), temp.data() + temp.size(), testIntVec[i]);
                testStrVec[i] = std::string_view(temp.data(), res.ptr - temp.data());
            }
        }
    });

    //
    // to_string / stoi
    //

    run("to_string", [&]() {
        for (size_t iter = 0; iter < times; ++iter) {
            for (size_t i = 0; i < testIntVec.size(); ++i)
                testStrVec[i] = std::to_string(testIntVec[i]);
        }
    });

    //
    // sprintf / atoi
    //

    run("sprintf", [&]() {
        for (size_t iter = 0; iter < times; ++iter) {
            for (size_t i = 0; i < testIntVec.size(); ++i) {
                auto res = snprintf(temp.data(), 15, "%d", testIntVec[i]);
                testStrVec[i] = std::string_view(temp.data(), (temp.data() + res) - temp.data());
            }
        }
    });

    // ostringstream / istringstream

    run("otringstream", [&]() {
        for (size_t iter = 0; iter < times; ++iter) {
            for (size_t i = 0; i < testIntVec.size(); ++i) {
                std::ostringstream ss;
                ss << testIntVec[i];
                testStrVec[i] = ss.str();
            }
        }
    });
}

int main() { 
    const uint32_t times = 1000;
    std::cout << "test iterations: " << times << '\n';
    const uint32_t size = 1000;
    std::cout << "vector size: " << size << '\n';

    benchmark(times, size);
}