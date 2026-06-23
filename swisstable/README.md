A from-scratch C++ implementation of a [Swiss table](https://abseil.io/about/design/swisstables) - the hash table design behind Google's Abseil, Rust's `HashMap`, and Go's built-in map.

Traditional hash tables like `std::unordered_map` use chaining, each bucket holds a pointer to a linked list of heap-allocated nodes. Every lookup chases these pointers across RAM, paying a cache miss penalty (~100ns vs ~1ns for L1) at each step, and then loads the full key (sometimes a very long string) for comparison only to rule it out as a non-match.

Swiss table uses some clever tricks to optimize the tradional hash table.

A few things I found particularly interesting about them: no pointer chasing, higher load factor (~75% -> ~87.5 more memory efficient), SIMD scanning allows for 16x less probes, hash splitting, tombstone deletion.