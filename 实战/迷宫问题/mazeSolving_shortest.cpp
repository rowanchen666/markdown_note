#include <iostream>
#include <queue>
using namespace std;

struct node{
    int x, y; //x表示行号，y表示列号
};

int main(){
    queue<node> st; //保存已搜索到的通道
    node cur , last;
    int ** maze;
    int row , col;
    int wall;
    int i , j;

    //输入迷宫矩阵
    cout << "输入迷宫尺寸（行，列）: ";
    cin >> row >> col;
    wall = row * col; //迷宫矩阵中对应于墙的数值

    maze = new int* [row];
    for (i = 0 ; i < row ; i++) {
        maze[i] = new int [col];
        cout << "输入第" << i << "行(0表示通道," << wall << "表示墙。用空格分开): ";
        for (j = 0 ; j < col ; j ++) {
            cin >> maze[i][j];
        }
    }

    //寻找路径
    //1.将起点入队列
    cur.x = 0;
    cur.y = 0;
    maze[0][0] = 1; // 标记起点距离为1
    st.push(cur);

    bool found = false;

    //2.反复寻找路径
    while (!st.empty() && !found) {
        last = st.front();
        st.pop();

        // 检查是否到达出口
        if (last.x == row - 1 && last.y == col - 1) {
            found = true;
            break;
        }

        //按照上右下左的顺序搜索相邻通道
        // 上
        if (last.x > 0 && maze[last.x - 1][last.y] == 0) {
            cur.x = last.x - 1;
            cur.y = last.y;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }

        // 右
        if (last.y < col - 1 && maze[last.x][last.y + 1] == 0) {
            cur.x = last.x;
            cur.y = last.y + 1;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }

        // 下
        if (last.x < row - 1 && maze[last.x + 1][last.y] == 0) {
            cur.x = last.x + 1;
            cur.y = last.y;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }

        // 左
        if (last.y > 0 && maze[last.x][last.y - 1] == 0) {
            cur.x = last.x;
            cur.y = last.y - 1;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }
    }

    if (!found) {
        cout << "没有找到从起点到终点的路径！" << endl;
        // 释放内存
        for (i = 0; i < row; i++) {
            delete[] maze[i];
        }
        delete[] maze;
        return 0;
    }

    //获取路径
    int length = maze[row - 1][col - 1];
    node* path = new node[length];
    cur.x = row - 1;
    cur.y = col - 1;
    int pathIndex = length - 1;

    while (pathIndex >= 0) {
        path[pathIndex] = cur;
        pathIndex--;

        if (cur.x == 0 && cur.y == 0) break; // 到达起点

        // 寻找上一个位置（距离值小1的相邻位置）
        int currentDistance = maze[cur.x][cur.y];
        bool moved = false;

        // 检查上方
        if (cur.x > 0 && maze[cur.x - 1][cur.y] == currentDistance - 1) {
            cur.x -= 1;
            moved = true;
        }
        // 检查左方
        else if (cur.y > 0 && maze[cur.x][cur.y - 1] == currentDistance - 1) {
            cur.y -= 1;
            moved = true;
        }
        // 检查下方
        else if (cur.x < row - 1 && maze[cur.x + 1][cur.y] == currentDistance - 1) {
            cur.x += 1;
            moved = true;
        }
        // 检查右方
        else if (cur.y < col - 1 && maze[cur.x][cur.y + 1] == currentDistance - 1) {
            cur.y += 1;
            moved = true;
        }

        if (!moved) {
            cout << "路径回溯错误！" << endl;
            break;
        }
    }

    //打印路径
    cout << "最短路径长度: " << length << endl;
    cout << "路径为: ";
    for (i = 0 ; i < length - 1 ; i ++) {
        cout << "(" << path[i].x << "," << path[i].y << ") -> ";
    }
    cout << "(" << path[length - 1].x << "," << path[length - 1].y << ")" << endl;

    // 释放内存
    for (i = 0; i < row; i++) {
        delete[] maze[i];
    }
    delete[] maze;
    delete[] path;

    return 0;
}