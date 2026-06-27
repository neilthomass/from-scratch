#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numbers>
#include <vector>


class CountMinSketch {
public:
    CountMinSketch(double epsilon, double delta): w(ceil(std::numbers::e/epsilon)), d(ceil(log(1/delta))) {
      grid = std::vector(d, std::vector<long long>(w, 0));
    }

    CountMinSketch(size_t width, size_t depth): w(width), d(depth) {
      grid = std::vector(d, std::vector<long long>(w, 0));
    }

    void add(uint64_t item, long long count = 1) {
      for (size_t row = 0; row < d; row++) {
        grid[row][hash(item, row)] += count;
      }
      n_total += count;
    }

    long long estimate(uint64_t item) const {
      long long est = std::numeric_limits<long long>::max();
      for (size_t row = 0; row < d; row++) {
        est = std::min(est, grid[row][hash(item, row)]);
      }
      return est;
    }


    size_t width() const { return w; }

    size_t depth() const { return d; }

    long long total() const { return n_total; }

private:
  
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    size_t hash(uint64_t item, size_t row) const {
        uint64_t h1 = splitmix64(item);
        uint64_t h2 = splitmix64(item ^ 0x9e3779b97f4a7c15ULL) | 1ULL;
        return static_cast<size_t>((h1 + row * h2) % w);
    }

    size_t w;
    size_t d;
    long long n_total = 0;
    std::vector<std::vector<long long>> grid;
};