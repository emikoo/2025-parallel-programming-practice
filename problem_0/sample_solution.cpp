#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>

std::istream& operator>>(std::istream& in, __int128& value) {
    std::string s;
    in >> s;
    value = 0;
    bool negative = false;
    size_t i = 0;
    if (s[0] == '-') {
        negative = true;
        i = 1;
    }
    for (; i < s.size(); ++i) {
        value = value * 10 + (s[i] - '0');
    }
    if (negative) value = -value;
    return in;
}

std::ostream& operator<<(std::ostream& out, __int128 value) {
    if (value == 0) {
        out << '0';
        return out;
    }
    std::string s;
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    while (value > 0) {
        s += '0' + static_cast<int>(value % 10);
        value /= 10;
    }
    if (negative) s += '-';
    std::reverse(s.begin(), s.end());
    out << s;
    return out;
}

std::atomic<uint64_t> search_limit;
std::mutex result_mutex;
__int128 found_factor;
bool has_factor;

void find_factor_worker(__int128 n, uint64_t start, uint64_t end) {
    for (uint64_t p = start; p < end; ++p) {
        if (p >= search_limit.load(std::memory_order_relaxed)) {
            return;
        }

        if (n % p == 0) {
            std::lock_guard<std::mutex> lock(result_mutex);
            if (!has_factor || p < found_factor) {
                found_factor = p;
                has_factor = true;
                uint64_t current_limit = search_limit.load();
                if (p < current_limit) {
                    search_limit.store(p);
                }
            }
            return;
        }
    }
}

int main() {
    __int128 n;
    std::cin >> n;
    if (n <= 1) {
        return 0;
    }

    std::vector<__int128> factors;

    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    __int128 p = 3;
    const uint64_t SEQUENTIAL_LIMIT = 10000;
    
    while (p * p <= n && p < SEQUENTIAL_LIMIT) {
        while (n % p == 0) {
            factors.push_back(p);
            n /= p;
        }
        p += 2;
    }

    while (p * p <= n) {
        uint64_t range_start = (uint64_t)p;
        uint64_t range_end;
        
        if (n < 18446744073709551615ULL) {
            range_end = (uint64_t)sqrt((double)n) + 2;
        } else {
             range_end = (uint64_t)sqrt((double)n) + 2;
        }

        has_factor = false;
        search_limit.store(range_end + 1);
        
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        
        std::vector<std::thread> threads;
        uint64_t range_len = range_end - range_start;
        
        if (range_len < 10000) {
             for (uint64_t i = range_start; i <= range_end; i += 2) {
                 if (i * i > n) break; 
                 if (n % i == 0) {
                     has_factor = true;
                     found_factor = i;
                     break;
                 }
             }
        } else {
            uint64_t chunk_size = range_len / num_threads;
            if (chunk_size % 2 != 0) chunk_size++; 

            for (unsigned int i = 0; i < num_threads; ++i) {
                uint64_t t_start = range_start + i * chunk_size;
                uint64_t t_end = (i == num_threads - 1) ? range_end + 1 : t_start + chunk_size;
                
                if (t_start % 2 == 0) t_start++;
                
                threads.emplace_back(find_factor_worker, n, t_start, t_end);
            }
            
            for (auto& t : threads) {
                t.join();
            }
        }

        if (has_factor) {
            factors.push_back(found_factor);
            n /= found_factor;
            p = found_factor;
        } else {
            break;
        }
    }

    if (n > 1) {
        factors.push_back(n);
    }

    for (const auto& factor : factors) {
        std::cout << factor << ' ';
    }
    std::cout << '\n';

    return 0;
}
