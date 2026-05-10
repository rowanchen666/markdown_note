// binomial.cpp
// 二项堆 (Binomial Heap) 完整实现 —— 整数键 / 最小堆
// 节点采用"左孩子-右兄弟"链式表示法

#include <iostream>
#include <climits>
#include <stdexcept>
using namespace std;

// ============================================================
// 节点结构
// ------------------------------------------------------------
// 用 struct 默认 public, 作为内部数据结构无需封装。
// 三个指针:
//   parent  : 指向父节点 (根的 parent 为 nullptr, 这是判断"是否为根"的依据)
//   child   : 指向"最左孩子"
//   sibling : 双重身份 ——
//             ① 节点在根链表上时, 指向下一棵树的根
//             ② 节点是某个父亲的孩子时, 指向自己的右兄弟
// ============================================================
struct Node {
    int   key;
    int   degree;       // 该节点为根的子树阶数 (= 它的孩子个数)
    Node *parent, *child, *sibling;

    Node(int k)
        : key(k), degree(0),
          parent(nullptr), child(nullptr), sibling(nullptr) {}
};

// ============================================================
// 二项堆类
// ============================================================
class Binomial {
private:
    Node* head = nullptr;   // 根链表头, 按 degree 升序排列

    // --------------------------------------------------------
    // link: 把以 y 为根的 B_k 挂成以 z 为根的 B_k 的最左孩子
    // 前置条件: y->degree == z->degree, 且 y 已从根链表摘下
    // 调用方负责"决定谁当父亲"——本函数无脑挂载
    // --------------------------------------------------------
    static void link(Node* y, Node* z) {
        if (y->degree != z->degree) return;   // 防御性检查

        // 头插法把 y 加入 z 的孩子链表前端
        y->sibling = z->child;   // 必须先保存 z 原 child 给 y 当兄弟
        z->child   = y;          // 再让 z 指向 y
        y->parent  = z;          // y 认 z 当爹
        z->degree++;             // z 的孩子数 +1
    }

    // --------------------------------------------------------
    // mergeRootLists: 把两条按 degree 升序的根链表归并成一条
    // 不处理同阶冲突, 只负责"排好队"
    // 经典两个有序链表合并 (LeetCode 21)
    // --------------------------------------------------------
    static Node* mergeRootLists(Node* a, Node* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;

        Node  dummy(0);              // 哨兵节点 (栈上)
        Node* tail = &dummy;         // tail 指向已合并部分的最后一个节点

        while (a && b) {
            if (a->degree <= b->degree) {
                tail->sibling = a;
                a = a->sibling;
            } else {
                tail->sibling = b;
                b = b->sibling;
            }
            tail = tail->sibling;    // 关键: tail 必须前进
        }
        tail->sibling = (a ? a : b); // 接上剩余的尾巴

        return dummy.sibling;        // 真正的头节点
    }

    // --------------------------------------------------------
    // unionHeaps: 合并两个二项堆 (核心算法)
    // 步骤: ① 调用 mergeRootLists 排好队
    //       ② 在归并链表上扫一遍, 消除同阶冲突 (二进制加法)
    // 维护三指针 prev / cur / next, 4 种 case 处理
    // --------------------------------------------------------
    static Node* unionHeaps(Node* h1, Node* h2) {
        Node* head = mergeRootLists(h1, h2);
        if (!head) return nullptr;

        Node* prev = nullptr;
        Node* cur  = head;
        Node* next = cur->sibling;

        while (next) {
            // 判断当前位置的两种"不合并"条件
            bool diff   = (cur->degree != next->degree);
            bool triple = (next->sibling && next->sibling->degree == cur->degree);

            if (diff || triple) {
                // case 1: 阶不同 → 整体后移
                // case 2: 三连同阶 → 让后两个先合并, 当前轮跳过
                prev = cur;
                cur  = next;
            } else if (cur->key <= next->key) {
                // case 3: 二连同阶, cur.key 较小 → cur 当父亲, 吸收 next
                cur->sibling = next->sibling;   // 先把 next 从根链表摘下
                link(next, cur);
                // cur 不动, 因为合并后的 cur (degree+1) 可能还要和后面比较
            } else {
                // case 4: 二连同阶, next.key 较小 → next 当父亲, 吸收 cur
                if (prev) prev->sibling = next;  // 修复前驱指针
                else      head          = next;  // cur 是头时, 更新 head
                link(cur, next);
                cur = next;                       // cur 上移到 next
            }
            next = cur->sibling;
        }
        return head;
    }

    // --------------------------------------------------------
    // destroy: 递归释放整片节点
    // 顺序: 先孩子, 再兄弟, 最后自己 (DFS 释放, 避免悬空指针)
    // --------------------------------------------------------
    static void destroy(Node* x) {
        if (x == nullptr) return;
        destroy(x->child);
        destroy(x->sibling);
        delete x;
    }

public:
    Binomial() = default;

    // 析构: 释放所有节点, 防止内存泄漏
    ~Binomial() {
        destroy(head);
        head = nullptr;
    }

    // 禁拷贝: 否则两个对象共享 head, 析构时 double-free
    Binomial(const Binomial&) = delete;
    Binomial& operator=(const Binomial&) = delete;

    // --------------------------------------------------------
    // insert: 插入一个 key, 返回新节点指针
    // 思路: 把新节点视为只含 B_0 的小堆, 与当前堆 union
    // 返回的指针用于以后的 decreaseKey / deleteNode
    // --------------------------------------------------------
    Node* insert(int key) {
        Node* node = new Node(key);
        head = unionHeaps(head, node);
        return node;        // 注意: 返回的是新节点, 不是 head
    }

    // --------------------------------------------------------
    // merge: 合并另一个堆 (合并后 other 变空)
    // --------------------------------------------------------
    void merge(Binomial& other) {
        head = unionHeaps(head, other.head);
        other.head = nullptr;    // 关键: 防止 other 析构时重复释放节点
    }

    // --------------------------------------------------------
    // findMin: 扫根链表取最小 key
    // 注意 head 不一定是最小, 因为 head 按 degree 排, 与 key 无关
    // --------------------------------------------------------
    int findMin() const {
        if (!head) throw runtime_error("heap is empty");

        int minKey = head->key;
        for (Node* x = head->sibling; x; x = x->sibling)
            if (x->key < minKey) minKey = x->key;
        return minKey;
    }

    // --------------------------------------------------------
    // extractMin: 删除并返回最小值
    // 步骤: ① 一遍扫描同时找最小根 minNode 和它的前驱 prevMin
    //       ② 把 minNode 从根链表摘除
    //       ③ 反转 minNode 的孩子链表 (孩子原本 degree 降序, 根链表要升序)
    //       ④ 与剩余堆 union
    //       ⑤ delete minNode 并返回 key
    // --------------------------------------------------------
    int extractMin() {
        if (!head) throw runtime_error("heap is empty");

        // ① 找最小根及其前驱
        Node* minNode = head;
        Node* prevMin = nullptr;
        Node* prev    = nullptr;
        for (Node* x = head; x; x = x->sibling) {
            if (x->key < minNode->key) {
                minNode = x;
                prevMin = prev;
            }
            prev = x;
        }

        // ② 摘除 minNode (注意 head 情况)
        if (prevMin) prevMin->sibling = minNode->sibling;
        else         head             = minNode->sibling;

        // ③ 反转孩子链表, 顺手清 parent (它们要升级为根了)
        Node* prevC = nullptr;
        Node* c     = minNode->child;
        while (c) {
            Node* nxt  = c->sibling;
            c->sibling = prevC;
            c->parent  = nullptr;
            prevC      = c;
            c          = nxt;
        }
        // 此时 prevC 是反转后的新头 (degree 升序)

        // ④ 把孩子小堆与剩余堆合并
        head = unionHeaps(head, prevC);

        // ⑤ 删除节点, 返回 key
        int ret = minNode->key;
        delete minNode;
        return ret;
    }

    // --------------------------------------------------------
    // decreaseKey: 把节点 x 的 key 减小到 newKey
    // 通过"沿 parent 上浮"维护堆序, 交换 key 而非节点
    // --------------------------------------------------------
    void decreaseKey(Node* x, int newKey) {
        if (newKey > x->key) throw runtime_error("new key is larger");
        x->key = newKey;

        // 上浮: 注意循环里要用 m/n 推进, 不能反复用 x
        Node* m = x;
        Node* n = x->parent;
        while (n && m->key < n->key) {
            // 交换 key
            int temp = m->key;
            m->key   = n->key;
            n->key   = temp;
            // 上移一层
            m = n;
            n = n->parent;
        }
    }

    // --------------------------------------------------------
    // deleteNode: 删除任意节点
    // 技巧: 先把 x 的 key 减到 INT_MIN, 它会一路上浮成为某棵树的根,
    //       然后 extractMin 必定弹出它
    // --------------------------------------------------------
    void deleteNode(Node* x) {
        decreaseKey(x, INT_MIN);
        extractMin();
    }
};

// ============================================================
// 简单测试
// ============================================================
int main() {
    Binomial h;
    int data[] = {10, 1, 6, 12, 25, 8, 14, 29, 11, 17, 38, 27};
    for (int v : data) h.insert(v);

    cout << "findMin = " << h.findMin() << "\n";   // 期望: 1

    cout << "排序输出: ";
    for (int i = 0; i < 12; ++i) cout << h.extractMin() << " ";
    cout << "\n";
    // 期望: 1 6 8 10 11 12 14 17 25 27 29 38
    return 0;
}
