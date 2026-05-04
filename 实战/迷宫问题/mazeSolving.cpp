#include <iostream>
#include <stack>
using namespace std;

struct node{
    int x, y; //x表示行号，y表示列号
    int dir; //表示下一次搜索的方向：0->上，1->右，2->下，3->左
};

int main(){
    int ** maze; //表示迷宫的矩阵
    int row , col; //row表示行数，col表示列数
    int i , j;
    stack<node> st; //保存路径
    node cur , last; //cur表示当前探索的通道，last表示cur前一个通道
    
    //输入迷宫矩阵
    cout << "输入迷宫尺寸（行，列）";
    cin >> row >> col;

    maze = new int* [row];
    for (i = 0 ; i < row ; i++) {
        maze[i] = new int [col];
        cout << "输入第" << i << "列（用空格分开）：";
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
    while(!st.empty()) {
        last = st.top();
        st.pop();
        if(last.dir == 4) continue; //表示该结点所有方向均已搜索

        switch(last.dir) {
            case 0:
                if (last.x != 0 && maze[last.x - 1][last.y] == 0) {
                    cur.x = last.x - 1;
                    cur.y = last.y;
                    last.dir = 1;
                    break;
                }
            case 1:
                if(last.y != col - 1 && maze[last.x][last.y + 1] == 0) {
                    cur.x = last.x;
                    cur.y = last.y + 1;
                    last.dir = 2;
                    break;
                }
            case 2:
                if(last.x != row - 1 && maze[last.x + 1][last.y] == 0) {
                    cur.x = last.x + 1;
                    cur.y = last.y;
                    last.dir = 3;
                    break;
                }
            case 3:
                if (last.y != 0 && maze[last.x][last.y - 1] == 0) {
                    cur.x = last.x;
                    cur.y = last.y + 1;
                    last.dir = 4;
                    break;
                }
            default : continue;
        }

        st.push(last); //last重新进栈
        st.push(cur); //新的通道进栈
        maze[cur.x][cur.y] = 2; //标记新的通道

        if (cur.x == row - 1 && cur.y == col - 1) {
            cout << "逆向路径为：" ;
            while (!st.empty()) {
                cur = st.top();
                st.pop();
                cout << "(" << cur.x << "," << cur.y << ")";
            }
            break;
        }
    }

    return 0;
}