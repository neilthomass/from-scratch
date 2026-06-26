#include <cassert>
#include <cstddef>
enum Color : bool { RED, BLACK };

struct RBNode {
  int key;
  Color color;
  RBNode *left, *right;
};

class RedBlackTree {
public:
  void insert(int key) {
    root = insert(root, key);
    root->color = BLACK;
  }

  bool contains(int key) const {
    RBNode *curr = root;
    while (curr && curr->key != key) {
      if (curr->key < key)
        curr = curr->right;
      else
        curr = curr->left;
    }
    return curr != nullptr;
  }

  void erase(int key) {
    if (!contains(key))
      return; // absent key: harmless no-op
    if (!is_red(root->left) && !is_red(root->right))
      root->color = RED;
    root = erase(root, key);
    if (root)
      root->color = BLACK;
  }

  size_t size() const { return count; }

  bool validate() const {
    if (root && root->color != BLACK)
      return false;
    if (black_height(root) == -1)
      return false;
    size_t n = 0;
    if (!check_bst(root, nullptr, nullptr, n))
      return false;
    return n == count;
  }

private:
  static bool is_red(RBNode *x) { return x != nullptr && x->color == RED; }

  RBNode *rotate_left(RBNode *h) {
    // This rotates left with h as pivot (h moves down)
    RBNode *x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = h->color;
    h->color = RED;
    return x;
  }

  RBNode *rotate_right(RBNode *h) {
    RBNode *x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = h->color;
    h->color = RED;
    return x;
  }

  void flip_colors(RBNode *h) {
    h->color = (h->color == RED) ? BLACK : RED;
    h->left->color = (h->left->color == RED) ? BLACK : RED;
    h->right->color = (h->right->color == RED) ? BLACK : RED;
  }

  RBNode *fixup(RBNode *h) {
    if (is_red(h->right) && !is_red(h->left))
      h = rotate_left(h);
    if (is_red(h->left) && is_red(h->left->left))
      h = rotate_right(h);
    if (is_red(h->left) && is_red(h->right))
      flip_colors(h);
    return h;
  }

  RBNode *insert(RBNode *h, int key) {
    if (!h) {
      count++;
      return new RBNode{key, RED, nullptr, nullptr};
    }
    if (key < h->key)
      h->left = insert(h->left, key);
    else if (key > h->key)
      h->right = insert(h->right, key);
    return fixup(h);
  }

  RBNode *move_red_left(RBNode *h) {
    flip_colors(h);
    if (is_red(h->right->left)) {
      h->right = rotate_right(h->right);
      h = rotate_left(h);
      flip_colors(h);
    }
    return h;
  }

  RBNode *move_red_right(RBNode *h) {
    flip_colors(h);
    if (is_red(h->left->left)) {
      h = rotate_right(h);
      flip_colors(h);
    }
    return h;
  }

  RBNode *minimum(RBNode *h) const {
    while (h->left)
      h = h->left;
    return h;
  }

  RBNode *erase_min(RBNode *h);

  RBNode *erase(RBNode *h, int key);

  int black_height(RBNode *x) const {
    if (!x)
      return 0;
    int lh = black_height(x->left);
    int rh = black_height(x->right);
    if (lh == -1 || rh == -1 || lh != rh)
      return -1;
    return lh + (is_red(x) ? 0 : 1);
  }

  bool check_bst(RBNode *h, const int *lo, const int *hi, size_t &n) const;

  RBNode *root = nullptr; // root of the tree (nullptr when empty)
  size_t count = 0;       // number of keys stored
};

RBNode *RedBlackTree::erase_min(RBNode *h) {
  if (!h->left) {
    delete h;
    count--;
    return nullptr;
  }
  if (!is_red(h->left) && !is_red(h->left->left))
    h = move_red_left(h);
  h->left = erase_min(h->left);
  return fixup(h);
}

RBNode *RedBlackTree::erase(RBNode *h, int key) {
  if (key < h->key) {
    if (!is_red(h->left) && !is_red(h->left->left))
      h = move_red_left(h);
    h->left = erase(h->left, key);
  } else {
    if (is_red(h->left))
      h = rotate_right(h);
    if (key == h->key && !h->right) {
      delete h;
      count--;
      return nullptr;
    }
    if (!is_red(h->right) && !is_red(h->right->left))
      h = move_red_right(h);
    if (key == h->key) {
      RBNode *m = minimum(h->right);
      h->key = m->key;
      h->right = erase_min(h->right);
    } else {
      h->right = erase(h->right, key);
    }
  }
  return fixup(h);
}

bool RedBlackTree::check_bst(RBNode *h, const int *lo, const int *hi,
                            size_t &n) const {
  if (!h)
    return true;
  if (lo && h->key <= *lo)
    return false;
  if (hi && h->key >= *hi)
    return false;
  n++;
  return check_bst(h->left, lo, &h->key, n) &&
         check_bst(h->right, &h->key, hi, n);
}