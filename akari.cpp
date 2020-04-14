#include "windows.h"
#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> solveAkari(vector<vector<int>> &g);
bool solveAkari1(vector<vector<int>> &g, vector<std::pair<unsigned, unsigned>> &dian);                     //处理特殊情况
bool solveAkari2(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc); //处理黑格子
inline bool checkNext(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian);          //预检查
bool solveAkari3(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc); //处理剩余的白格子
bool create(vector<vector<int>> &gg, unsigned x, unsigned y);                                              //放灯
int check(vector<vector<int>> &g, unsigned x, unsigned y);                                                 //检查黑格子
int check2(vector<vector<int>> &g, unsigned x, unsigned y);                                                //检查白格子
bool check3(vector<vector<int>> &g, unsigned x, unsigned y);                                               //检查白格子

DWORD WINAPI threadFunc(LPVOID p);

void printp(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc);
void printP(vector<vector<int>> &g, unsigned x, unsigned y);

static clock_t start1, end1, end2;

struct chuanCan
{
    vector<vector<int>> g;
    vector<std::pair<unsigned, unsigned>> dian;
    unsigned loc;
    unsigned flag;
};

const int BNUM = 10, HNUM = 0; //最大线程数
static int bnum, hnum;         //当前线程数
static bool zhaodao;           //如果找到解，所有其他线程直接返回
static int true_value;         //用于多线程标识是否找到解

int main()
{
    unsigned N, M;
    cin >> N >> M;

    vector<vector<int>> g(N);
    for (unsigned i = 0; i < g.size(); i++)
    {
        g[i].resize(M);
    }

    for (unsigned i = 0; i < N; i++)
        for (unsigned j = 0; j < M; j++)
        {
            cin >> g[i][j];
        }

    start1 = clock();

    //for(int i=0;i<100000;i++){
    //vector<vector<int>> gg(g);
    g = solveAkari(g);
    //}

    return 0;
}

vector<vector<int>> solveAkari(vector<vector<int>> &g)
{
    vector<std::pair<unsigned, unsigned>> dian;

    solveAkari1(g, dian);

    solveAkari2(g, dian, 0);

    g[0][0] = true_value;

    double time1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
    double time2 = (double)(end2 - end1) / CLOCKS_PER_SEC;
    cout << "bai:" << time1 * 1000 << "ms" << endl;
    cout << "hei:" << time2 * 1000 << "ms" << endl;
    printP(g, 0, 0);

    for (unsigned i = 0; i < g.size(); i++)
    {
        for (unsigned j = 0; j < g[0].size(); j++)
        {
            if (g[i][j] >= 6)
                g[i][j] = -2;
        }
    }

    return g;
}

bool solveAkari1(vector<vector<int>> &g, vector<std::pair<unsigned, unsigned>> &dian)
{
    unsigned z, n;
    //这里处理特殊情况
    for (unsigned i = 0; i < g.size(); i++)
        for (unsigned j = 0; j < g[0].size(); j++)
        {
            if (g[i][j] > 4 || g[i][j] < 1)
            {
                if (g[i][j] == 0)
                    dian.push_back(std::make_pair(i, j));
                ;
                continue;
            }

            if (g[i][j] == 4) //显然上下左右都要放灯
            {
                create(g, i, j - 1);
                create(g, i, j + 1);
                create(g, i - 1, j);
                create(g, i + 1, j);
                continue;
            }

            z = n = 0;
            if (i != 0 && g[i - 1][j] == -2)
            {
                z |= 1;
                n++;
            }
            if (i != 0 && g[i - 1][j] == 5)
                n++;
            if (j != 0 && g[i][j - 1] == -2)
            {
                z |= 2;
                n++;
            }
            if (j != 0 && g[i][j - 1] == 5)
                n++;
            if (i != g.size() - 1 && g[i + 1][j] == -2)
            {
                z |= 4;
                n++;
            }
            if (i != g.size() - 1 && g[i + 1][j] == 5)
                n++;
            if (j != g[0].size() - 1 && g[i][j + 1] == -2)
            {
                z |= 8;
                n++;
            }
            if (j != g[0].size() - 1 && g[i][j + 1] == 5)
                n++;

            if (g[i][j] == 3 && n == 3)
            {
                if (z & 1)
                    create(g, i - 1, j);
                if (z & 2)
                    create(g, i, j - 1);
                if (z & 4)
                    create(g, i + 1, j);
                if (z & 8)
                    create(g, i, j + 1);
                continue;
            }

            if (g[i][j] == 2 && n == 2)
            {
                if (z & 1)
                    create(g, i - 1, j);
                if (z & 2)
                    create(g, i, j - 1);
                if (z & 4)
                    create(g, i + 1, j);
                if (z & 8)
                    create(g, i, j + 1);
                continue;
            }

            if (g[i][j] == 1 && n == 1)
            {
                //create(g,i-z&1+z&4,j-z&2+z&8);
                if (z & 1)
                    create(g, i - 1, j);
                if (z & 2)
                    create(g, i, j - 1);
                if (z & 4)
                    create(g, i + 1, j);
                if (z & 8)
                    create(g, i, j + 1);
                continue;
            }

            dian.push_back(std::make_pair(i, j));
        }
    return true;
}

bool solveAkari2(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc)
{
    //cout<<"id2:"<<GetCurrentThreadId()<<endl;
    //printp(g,dian,loc);
    if (zhaodao)
        return false;

    //处理完最后的黑格子就要把白格子存在一个对向量里面， 然后交给solveAkari3处理
    if (loc == dian.size())
    {
        vector<std::pair<unsigned, unsigned>> dian2;

        for (unsigned i = 0; i < g.size(); i++)
        {
            for (unsigned j = 0; j < g[0].size(); j++)
            {
                if (g[i][j] == -2)
                {
                    if (check2(g, i, j) == 0)
                        return false;
                    dian2.push_back(std::make_pair(i, j));
                }
            }
        }
        end1 = clock();
        return solveAkari3(g, dian2, 0);
    }

    unsigned x = dian[loc].first, y = dian[loc].second;
    if (!g[x][y])
        return solveAkari2(g, dian, loc + 1);

    if (!check(g, x, y))
        return solveAkari2(g, dian, loc + 1);

    HANDLE hThread[4];
    struct chuanCan cc[4];

    int n = 0;
    bool k;
    if (x - 1 < g.size() && g[x - 1][y] == -2)
    {
        vector<vector<int>> gg(g);
        if (create(gg, x - 1, y))
        {
            k = checkNext(gg, dian);
            if (k && hnum < HNUM)
            {
                DWORD threadId;
                cc[n] = {gg, dian, loc, 0};
                hThread[n] = CreateThread(NULL, 0, threadFunc, cc + n, 0, &threadId);
                n++;
            }
            else if (k && solveAkari2(gg, dian, loc))
            {
                g.swap(gg);
                return true;
            }
        }
    }

    if (x + 1 < g.size() && g[x + 1][y] == -2)
    {
        vector<vector<int>> gg(g);
        if (create(gg, x + 1, y))
        {
            k = checkNext(gg, dian);
            if (k && hnum < HNUM)
            {
                DWORD threadId;
                cc[n] = {gg, dian, loc, 0};
                hThread[n] = CreateThread(NULL, 0, threadFunc, cc + n, 0, &threadId);
                n++;
            }
            else if (k && solveAkari2(gg, dian, loc))
            {
                g.swap(gg);
                return true;
            }
        }
    }

    if (y - 1 < g[0].size() && g[x][y - 1] == -2)
    {
        vector<vector<int>> gg(g);
        if (create(gg, x, y - 1))
        {
            k = checkNext(gg, dian);
            if (k && hnum < HNUM)
            {
                DWORD threadId;
                cc[n] = {gg, dian, loc, 0};
                hThread[n] = CreateThread(NULL, 0, threadFunc, cc + n, 0, &threadId);
                n++;
            }
            else if (k && solveAkari2(gg, dian, loc))
            {
                g.swap(gg);
                return true;
            }
        }
    }

    if (y + 1 < g[0].size() && g[x][y + 1] == -2)
    {
        vector<vector<int>> gg(g);
        if (create(gg, x, y + 1))
        {
            k = checkNext(gg, dian);
            if (k && hnum < HNUM)
            {
                DWORD threadId;
                cc[n] = {gg, dian, loc, 0};
                hThread[n] = CreateThread(NULL, 0, threadFunc, cc + n, 0, &threadId);
                n++;
            }
            else if (k && solveAkari2(gg, dian, loc))
            {
                g.swap(gg);
                return true;
            }
        }
    }

    WaitForMultipleObjects((DWORD)n, hThread, true, INFINITE);

    for (int i = 0; i < n; i++)
    {
        if ((cc[i].g)[0][0] == 100)
        {
            g.swap(cc[i].g);
            return true;
        }
    }

    return false;
}

bool checkNext(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian)
{
    for (unsigned i = 0; i < dian.size(); i++)
        if (check(g, dian[i].first, dian[i].second) == 1)
            return false;
    return true;
}

DWORD WINAPI threadFunc(LPVOID p)
{
    unsigned flag = ((struct chuanCan *)p)->flag;
    if (flag)
        bnum++;
    else
        hnum++;

    //vector<vector<int>> g=((struct chuanCan *)p)->g;
    vector<std::pair<unsigned, unsigned>> dian = ((struct chuanCan *)p)->dian;
    unsigned loc = ((struct chuanCan *)p)->loc;

    if (!flag)
    {
        solveAkari2(((struct chuanCan *)p)->g, dian, loc);
        hnum--;
    }
    else
    {
        solveAkari3(((struct chuanCan *)p)->g, dian, loc);
        bnum--;
    }

    return 0;
}

bool solveAkari3(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc)
{
    //cout<<"id:"<<GetCurrentThreadId()<<endl;
    //printp(g,dian,loc);
    if (zhaodao)
        return false;

    //当所有要处理的白格子都已经处理，要检查一下是否还有没被照亮的白格子
    if (loc == dian.size())
    {
        for (unsigned i = 0; i < loc; i++)
        {
            if (g[dian[i].first][dian[i].second] == -2)
                return false;
        }
        zhaodao = true;
        true_value = g[0][0];
        g[0][0] = 100;

        end2 = clock();

        return true;
    }

    //检查之前跳过的白格子是否有希望被照亮，没有则直接返回false
    for (unsigned i = 0; i < loc; i++)
    {
        if (check2(g, dian[i].first, dian[i].second) == 0)
            return false;
    }

    unsigned x = dian[loc].first, y = dian[loc].second;

    //根据检查的结果决定如何操作
    int ck = check2(g, x, y);
    switch (ck)
    {
    case 0:
        return false;
    case 1:
        return solveAkari3(g, dian, loc + 1);
    case 2:
    {
        vector<vector<int>> gg(g);
        if (create(gg, x, y) && solveAkari3(gg, dian, loc + 1))
        {
            g.swap(gg);
            return true;
        }
        return false;
    }
    case 3:
    {
        vector<vector<int>> gg(g);

        HANDLE hThread = 0;
        struct chuanCan cc;

        if (bnum < BNUM)
        {
            DWORD threadId;
            vector<vector<int>> ggg(g);
            cc = {ggg, dian, loc + 1, 1};
            hThread = CreateThread(NULL, 0, threadFunc, &cc, 0, &threadId);
        }

        if (create(gg, x, y) && solveAkari3(gg, dian, loc + 1))
        {
            g.swap(gg);
            return true;
        }

        if (hThread)
        {
            WaitForSingleObject(hThread, INFINITE);
            if ((cc.g)[0][0] == 100)
            {
                g.swap(cc.g);
                return true;
            }
            return false;
        }

        else
            return solveAkari3(g, dian, loc + 1);
    }
    }
    return false;
}

//用于检查黑格子的情况，0代表已经满足，1代表不能满足，2代表有可能满足
int check(vector<vector<int>> &g, unsigned x, unsigned y)
{
    int yfz = 0, kfz = 0;
    if (x - 1 < g.size())
    {
        if (g[x - 1][y] == 5)
            yfz++;
        if (g[x - 1][y] == -2)
            kfz++;
    }
    if (x + 1 < g.size())
    {
        if (g[x + 1][y] == 5)
            yfz++;
        if (g[x + 1][y] == -2)
            kfz++;
    }
    if (y - 1 < g.size())
    {
        if (g[x][y - 1] == 5)
            yfz++;
        if (g[x][y - 1] == -2)
            kfz++;
    }
    if (y + 1 < g.size())
    {
        if (g[x][y + 1] == 5)
            yfz++;
        if (g[x][y + 1] == -2)
            kfz++;
    }
    if (g[x][y] == 0 && yfz != 0)
        return 1;

    if (g[x][y] == 1)
    {
        if (yfz == 1)
            return 0;
        if (yfz != 0 || kfz == 0)
            return 1;
    }
    else if (g[x][y] == 2)
    {
        if (yfz == 2)
            return 0;
        if (yfz > 2 || yfz + kfz < 2)
            return 1;
    }
    else if (g[x][y] == 3)
    {
        if (yfz == 3)
            return 0;
        if (yfz > 3 || yfz + kfz < 3)
            return 1;
    }
    return 2;
}

//用于检查白格子的情况，0代表不可能亮，1代表有被别的灯照亮的可能或者已经照亮，2代表自身可以放灯，3代表能放灯且有可能被其他灯照亮
int check2(vector<vector<int>> &g, unsigned x, unsigned y)
{
    if (g[x][y] >= 5)
        return 1;
    bool flag = check3(g, x, y);

    bool k = 0;
    for (unsigned i = x - 1; i < g.size(); i--)
    {
        if (g[i][y] == -2)
            k = check3(g, i, y);
        if (k || (g[i][y] > -2 && g[i][y] < 5))
            break;
    }
    if (!k)
    {
        for (unsigned i = x + 1; i < g.size(); i++)
        {
            if (g[i][y] == -2)
                k = check3(g, i, y);
            if (k || (g[i][y] > -2 && g[i][y] < 5))
                break;
        }
    }
    if (!k)
    {
        for (unsigned i = y - 1; i < g[0].size(); i--)
        {
            if (g[x][i] == -2)
                k = check3(g, x, i);
            if (k || (g[x][i] > -2 && g[x][i] < 5))
                break;
        }
    }
    if (!k)
    {
        for (unsigned i = y + 1; i < g[0].size(); i++)
        {
            if (g[x][i] == -2)
                k = check3(g, x, i);
            if (k || (g[x][i] > -2 && g[x][i] < 5))
                break;
        }
    }

    if (k && flag)
        return 3;
    if (k && !flag)
        return 1;
    if (!k && flag)
        return 2;
    return 0;
}

//用于检查白格子能否放灯，1代表可以
bool check3(vector<vector<int>> &g, unsigned x, unsigned y)
{
    bool flag = 1; //能否放灯
    if (x - 1 < g.size() && g[x - 1][y] >= 0 && g[x - 1][y] <= 5)
        flag = 0;
    if (flag && x + 1 < g.size() && g[x + 1][y] >= 0 && g[x + 1][y] <= 5)
        flag = 0;
    if (flag && y - 1 < g.size() && g[x][y - 1] >= 0 && g[x][y - 1] <= 5)
        flag = 0;
    if (flag && y + 1 < g.size() && g[x][y + 1] >= 0 && g[x][y + 1] <= 5)
        flag = 0;
    return flag;
}

//创建灯，兼顾检查能否放置灯，能放会改变g，不能则不会
bool create(vector<vector<int>> &gg, unsigned x, unsigned y)
{
    if (gg[x][y] == 5)
        return true;

    if (gg[x][y] > 5)
        return false;

    vector<vector<int>> g(gg);
    for (unsigned i = x - 1; i < g.size(); i--)
    {
        if (g[i][y] == 5)
            return false;
        else if (g[i][y] > -2 && g[i][y] < 5)
            break;
        else if (g[i][y] == -2)
            g[i][y] = 6;
        else
            g[i][y]++;
    }
    for (unsigned i = x + 1; i < g.size(); i++)
    {
        if (g[i][y] == 5)
            return false;
        if (g[i][y] > -2 && g[i][y] < 5)
            break;
        else if (g[i][y] == -2)
            g[i][y] = 6;
        else
            g[i][y]++;
    }
    for (unsigned i = y - 1; i < g[0].size(); i--)
    {
        if (g[x][i] == 5)
            return false;
        if (g[x][i] > -2 && g[x][i] < 5)
            break;
        else if (g[x][i] == -2)
            g[x][i] = 6;
        else
            g[x][i]++;
    }
    for (unsigned i = y + 1; i < g[0].size(); i++)
    {
        if (g[x][i] == 5)
            return false;
        if (g[x][i] > -2 && g[x][i] < 5)
            break;
        else if (g[x][i] == -2)
            g[x][i] = 6;
        else
            g[x][i]++;
    }
    g[x][y] = 5;
    gg.swap(g);
    return true;
}

void printp(vector<vector<int>> &g, const vector<std::pair<unsigned, unsigned>> &dian, unsigned loc)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    for (unsigned i = 0; i < g.size(); i++)
    {
        for (unsigned j = 0; j < g[0].size(); j++)
        {
            if (g[i][j] > 5)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
            else if (g[i][j] == 5)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
            else if (g[i][j] == -2)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            else if (g[i][j] == -1)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            else
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
            if (i == dian[loc].first && j == dian[loc].second)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            printf("%4d", g[i][j]);
        }
        cout << endl;
    }
    for (unsigned i = 0; i < dian.size(); i++)
        printf("%d:%d %d\n", i, dian[i].first, dian[i].second);
    printf("loc:%d\n", loc);
    system("pause");
    system("cls");
}

void printP(vector<vector<int>> &g, unsigned x, unsigned y)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    for (unsigned i = 0; i < g.size(); i++)
    {
        for (unsigned j = 0; j < g[0].size(); j++)
        {
            if (g[i][j] > 5)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
            else if (g[i][j] == 5)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
            else if (g[i][j] == -2)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            //else if(g[i][j]==-1) SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            else
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
            if (i == x && j == y)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            printf("%4d", g[i][j]);
        }
        cout << endl;
    }
    system("pause");
    system("cls");
}