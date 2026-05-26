# C++ Lambda 表达式详解

## 一、什么是 Lambda

**Lambda 表达式**就是"一个可以现场定义、立即使用的匿名函数"。C++11 引入。

用途：当你需要一个**短小的、只用一次的函数**（尤其传给 `sort`、`for_each`、`find_if` 等算法时），不必专门写命名函数。

## 二、完整语法

```
[捕获列表](参数列表) mutable 异常说明 -> 返回类型 { 函数体 }
```

最常用的简化形式：

```
[捕获列表](参数列表) { 函数体 }
```

返回类型通常**自动推导**，不用写。

## 三、五个组成部分

### 1. 捕获列表 `[ ]` —— Lambda 独有的部分

控制 lambda 能不能用**外部变量**，以及怎么用。

| 写法 | 含义 |
|------|------|
| `[]` | 不捕获任何外部变量 |
| `[x]` | 按**值**捕获 x（拷贝一份进来） |
| `[&x]` | 按**引用**捕获 x（改了外面也变） |
| `[=]` | 所有外部变量按值捕获 |
| `[&]` | 所有外部变量按引用捕获 |
| `[=, &x]` | 默认按值，x 单独按引用 |
| `[&, x]` | 默认按引用，x 单独按值 |
| `[this]` | 类成员函数里捕获当前对象 |

**例子：**

```cpp
int threshold = 60;

// 按值捕获 threshold
auto isPass = [threshold](int score) {
    return score >= threshold;
};

cout << isPass(80);  // 1
cout << isPass(50);  // 0
```

如果改成 `[&threshold]`，threshold 在外面变了，lambda 内部看到的也跟着变。

### 2. 参数列表 `( )` —— 和普通函数一样

```cpp
auto add = [](int a, int b) { return a + b; };
cout << add(3, 5);   // 8
```

可以有默认参数、引用、const 等：

```cpp
auto greet = [](const string& name = "world") {
    cout << "hello, " << name;
};
```

### 3. 返回类型 `-> 类型` —— 通常省略

编译器能从 `return` 推断：

```cpp
auto square = [](int x) { return x * x; };             // 推断为 int
auto half   = [](int x) -> double { return x / 2.0; }; // 显式指定
```

只有当函数体有多个 return 且类型不一致，或编译器推断不出时，才需要显式写。

### 4. 函数体 `{ }`

和普通函数没区别。

### 5. `mutable` —— 修改按值捕获的变量

按值捕获的变量默认是 **const**，想在 lambda 里改它，得加 `mutable`：

```cpp
int x = 10;
auto f = [x]() mutable {
    x += 1;          // 没有 mutable 会编译错误
    cout << x;
};
f();   // 11（lambda 内部的副本）
cout << x;  // 10（外面的 x 没变，因为按值捕获）
```

## 四、Lambda 的本质：一个"重载了 operator()"的对象

编译器会把这段：

```cpp
auto f = [](int a, int b) { return a + b; };
```

大致编译成：

```cpp
struct __Lambda_xxx {
    int operator()(int a, int b) const { return a + b; }
};
__Lambda_xxx f;
```

所以 lambda 是个**对象**（叫"闭包对象"），不是函数指针。但**无捕获**的 lambda 可以隐式转成普通函数指针：

```cpp
int (*fp)(int, int) = [](int a, int b) { return a + b; };  // OK
```

有捕获的 lambda 不行，因为它带状态。

## 五、常见用法场景

### 场景 1：作为 `sort` 的比较器

```cpp
sort(v.begin(), v.end(), [](const Student& x, const Student& y) {
    return x.s > y.s;
});
```

### 场景 2：`for_each` 遍历

```cpp
vector<int> v = {1, 2, 3};
for_each(v.begin(), v.end(), [](int x) { cout << x << ' '; });
```

### 场景 3：`find_if` 找符合条件的元素

```cpp
auto it = find_if(v.begin(), v.end(), [](int x) { return x > 10; });
```

### 场景 4：`count_if` 计数

```cpp
int evens = count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
```

### 场景 5：捕获外部变量做"参数化"

```cpp
int k;
cin >> k;
int cnt = count_if(v.begin(), v.end(), [k](int x) { return x > k; });
```

### 场景 6：递归 lambda（需要 `std::function` 或 auto + 自传引用）

```cpp
function<int(int)> fact = [&](int n) {
    return n <= 1 ? 1 : n * fact(n - 1);
};
cout << fact(5);  // 120
```

## 六、按值 vs 按引用捕获的坑

```cpp
int x = 10;

auto f1 = [x]() { cout << x; };   // 拷贝当前的 x = 10
auto f2 = [&x]() { cout << x; };  // 引用，用时再看 x

x = 999;
f1();   // 输出 10（拍下了快照）
f2();   // 输出 999（实时）
```

**坑**：按引用捕获的变量，如果 lambda 的寿命超过原变量（比如返回 lambda 出函数），会**悬空引用**：

```cpp
auto makeCounter() {
    int cnt = 0;
    return [&cnt]() { return ++cnt; };  // ❌ cnt 出函数就销毁了
}
```

应该按值并加 `mutable`：

```cpp
auto makeCounter() {
    int cnt = 0;
    return [cnt]() mutable { return ++cnt; };  // ✅
}
```

## 七、实战例子：多关键字排序

```cpp
sort(v.begin(), v.end(), [](const Student& x, const Student& y) {
    if (x.s != y.s) return x.s > y.s;   // 综合分大的在前
    if (x.b != y.b) return x.b > y.b;   // 实验分大的在前
    return x.id < y.id;                 // 学号小的在前
});
```

逐项对照：

| 部分 | 内容 | 含义 |
|------|------|------|
| `[]` | 空捕获 | 不需要用外部变量 |
| `(const Student& x, const Student& y)` | 两个参数 | 要比较的两个学生，引用避免拷贝 |
| 无 `->` | 返回类型推断 | 编译器知道是 `bool` |
| `{ ... }` | 函数体 | 三级比较规则 |

**核心约定**：比较函数返回 `true` ⟺ 第一个参数应排在第二个前面。

## 八、什么时候**不**该用 lambda

- 逻辑很长、超过 10~20 行 → 写成命名函数更清晰
- 多处复用同一个比较逻辑 → 写成命名函数，避免重复
- 需要递归 → 普通函数更自然

## 九、一句话总结

> **Lambda = 捕获列表 + 参数列表 + 函数体**，本质是一个带状态的可调用对象，用来在需要"传函数"的地方现场写一个匿名函数，让代码更紧凑。

记住三件事：
1. `[]` 是 lambda 独有的，用来从外部抓变量。
2. 比较函数返回 `true` 意味着"左边在前"。
3. 短小、一次性的逻辑用 lambda；复杂、复用的逻辑用命名函数。
