# 1. if-else匹配问题

因为else可以省略，所以在嵌套的if语句中省略它的else部分会导致歧义。例如：
```c++
if (n >= 0) {
	for (i = 0 ; i < n ; i ++)
		if (s[i] > 0) {
			cout << "true";
		}
}
else 
	cout << "error";
```

在这个例子中，虽然程序的缩进结构明确了设计意图，但编译器看不懂，它会将else与内层的if配对，应该为如下：
```c++
if (n >= 0) {
	for (i = 0 ; i < n ; i ++) { //用大括号标明作用域
		if (s[i] > 0) {
			cout << "true";
		}
	}
}
else 
	cout << "error";
```