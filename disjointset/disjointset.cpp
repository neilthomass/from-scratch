// Design Disjoint Set (Union-Find) — Medium
// Union-find with path compression + union by rank/size. ~O(1) amortized per op.

#include <ios>
#include <vector>

using namespace std;

class DSU {
public:
    DSU(int n) {
      for (int i = 0; i < n; i++) {
        parent.push_back(i);
      }
      rank_ = vector<int>(n,0);
      count = n;
    }

    int find(int x) {
      if (parent[x] == x) return x;
      return parent[x] = find(parent[x]);
    }
    
    bool unite(int a, int b) {
      int parent_a = find(a); 
      int parent_b = find(b);

      if (parent_a == parent_b) return false;

      if (rank_[parent_a] < rank_[parent_b]) {
        parent[parent_a] = parent_b;
      } else if (rank_[parent_a] > rank_[parent_b]) {
        parent[parent_b] = parent_a;
      } else {
        parent[parent_b] = parent_a;
        rank_[parent_a]++;
      }


      
      count--;
      return true;
       
    }
    
    bool connected(int a, int b) {
      if (find(parent[a]) == find(parent[b])) return true;
      return false;
    }
    int components() {
      return count;
    }

private:
    vector<int> parent, rank_;
    int count;
};
