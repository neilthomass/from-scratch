#include "countminsketch.cpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <unordered_map>
// This file is ai generated and used for verification

// =============================================================================
//  TEST SUITE — implement the methods in countminsketch.cpp until every
//  assertion passes.
//  Build:  g++ -std=c++23 -O2 countminsketch/main.cpp -o countminsketch && ./countminsketch
// =============================================================================

int main() {
    std::cout << "running count-min sketch tests...\n";

    // ── dimensions derived from (epsilon, delta) are sensible ────────────────
    {
        // w = ceil(e/epsilon), d = ceil(ln(1/delta))
        CountMinSketch cms(0.01, 0.01);
        assert(cms.width() > 0);
        assert(cms.depth() > 0);
        // e/0.01 ~= 271.8 -> w = 272 ; ln(1/0.01) ~= 4.6 -> d = 5
        assert(cms.width() == 272);
        assert(cms.depth() == 5);

        // smaller epsilon -> larger width (tighter additive error)
        CountMinSketch tighter(0.001, 0.01);
        assert(tighter.width() > cms.width());
        assert(tighter.depth() == cms.depth());  // delta unchanged

        // smaller delta -> larger depth (higher confidence)
        CountMinSketch surer(0.01, 0.0001);
        assert(surer.depth() > cms.depth());
        assert(surer.width() == cms.width());    // epsilon unchanged
    }

    // ── an empty sketch estimates everything at 0 ────────────────────────────
    {
        CountMinSketch cms(size_t{500}, size_t{5});
        assert(cms.total() == 0);
        for (uint64_t i = 0; i < 1000; i++) assert(cms.estimate(i) == 0);
    }

    // ── single add, then estimate is at least the count added ────────────────
    {
        CountMinSketch cms(size_t{500}, size_t{5});
        cms.add(42, 3);
        assert(cms.estimate(42) >= 3);   // one-sided: never below true count
        assert(cms.total() == 3);
        cms.add(42);                     // default count of 1
        assert(cms.estimate(42) >= 4);
        assert(cms.total() == 4);
    }

    // ── ONE-SIDED GUARANTEE: estimate(x) >= true_count(x) for ALL x ──────────
    // Build a populated stream, count exactly with a map oracle, and verify the
    // sketch NEVER underestimates. This must hold 100% of the time.
    {
        CountMinSketch cms(0.01, 0.01);
        std::unordered_map<uint64_t, long long> oracle;

        std::mt19937_64 rng(12345);
        std::uniform_int_distribution<uint64_t> key(0, 5000);
        std::uniform_int_distribution<int> amt(1, 9);
        for (int i = 0; i < 50000; i++) {
            uint64_t x = key(rng);
            long long c = amt(rng);
            cms.add(x, c);
            oracle[x] += c;
        }

        long long oracle_sum = 0;
        for (const auto& [x, c] : oracle) {
            assert(cms.estimate(x) >= c);   // must NEVER underestimate
            oracle_sum += c;
        }
        assert(cms.total() == oracle_sum);  // total mass is tracked exactly
    }

    // ── error stays within epsilon * total with high probability ─────────────
    // Over a random stream, the over-count for each item should obey
    // estimate(x) - true(x) <= epsilon * N for the vast majority of items.
    {
        const double epsilon = 0.001;
        const double delta = 0.01;
        CountMinSketch cms(epsilon, delta);
        std::unordered_map<uint64_t, long long> oracle;

        std::mt19937_64 rng(999);
        std::uniform_int_distribution<uint64_t> key(0, 20000);
        for (int i = 0; i < 200000; i++) {
            uint64_t x = key(rng);
            cms.add(x, 1);
            oracle[x] += 1;
        }

        const long long N = cms.total();
        const double bound = epsilon * static_cast<double>(N);
        long long max_err = 0;
        int violations = 0;
        for (const auto& [x, c] : oracle) {
            long long err = cms.estimate(x) - c;
            assert(err >= 0);               // one-sided again
            if (err > max_err) max_err = err;
            if (static_cast<double>(err) > bound) violations++;
        }
        double viol_rate = static_cast<double>(violations) / oracle.size();
        std::cout << "  N = " << N
                  << ", epsilon*N bound = " << bound
                  << ", max observed error = " << max_err
                  << ", bound violations = " << viol_rate << "\n";
        // the (epsilon, delta) promise: violations stay below delta (with slack)
        assert(viol_rate < delta);
    }

    // ── heavy hitters are estimated accurately ───────────────────────────────
    // A few items with huge true counts should have estimates very close to
    // their true value, since the epsilon*N noise floor is tiny next to them.
    {
        const double epsilon = 0.001;
        CountMinSketch cms(epsilon, 0.01);
        std::unordered_map<uint64_t, long long> oracle;

        // background noise of many light items
        std::mt19937_64 rng(7);
        std::uniform_int_distribution<uint64_t> key(1000, 50000);
        for (int i = 0; i < 100000; i++) {
            uint64_t x = key(rng);
            cms.add(x, 1);
            oracle[x] += 1;
        }
        // a handful of genuine heavy hitters
        const uint64_t heavy[] = {1, 2, 3, 4, 5};
        for (uint64_t h : heavy) {
            cms.add(h, 20000);
            oracle[h] += 20000;
        }

        const double bound = epsilon * static_cast<double>(cms.total());
        long long max_heavy_err = 0;
        for (uint64_t h : heavy) {
            long long est = cms.estimate(h);
            long long err = est - oracle[h];
            assert(err >= 0);
            assert(static_cast<double>(err) <= bound);   // within the guarantee
            if (err > max_heavy_err) max_heavy_err = err;
            // relative error on a heavy hitter should be tiny
            double rel = static_cast<double>(err) / oracle[h];
            assert(rel < 0.01);
        }
        std::cout << "  max heavy-hitter error = " << max_heavy_err
                  << " (out of true count 20000+)\n";
    }

    // ── two independent sketches don't share state ───────────────────────────
    {
        CountMinSketch a(size_t{500}, size_t{5});
        CountMinSketch b(size_t{500}, size_t{5});
        a.add(1, 10);
        assert(a.estimate(1) >= 10);
        assert(b.estimate(1) == 0);   // b never saw it
        assert(b.total() == 0);
    }

    std::cout << "all tests passed ✔\n";
    return 0;
}
