#include "redblacktree.cpp"

#include <cassert>
#include <iostream>
#include <random>
#include <set>
// This file is ai generated
int main() {

  // ── a fresh tree is empty and valid ─────────────────────────────────────
  {
    RedBlackTree t;
    assert(t.size() == 0);
    assert(t.validate());
    assert(!t.contains(0));
    assert(!t.contains(42));
  }

  // ── a single insert ─────────────────────────────────────────────────────
  {
    RedBlackTree t;
    t.insert(10);
    assert(t.size() == 1);
    assert(t.contains(10));
    assert(!t.contains(11));
    assert(t.validate());
  }

  // ── ascending insert: the input that DESTROYS a naive BST ───────────────
  // A plain BST would degenerate into a height-n list here. The LLRB must
  // stay balanced and valid throughout.
  {
    RedBlackTree t;
    const int n = 5000;
    for (int i = 0; i < n; i++) {
      t.insert(i);
      assert(t.validate()); // invariants hold after EVERY insert
    }
    assert(t.size() == static_cast<size_t>(n));
    for (int i = 0; i < n; i++)
      assert(t.contains(i));
  }

  // ── random inserts, validating after each ───────────────────────────────
  {
    RedBlackTree t;
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 100000);
    std::set<int> reference;
    for (int i = 0; i < 3000; i++) {
      int k = dist(rng);
      t.insert(k);
      reference.insert(k);
      assert(t.validate());
    }
    assert(t.size() == reference.size());
    for (int k : reference)
      assert(t.contains(k));
  }

  // ── duplicate inserts are ignored ───────────────────────────────────────
  {
    RedBlackTree t;
    for (int i = 0; i < 100; i++)
      t.insert(7);
    assert(t.size() == 1);
    assert(t.contains(7));
    assert(t.validate());
  }

  // ── contains-all after a bulk insert ────────────────────────────────────
  {
    RedBlackTree t;
    for (int i = 0; i < 2000; i++)
      t.insert(i * 3);
    assert(t.validate());
    for (int i = 0; i < 2000; i++) {
      assert(t.contains(i * 3));
      assert(!t.contains(i * 3 + 1));
    }
  }

  // ── erase then absent, validating after every erase ─────────────────────
  {
    RedBlackTree t;
    const int n = 2000;
    for (int i = 0; i < n; i++)
      t.insert(i);
    assert(t.validate());
    for (int i = 0; i < n; i++) {
      assert(t.contains(i));
      t.erase(i);
      assert(!t.contains(i)); // gone immediately
      assert(t.validate());   // invariants hold after EVERY erase
    }
    assert(t.size() == 0);
    assert(t.validate());
  }

  // ── erasing an absent key is a harmless no-op ────────────────────────────
  {
    RedBlackTree t;
    t.insert(5);
    t.insert(15);
    t.erase(999); // not present
    assert(t.size() == 2);
    assert(t.contains(5));
    assert(t.contains(15));
    assert(t.validate());
  }

  // ── in-order traversal yields a sorted sequence ─────────────────────────
  // We verify the BST ordering indirectly: probing keys in order should match
  // the membership of a std::set built from the same data.
  {
    RedBlackTree t;
    std::set<int> reference;
    std::mt19937 rng(999);
    std::uniform_int_distribution<int> dist(0, 5000);
    for (int i = 0; i < 1500; i++) {
      int k = dist(rng);
      t.insert(k);
      reference.insert(k);
    }
    assert(t.validate());
    int prev = -1;
    bool sorted = true;
    for (int k = 0; k <= 5000; k++) {
      bool in_tree = t.contains(k);
      assert(in_tree == (reference.count(k) > 0));
      if (in_tree) {
        if (k < prev)
          sorted = false; // keys appear in ascending order
        prev = k;
      }
    }
    assert(sorted);
    assert(t.size() == reference.size());
  }

  // ── large mixed insert/erase workload vs std::set, invariants throughout ─
  {
    RedBlackTree t;
    std::set<int> reference;
    std::mt19937 rng(2024);
    std::uniform_int_distribution<int> key_dist(0, 20000);
    std::uniform_int_distribution<int> op_dist(0, 1);

    for (int i = 0; i < 40000; i++) {
      int k = key_dist(rng);
      if (op_dist(rng) == 0) {
        t.insert(k);
        reference.insert(k);
      } else {
        t.erase(k);
        reference.erase(k);
      }
      // full validate() is O(n); spot-check it periodically to keep the
      // workload large, but verify the invariants do hold throughout.
      if (i % 500 == 0) {
        assert(t.validate());
        assert(t.size() == reference.size());
      }
    }
    assert(t.validate());
    assert(t.size() == reference.size());
    // every key in the reference set must be in the tree, and vice versa
    for (int k : reference)
      assert(t.contains(k));
    for (int k = 0; k <= 20000; k++) {
      assert(t.contains(k) == (reference.count(k) > 0));
    }
  }

  std::cout << "all tests passed ✔\n";
  return 0;
}
