#include <string>

using namespace std;

class Trie {
public:
    Trie() = default;


    void insert(string word) {
      Node* curr = root;
      for (char c: word) {
        int idx = c - 'a';
        if (!curr->ch[idx]) {
          curr->ch[idx] = new Node();
        }
        curr = curr->ch[idx];
      }
      curr->end = true;
    }
    
    bool search(string word) {
      Node* curr = root;
      for (char c: word) {
        Node* next = curr->ch[c - 'a'];
        if (!next) return false;
        curr = next;
      }
      return curr->end;
    }
    bool startsWith(string prefix) {
      Node* curr = root;
      for (char c: prefix) {
        Node* next = curr->ch[c - 'a'];
        if (!next) return false;
        curr = next;
      }
      return true;
    }
    
private:
    struct Node { Node* ch[26] = {}; bool end = false; };
    Node* root = new Node();
};
