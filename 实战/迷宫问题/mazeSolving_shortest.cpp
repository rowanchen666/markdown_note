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
    cout << "输入迷宫尺寸（行，列）";
    cin >> row >> col;
    wall = row * col; //迷宫矩阵中对应于墙的数值

    maze = new int* [row];
    for (i = 0 ; i < row ; i++) {
        maze[i] = new int [col];
        cout << "输入第" << i << "列(0表示通道," << wall << "表示墙。用空格分开)";
        for (j = 0 ; j < col ; j ++) {
            cin >> maze[i][j];
        }
    }

    //寻找路径
    //1.将起点入栈
    cur.x = 0;
    cur.y = 0;
    st.push(cur);

    //2.反复寻找路径
    while (!st.empty()) {
        last = st.front();
        st.pop();

        //按照上右下左的顺序搜索相邻通道，到达出口时立即退出循环
        if (last.x != 0 && maze[last.x - 1][last.y] == 0) { //上单元是通道
            cur.x = last.x - 1;
            cur.y = last.y;
            if (cur.x != 0 || cur.y != 0) { //排除入口
                maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
                st.push(cur);
            }
        }

        if (last.x == row - 1 && last.y == col - 2) break; //右单元是出口
        if (maze[last.x][last.y + 1] == 0) {
            cur.x = last.x;
            cur.y = last.y + 1;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }

        if(maze[last.x + 1][last.y] == 0) { //下单元是通道
            cur.x = last.x + 1;
            cur.y = last.y;
            maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
            st.push(cur);
        }

        if (last.y != 0 && maze[last.x][last.y -1] == 0) { //左单元是通道
            cur.x = last.x;
            cur.y = last.y - 1;
            if (cur.x != 0 || cur.y != 0) { //左单元不是入口
                maze[cur.x][cur.y] = maze[last.x][last.y] + 1;
                st.push(cur);
            }
        }
    }

    maze[row - 1][col - 1] = maze[last.x][last.y] + 1; //设置出口的路径长度

    //获取路径
    int length = maze[row - 1][col - 1] + 1;
    node* path = new node[length];
    cur.x = row - 1;
    cur.y = col - 1;
    while (cur.x != 0 || cur.y != 0) {
        path[maze[cur.x][cur.y]] = cur;
        if (cur.x != 0 && maze[cur.x - 1][cur.y] < maze[cur.x][cur.y]) {
            cur.x -= 1;
            continue;
        }
        if (cur.x != row - 1 && maze[cur.x + 1][cur.y] < maze[cur.x][cur.y]) {
            cur.x += 1;
            continue;
        }
        if (cur.y != 0 && maze[cur.x][cur.y - 1] < maze[cur.x][cur.y]) {
            cur.y -= 1;
            continue;
        }
        if (cur.y != col - 1 && maze[cur.x][cur.y + 1] < maze[cur.x][cur.y]) {
            cur.y += 1;
            continue;
        }
    }
    path[0] = cur;

    //打印路径
    cout << "路径为：";
    for (i = 0 ; i < length - 1 ; i ++) cout << "(" << path[i].x << "," << path[i].y << ") ->";
    cout << "(" << path[length - 1].x << "," << path[length - 1].y << ")" << endl;

    return 0;
}