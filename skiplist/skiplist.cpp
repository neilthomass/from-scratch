// Design Skiplist — Hard
// add/erase/search in O(log n) expected without a balanced tree.

#include <algorithm>
#include <vector>
#include <cstdlib>

struct Node {
  int key;
  std::vector<Node*> next;
  Node(int key, int level) : key(key), next(level, nullptr) {}
};


// The entire upside of a skip lst is that we can use log(n) insert, search erase all while being concurent friendly.

template <typename T>
class Skiplist {
public:
    Skiplist(int maxLevel = 8) : maxLevel(maxLevel){
      head = new Node(-1, maxLevel);
    }
    
    void add(int key) {
      std::vector<Node*> update(maxLevel, head);
      Node* cur = head;

      for (int i = maxLevel - 1; i >= 0; i--) {
        while (cur->next[i] && cur->next[i]->key < key) {
          cur = cur->next[i];
        }
        update[i] = cur;
      }
      int lvl = randomLevel();
      Node* node = new Node(key, lvl);
      for (int i = 0; i < lvl; i++) {
        node->next[i] = update[i]->next[i];
        update[i]->next[i] = node;
      }
    }
      
    bool search(int target) {

      Node* cur = head;
      for (int i = maxLevel - 1; i >= 0; i-- ) {
        while ( cur->next[i] && cur->next[i]->key < target)
          cur = cur->next[i];


      }
      return cur->next[0] && cur->next[0]->key == target;
      
    }
    bool erase(int num) {
      std::vector<Node*> update(maxLevel, head);
      Node* cur = head;

      for (int i = maxLevel - 1; i >= 0; i--) {
        while (cur->next[i] && cur->next[i]->key < num) {
          cur = cur->next[i];
        } 
        update[i] = cur;
      }

      Node* target = cur->next[0];
      if (!target || target->key != num) return false;

      for (int i = 0; i < maxLevel; i++) {
        if (update[i]-> next[i] != target) break;
        update[i]->next[i] = target->next[i];
        
      }
      delete target;
      return true;
      
    }

private:
  Node* head;
  int maxLevel;

  int randomLevel() {
    
    int lvl = 1;
    while (rand() % 2 && lvl < maxLevel) lvl++;
    return lvl;
  }
  
};
