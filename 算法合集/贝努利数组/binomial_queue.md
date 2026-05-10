# 二项队列（Binomial Queue / 二项堆）

## 一、概述

**二项队列**（Binomial Queue），又称**二项堆**（Binomial Heap），是一种支持高效合并操作的优先级队列。它由若干棵**二项树**（Binomial Tree）组成的森林构成，每棵二项树都满足堆序性质（最小堆或最大堆）。

主要操作的时间复杂度：

| 操作 | 时间复杂度 |
|------|-----------|
| `insert`（插入） | 摊还 O(1)，最坏 O(log n) |
| `findMin`（查最值） | O(log n)（可优化到 O(1)） |
| `extractMin`（删除最值） | O(log n) |
| `merge`（合并两个堆） | O(log n) |
| `decreaseKey`（减小键） | O(log n) |
| `delete`（删除任意节点） | O(log n) |

它最大的优势是 **merge 操作可在 O(log n) 内完成**，远优于普通二叉堆的 O(n)。

## 二、二项树 B_k

二项树 `B_k` 的递归定义：

- `B_0` 是单个节点
- `B_k` 由两棵 `B_{k-1}` 合并而成：把其中一棵的根作为另一棵根的最左孩子

性质：

1. `B_k` 共有 `2^k` 个节点
2. 树高为 `k`
3. 深度 `i` 的节点个数为 `C(k, i)`（这正是"二项"名字的来源）
4. 根节点的度数为 `k`，是树中度数最大的节点

```
B_0:  •

B_1:  •
      |
      •

B_2:    •
       / \
      •   •
          |
          •

B_3:      •
        / | \
       •  •  •
       |  |
       •  •
       |
       •
```

## 三、二项堆的结构

一个含 `n` 个元素的二项堆由若干棵不同阶的二项树组成。设 `n` 的二进制表示为：

$$
n = \sum_{i} b_i \cdot 2^i, \quad b_i \in \{0, 1\}
$$

则堆中包含 `B_i` 当且仅当 `b_i = 1`。因此一个 `n` 节点的二项堆**至多包含 `⌊log₂ n⌋ + 1` 棵二项树**。

每棵二项树满足**最小堆性质**：父节点的键 ≤ 子节点的键。

## 四、核心操作

### 1. 合并（merge）

合并是其它操作的基础。类比二进制加法：将两个堆按二项树的阶从小到大扫描，遇到相同阶的两棵树就合并为更高一阶的树（处理"进位"）。

合并两棵相同阶 `B_k` 的方法：键值较小的根成为新根，另一棵作为其新孩子，得到 `B_{k+1}`，时间 O(1)。

整体合并时间：O(log n)。

### 2. 插入（insert）

将单个元素视为 `B_0`，与现有堆合并即可。摊还复杂度 O(1)。

### 3. 查找最小值（findMin）

最小值一定在某棵二项树的根上，扫描所有根：O(log n)。可用一个变量缓存指向最小根的指针，做到 O(1)。

### 4. 删除最小值（extractMin）

1. 找到最小根所在的二项树 `B_k`
2. 移除该根，其孩子构成一个新堆（阶为 `0..k-1` 的二项树）
3. 把新堆与原堆剩下部分合并

总复杂度 O(log n)。

### 5. 减小键（decreaseKey）

类似二叉堆的"上浮"操作，沿父指针上浮直至满足堆序，O(log n)。

## 五、伪代码（Merge 主体）

```text
function MERGE(H1, H2):
    H ← 按阶从小到大归并 H1 与 H2 的根链表
    prev ← null;  cur ← H.head;  next ← cur.sibling
    while next ≠ null:
        if cur.degree ≠ next.degree
           or (next.sibling ≠ null and next.sibling.degree = cur.degree):
            prev ← cur
            cur  ← next
        else if cur.key ≤ next.key:
            cur.sibling ← next.sibling
            LINK(next, cur)             # next 成为 cur 的孩子
        else:
            if prev = null: H.head ← next
            else: prev.sibling ← next
            LINK(cur, next)             # cur 成为 next 的孩子
            cur ← next
        next ← cur.sibling
    return H
```

## 六、C++ 简要实现骨架

```cpp
struct Node {
    int key, degree;
    Node *parent, *child, *sibling;
};

Node* link(Node* y, Node* z) {     // y 成为 z 的孩子
    y->parent  = z;
    y->sibling = z->child;
    z->child   = y;
    z->degree++;
    return z;
}

Node* mergeRoots(Node* h1, Node* h2);   // 按 degree 升序归并根链表
Node* unionHeap(Node* h1, Node* h2);    // 主合并算法（同上伪代码）
void  insert(Node*& h, int key);
int   findMin(Node* h);
Node* extractMin(Node*& h);
```

## 七、与其它优先级队列的比较

| 数据结构 | insert | extractMin | merge | decreaseKey |
|---------|--------|-----------|-------|-------------|
| 二叉堆        | O(log n)        | O(log n) | O(n)         | O(log n) |
| **二项堆**    | **O(1) 摊还**   | O(log n) | **O(log n)** | O(log n) |
| 斐波那契堆    | O(1) 摊还       | O(log n) 摊还 | O(1) 摊还 | O(1) 摊还 |
| 配对堆        | O(1)            | O(log n) 摊还 | O(1)    | o(log n) 摊还 |

二项堆是**斐波那契堆**的前身和理论基础，理解它有助于理解后者的"懒惰合并"思想。

## 八、应用

- 需要频繁合并的优先级队列场景（如并行算法、事件模拟）
- 图算法中（Dijkstra、Prim 算法的早期实现）
- 教学：是从二叉堆过渡到斐波那契堆的桥梁

## 九、参考文献

1. Vuillemin, J. (1978). *A data structure for manipulating priority queues*. Communications of the ACM, 21(4), 309–315.
2. Cormen, Leiserson, Rivest, Stein. *Introduction to Algorithms (CLRS)*, 第 19 章 "Binomial Heaps"。
3. Mark Allen Weiss. *Data Structures and Algorithm Analysis*.
