#include "bloomfilter.cpp"
#include <cassert>
#include <iostream>
#include <string>
// This file is ai generated and used for verification
int main() {
  std::cout << "running bloom filter tests...\n";

  // ── sizing math: m and k should be derived from (n, p) ──────────────────
  {
    // n = 1000, p = 0.01 -> m ~= 9586 bits, k ~= 7
    BloomFilter<int> bf(1000, 0.01);
    assert(bf.bit_count() > 0);
    assert(bf.hash_count() > 0);
    // for 1% FP the textbook answer is k = 7
    assert(bf.hash_count() == 7);
    // ~9.6 bits/item: allow a little slack for rounding
    assert(bf.bit_count() >= 9000 && bf.bit_count() <= 10500);
    // a stricter target needs more bits and more hashes
    BloomFilter<int> stricter(1000, 0.001);
    assert(stricter.bit_count() > bf.bit_count());
    assert(stricter.hash_count() > bf.hash_count());
  }

  // ── a fresh filter contains nothing ─────────────────────────────────────
  {
    BloomFilter<int> bf(100, 0.01);
    assert(bf.inserted() == 0);
    assert(bf.fill_ratio() == 0.0);
    for (int i = 0; i < 1000; i++)
      assert(!bf.contains(i));
  }

  // ── basic add / contains ────────────────────────────────────────────────
  {
    BloomFilter<int> bf(100, 0.01);
    bf.add(42);
    assert(bf.contains(42));
    assert(bf.inserted() == 1);
    bf.add(7);
    bf.add(99);
    assert(bf.contains(7));
    assert(bf.contains(99));
    assert(bf.contains(42));
  }

  // ── NO FALSE NEGATIVES: everything inserted must report present ──────────
  // This is the core guarantee and must hold 100% of the time.
  {
    BloomFilter<int> bf(10000, 0.01);
    for (int i = 0; i < 10000; i++)
      bf.add(i);
    for (int i = 0; i < 10000; i++) {
      assert(bf.contains(i)); // must NEVER fail
    }
    assert(bf.inserted() == 10000);
  }

  // ── adding a duplicate sets no new bits (idempotent) ────────────────────
  {
    BloomFilter<int> bf(100, 0.01);
    bf.add(123);
    double after_first = bf.fill_ratio();
    bf.add(123);
    bf.add(123);
    assert(bf.fill_ratio() == after_first); // bits already set
    assert(bf.contains(123));
  }

  // ── works with non-integer types (strings) ──────────────────────────────
  {
    BloomFilter<std::string> bf(100, 0.01);
    bf.add("apple");
    bf.add("banana");
    bf.add("cherry");
    assert(bf.contains("apple"));
    assert(bf.contains("banana"));
    assert(bf.contains("cherry"));
    assert(!bf.contains("durian") || true); // may FP, but usually absent
  }

  // ── false-positive RATE stays near the target ───────────────────────────
  // Insert n items, then query a large disjoint set that was never added and
  // count how many wrongly report present. Should be roughly p (here 1%).
  {
    const int n = 10000;
    const double p = 0.01;
    BloomFilter<int> bf(n, p);
    for (int i = 0; i < n; i++)
      bf.add(i);

    int false_positives = 0;
    const int trials = 100000;
    for (int i = n; i < n + trials; i++) { // disjoint from inserted keys
      if (bf.contains(i))
        false_positives++;
    }
    double observed = static_cast<double>(false_positives) / trials;
    std::cout << "  observed FP rate: " << observed << " (target " << p
              << ")\n";
    // generous bound: should be well under 2x the target
    assert(observed < 2.0 * p);
  }

  // ── fill ratio and estimated fpp move in the right direction ────────────
  {
    BloomFilter<int> bf(1000, 0.01);
    double prev_fill = bf.fill_ratio();
    assert(prev_fill == 0.0);
    for (int batch = 0; batch < 5; batch++) {
      for (int i = batch * 100; i < (batch + 1) * 100; i++)
        bf.add(i);
      double fill = bf.fill_ratio();
      assert(fill >= prev_fill); // monotonically non-decreasing
      assert(fill >= 0.0 && fill <= 1.0);
      prev_fill = fill;
    }
    double fpp = bf.estimated_fpp();
    assert(fpp >= 0.0 && fpp <= 1.0);
  }

  // ── two independent filters don't share state ───────────────────────────
  {
    BloomFilter<int> a(100, 0.01);
    BloomFilter<int> b(100, 0.01);
    a.add(1);
    assert(a.contains(1));
    assert(!b.contains(1)); // b never saw it
  }

  std::cout << "all tests passed ✔\n";
  return 0;
}
