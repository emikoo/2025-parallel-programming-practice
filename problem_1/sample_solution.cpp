#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <cmath>

std::vector<std::vector<double>> read_matrix() {
    size_t rows, cols;
    if (!(std::cin >> rows >> cols)) return {};

    size_t a, b, x, y, z, p;
    std::cin >> a >> b >> x >> y >> z >> p;
    
    std::vector<std::vector<size_t>> intermediate(rows, std::vector<size_t>(cols));
    
    for(size_t i=0; i<rows; ++i) {
        for(size_t j=0; j<cols; ++j) {
            intermediate[i][j] = b % p;
        }
    }
    intermediate[0][0] = a % p;
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            size_t val = intermediate[i][j];
            if (i > 0 && j > 0) {
                val = (val + intermediate[i - 1][j - 1] * x) % p;
            }
            if (i > 0) {
                val = (val + intermediate[i - 1][j] * y) % p;
            }
            if (j > 0) {
                val = (val + intermediate[i][j - 1] * z) % p;
            }
            intermediate[i][j] = val;
        }
    }
    
    size_t max_value = 0;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (intermediate[i][j] > max_value) max_value = intermediate[i][j];
        }
    }

    std::vector<std::vector<double>> result(rows, std::vector<double>(cols));
    
    size_t total_elements = rows * cols;
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    
    std::vector<std::thread> threads;
    size_t chunk_size = total_elements / num_threads;
    
    auto normalize_worker = [&](size_t start, size_t end) {
        for (size_t k = start; k < end; ++k) {
            size_t r = k / cols;
            size_t c = k % cols;
            result[r][c] = static_cast<double>(intermediate[r][c]) / static_cast<double>(max_value);
        }
    };

    if (total_elements < 10000) {
        normalize_worker(0, total_elements);
    } else {
        for (unsigned int i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? total_elements : start + chunk_size;
            threads.emplace_back(normalize_worker, start, end);
        }
        for (auto& t : threads) t.join();
    }
    
    return result;
}

void multiply_worker(const std::vector<std::vector<double>>& left, 
                     const std::vector<std::vector<double>>& right_T,
                     std::vector<std::vector<double>>& result,
                     size_t start_idx, size_t end_idx) {
    size_t right_cols = result[0].size();
    size_t K = left[0].size();
    
    for (size_t idx = start_idx; idx < end_idx; ++idx) {
        size_t i = idx / right_cols;
        size_t j = idx % right_cols;
        
        double sum = 0;
        const auto& right_col_j = right_T[j];
        const auto& left_row_i = left[i];
        
        for (size_t k = 0; k < K; ++k) {
            sum += left_row_i[k] * right_col_j[k];
        }
        result[i][j] = sum;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    auto left = read_matrix();
    auto right = read_matrix();
    
    if (left.empty() || right.empty()) return 0;
    
    auto left_rows = left.size();
    auto left_cols = left[0].size();
    auto right_rows = right.size();
    auto right_cols = right[0].size();

    if (left_cols != right_rows) {
        std::cerr << "Wrong matrices";
        return 1;
    }

    std::vector<std::vector<double>> right_T(right_cols, std::vector<double>(right_rows));
    
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    
    {
        std::vector<std::thread> threads;
        size_t total_elements = right_rows * right_cols;
        size_t chunk_size = total_elements / num_threads;
        
        auto transpose_worker = [&](size_t start, size_t end) {
            for (size_t k = start; k < end; ++k) {
                size_t r = k / right_cols;
                size_t c = k % right_cols;
                right_T[c][r] = right[r][c];
            }
        };
        
        if (total_elements < 10000) {
            transpose_worker(0, total_elements);
        } else {
            for (unsigned int i = 0; i < num_threads; ++i) {
                size_t start = i * chunk_size;
                size_t end = (i == num_threads - 1) ? total_elements : start + chunk_size;
                threads.emplace_back(transpose_worker, start, end);
            }
            for (auto& t : threads) t.join();
        }
    }

    std::vector<std::vector<double>> result(left_rows, std::vector<double>(right_cols));
    
    {
        std::vector<std::thread> threads;
        size_t total_tasks = left_rows * right_cols;
        size_t chunk_size = total_tasks / num_threads;
        
        for (unsigned int i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? total_tasks : start + chunk_size;
            threads.emplace_back(multiply_worker, std::cref(left), std::cref(right_T), std::ref(result), start, end);
        }
        for (auto& t : threads) t.join();
    }

    std::cout << left_rows << ' ' << right_cols << "\n";
    std::cout << std::fixed << std::setprecision(12);
    for (int i = 0; i < left_rows; ++i) {
        for (int j = 0; j < right_cols; ++j) {
            std::cout << result[i][j] << (j == right_cols - 1 ? "" : " ");
        }
        std::cout << "\n";
    }

    return 0;
}
