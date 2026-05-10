#include <iostream>
#include <climits>
using namespace std;

struct Node {
        int key; 
        int degree;
        Node* parent , * child , * sibling;
        Node (int k) : key(k) , degree(0) , parent(nullptr)  , child (nullptr) , sibling(nullptr) {}
};

class Binomial {
    private:
        Node* head = nullptr;

        static void link (Node* y , Node* z) {
            if (y -> degree != z -> degree) return;

            y -> sibling = z -> child;
            z -> child = y;
            y -> parent = z;
            z -> degree ++;
        }

        static Node* mergeRootLists(Node* a, Node* b) {
            if (a == nullptr && b == nullptr) return nullptr;
            if (a == nullptr) return b;
            if (b == nullptr) return a;

            Node dummy(0);
            Node* tail = &dummy;

            while (a && b) {
                if (a->degree <= b->degree) {
                    tail->sibling = a;
                    a = a->sibling;
                } else {
                    tail->sibling = b;
                    b = b->sibling;
                }
                tail = tail->sibling;
            }            
            tail->sibling = (a ? a : b);

            return dummy.sibling;
        }

        static Node* unionHeaps(Node* h1, Node* h2) {
            Node* head = mergeRootLists(h1, h2);
            if (!head) return nullptr;

            Node* prev = nullptr;
            Node* cur  = head;
            Node* next = cur->sibling;

            while (next) {
                bool diff   = (cur->degree != next->degree);
                bool triple = (next->sibling && next->sibling->degree == cur->degree);

                if (diff || triple) {
                    // case 1 or 2: 不合并, 整体后移
                    prev = cur;
                    cur  = next;
                } else if (cur->key <= next->key) {
                    // case 3: cur 吃掉 next
                    cur->sibling = next->sibling;
                    link(next, cur);
                    // cur 保持不变, 因为合并后的 cur 可能还要和后面的 cur.sibling 比较
                } else {
                    // case 4: next 吃掉 cur
                    if (prev) prev->sibling = next;
                    else head = next;
                    link(cur, next);
                    cur = next;
                }
                next = cur->sibling;
            }
            return head;
        }

        static void destroy(Node* x) {
            if(x == nullptr) return;
            destroy(x -> child);
            destroy(x -> sibling);
            delete x;
        }

    public:
        Node* insert(int key) {
            Node* node = new Node(key);
            head = unionHeaps(head , node);
            return node;
        }

        void merge(Binomial& other) {
            head = unionHeaps(head , other.head);
            other.head = nullptr;
        }

        int findMin() const {
            if (!head) throw runtime_error("heap is empty");

            int minKey = head -> key;
            for (Node* temp = head ; temp != nullptr ; temp = temp -> sibling) minKey = (minKey < temp -> key) ? minKey : temp -> key;

            return minKey;
        }

        int extractMin(){
            if (!head) throw runtime_error("heap is empty");

            // 1) 一遍扫描: 同时找到 minNode 和 prevMin
            Node* minNode = head;
            Node* prevMin = nullptr;
            Node* prev    = nullptr;
            for (Node* x = head; x; x = x->sibling) {
                if (x -> key < minNode -> key) {
                    minNode = x;
                    prevMin = prev;
                }
                prev = x;
            }

            // 2) 把 minNode 从根链表摘除
            if (prevMin) prevMin -> sibling = minNode -> sibling;
            else head = minNode -> sibling;

            // 3) 反转 minNode 的孩子链表, 顺手清 parent
            Node* prevC = nullptr;
            Node* c = minNode->child;
            while (c) {
            Node* nxt  = c -> sibling;
                c -> sibling = prevC;
                c -> parent  = nullptr;
                prevC = c;
                c = nxt;
            }

            // 4) 与剩余堆合并
            head = unionHeaps(head, prevC);

            // 5) 删除 minNode, 返回 key
            int ret = minNode -> key;
            delete minNode;
            return ret;
        }

        void decreaseKey(Node* x, int newKey) {
            if (newKey > x->key) throw runtime_error("new key is larger");
            x -> key = newKey;

            Node* m = x , *n = x -> parent;
            while (n && m -> key < n -> key) {
                int temp = m -> key;
                m -> key = n -> key;
                n -> key = temp;
                m = n;
                n = n -> parent;
            }
        }

        void deleteNode(Node* x) {
            decreaseKey(x , INT_MIN);
            extractMin();
        }

        Binomial() = default;
        ~Binomial() {
            destroy(head);
            head = nullptr;
        }

        Binomial(const Binomial&) = delete;
        Binomial& operator=(const Binomial&) = delete;
};