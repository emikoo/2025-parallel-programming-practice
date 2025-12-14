#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <mutex>

std::vector<size_t> read_array() {
    size_t length, a, b, p;
    if (!(std::cin >> length >> a >> b >> p)) return {};
    
    std::vector<size_t> result(length);
    if (length == 0) return result;
    
    result[0] = a % p;
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] = (result[i - 1] * a + b) % p;
    }
    return result;
}

struct Range {
    size_t start;
    size_t end;
};

void parallel_sort(std::vector<size_t>& array) {
    if (array.size() < 10000) {
        std::sort(array.begin(), array.end());
        return;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    
    if (num_threads > array.size()) num_threads = array.size();

    size_t len = array.size();
    size_t chunk_size = len / num_threads;
    
    std::vector<std::thread> threads;
    std::vector<Range> ranges;
    
    for (unsigned int i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == num_threads - 1) ? len : start + chunk_size;
        ranges.push_back({start, end});
        threads.emplace_back([start, end, &array]() {
            std::sort(array.begin() + start, array.begin() + end);
        });
    }
    
    for (auto& t : threads) t.join();
    
    while (ranges.size() > 1) {
        std::vector<Range> next_ranges;
        std::vector<std::thread> merge_threads;
        
        for (size_t i = 0; i + 1 < ranges.size(); i += 2) {
            Range r1 = ranges[i];
            Range r2 = ranges[i+1];
            merge_threads.emplace_back([r1, r2, &array]() {
                std::inplace_merge(array.begin() + r1.start, 
                                   array.begin() + r1.end, 
                                   array.begin() + r2.end);
            });
            next_ranges.push_back({r1.start, r2.end});
        }
        
        if (ranges.size() % 2 != 0) {
            next_ranges.push_back(ranges.back());
        }
        
        for (auto& t : merge_threads) t.join();
        ranges = next_ranges;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    auto array = read_array();
    
    parallel_sort(array);

    size_t k;
    if (std::cin >> k) {
        for (size_t i = k - 1; i < array.size(); i += k) {
            std::cout << array[i] << ' ';
        }
        std::cout << "\n";
    }

    return 0;
}
