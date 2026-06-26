#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

template <typename T> class BloomFilter {
public:
  BloomFilter(size_t expected_items, double false_positive_rate) {
    //   m = ceil( -(n * ln p) / (ln 2)^2 )      // total bits
    //   k = round( (m / n) * ln 2 )             // number of hash functions
    m_bits =
        ceil(-1 * (expected_items * log(false_positive_rate)) / pow(log(2), 2));
    k_hashes = round(((double)m_bits / expected_items) * log(2));
    bits = std::vector<bool>(m_bits);
    n_inserted = 0;
  }

  void add(const T &item) {
    auto [h1, h2] = base_hashes(item);
    for (int i = 0; i < k_hashes; i++) {
      bits[nth_index(h1, h2, i)] = true;
    }
    n_inserted++;
  }

  // Query membership.
  //   returns false -> item is DEFINITELY not present.
  //   returns true  -> item is PROBABLY present (may be a false positive).
  bool contains(const T &item) const {
    auto [h1, h2] = base_hashes(item);
    for (int i = 0; i < k_hashes; i++) {
      if (!bits[nth_index(h1, h2, i)])
        return false;
    }
    return true;
  }

  size_t bit_count() const { return m_bits; }

  size_t hash_count() const { return k_hashes; }

  // Number of add() calls made (counts duplicates; not distinct items).
  size_t inserted() const { return n_inserted; }

  double fill_ratio() const {
    int count = std::count(bits.begin(), bits.end(), true);
    return (double)count / m_bits;
  }

  double estimated_fpp() const { return pow(fill_ratio(), k_hashes); }

private:
  std::pair<uint64_t, uint64_t> base_hashes(const T &item) const {
    size_t seed = std::hash<T>{}(item);
    uint64_t h1 = seed;
    uint64_t h2 = seed ^ 0xdeadbeefbad; // haha funny
    return {h1, h2};
  }

  size_t nth_index(uint64_t h1, uint64_t h2, size_t i) const {
    return (h1 + i * h2) % m_bits;
  }

  size_t m_bits;          // size of bit array (m)
  size_t k_hashes;        // number of hash functions (k)
  size_t n_inserted;      // how many add() calls have happened
  std::vector<bool> bits; // the bit array, all false at construction
};
