A from-scratch C++ disjoint set (union-find) tracking which elements share a group, with near-O(1) amortized union and find.

Path compression flattens lookup chains and union by rank keeps trees shallow, together giving the famous inverse-Ackermann amortized bound.
