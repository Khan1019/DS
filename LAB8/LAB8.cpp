#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXVERTEX 100
#define MAXEDGE 20000
#define INF 20000

//顶点结构体
typedef struct Vertex
{
    int id;                //顶点编号
    int w;                 //顶点权值
    int degree;            //顶点的度
    long edges[MAXVERTEX]; //边的编号
    int group;
};

//边结构体
typedef struct Edge
{
    long id; //边的编号
    int h;   //边头顶点编号
    int t;   //边尾顶点编号
    int w;   //权值
};

//图结构体
typedef struct Graph
{
    Vertex v[MAXVERTEX];    //顶点数组
    Edge e[MAXEDGE];        //边数组
    int visited[MAXVERTEX]; //访问标记
    bool dirctional;        // t:有向图，f:无向图
    bool weighted;          // t:带权图，f:等权图
    unsigned nv;            //顶点数
    unsigned ne;            //边数
};

//生成一个随机图，包括n个顶点，每个顶点和其它顶点连边的概率为p
int randgengraph(int n, float p, bool directional, bool weighted, const char *filename, int seed)
{
    srand(seed);
    char *content = (char *)malloc(5000000);
    if (!content)
    {
        printf("分配存储空间失败！\n");
        exit(0);
    }
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("生成图文件%s时失败！\n", filename);
        exit(0);
    }
    long offset = 0; // content当前的写入位置
    int ne = 0;      //生成边数
    for (int i = 0; i < n; i++)
        offset += sprintf(content + offset, "%3d %5d\n", i, rand() % 100); //写入顶点的编号和顶点随机权值
    int pn = (int)(10000 * p);
    if (directional)
    { //有向图
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (i != j && rand() % 10000 < pn)
                {
                    offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
                    ne++;
                }
    }
    else
    { //无向图
        for (int i = 0; i < n; i++)
            for (int j = i; j < n; j++)
                if (i != j && rand() % 10000 < pn)
                {
                    offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
                    ne++;
                }
    }
    char ch[40];
    int sz = sprintf(ch, "%d %7d %5s %5s\n", n, ne, weighted ? "true" : "flase", directional ? "true" : "flase");
    fwrite(&ch, sz, 1, fp);         //写入图的顶点数和边数
    fwrite(content, offset, 1, fp); //写入图的顶点和边的信息
    fclose(fp);
    free(content);
}

//从文件中读入图，在内存中生成图的表示
void initgraph(Graph *g, const char *gfile)
{
    FILE *fp = fopen(gfile, "r");
    if (!fp)
    {
        printf("读取图数据文件出错！\n");
        exit(0);
    }
    char bv[10];
    //读取图的基本信息
    fscanf(fp, "%u%u%s", &(g->nv), &(g->ne), bv);
    // printf("%d %d %s", g->ne, g->nv, bv);
    g->weighted = strcmp(bv, "true") == 0 ? true : false;
    fscanf(fp, "%s", bv);
    g->dirctional = strcmp(bv, "true") == 0 ? true : false;
    //读取顶点信息
    for (int i = 0; i < g->nv; i++)
    {
        fscanf(fp, "%d%d", &(g->v[i].id), &(g->v[i].w));
        g->v[i].degree = 0;
        // printf("顶点%d-权%d\n", g->v[i].id, g->v[i].w);
    }
    //读取边信息
    for (unsigned long i = 0; i < g->ne; i++)
    {
        fscanf(fp, "%u%u%d", &(g->e[i].h), &(g->e[i].t), &(g->e[i].w));
        printf("边:%u-%u=>%d\n", g->e[i].h, g->e[i].t, g->e[i].w);
        g->e[i].id = i;
        unsigned h, t;
        h = g->e[i].h;
        t = g->e[i].t;
        g->v[t].edges[g->v[t].degree++] = i;
        if (g->dirctional == false)
            g->v[h].edges[g->v[h].degree++] = i;
    }
    fclose(fp);
}

inline long getEdgeIdx(Graph *g, long id);

//打印邻接表的信息
void printgraph(Graph *g)
{
    printf("图的基本信息:顶点数(%u)-边数(%lu)-%s-%s\n", g->nv, g->ne, g->weighted ? "加权图" : "等权图", g->dirctional ? "有向图" : "无向图");
    for (int i = 0; i < g->nv; i++)
    {
        printf("ID(%u)-度(%d)-权(%d)-边表 t(eid|w):%u", g->v[i].id, g->v[i].degree, g->v[i].w, g->v[i].id);
        for (int j = 0; j < g->v[i].degree; j++)
        {
            unsigned long E = g->v[i].edges[j];
            unsigned long e = getEdgeIdx(g, E);
            if (g->v[i].id == g->e[e].h)
                printf("->%u(%d, %d)", g->e[e].t, g->e[e].id, g->e[e].w);
            else
                printf("->%u(%d, %d)", g->e[e].h, g->e[e].id, g->e[e].w);
        }
        printf("\n");
    }
}

//查找给定编号的顶点，返回其顶点数组下标
inline int getVexIdx(Graph *g, int id)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].id == id)
            return i;
    printf("input wrong vertex id in getVexIdx()!\n");
    exit(0);
}

//查找给定编号的顶点，返回其权值
inline int getVexW(Graph *g, int id)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].id == id)
            return g->v[i].w;
    printf("input wrong vertex id in getVexW()!\n");
    exit(0);
}

//查找给定编号的顶点，返回其度
inline int getVexDegree(Graph *g, int id)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].id == id)
            return g->v[i].degree;
    printf("input wrong vertex id in getVexDegree()!\n");
    exit(0);
}

//查找给定编号的顶点,返回其所有边
inline long *getEdgesByNode(Graph *g, int id, int *ne)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].id == id)
        {
            *ne = g->v[i].degree;
            return g->v[i].edges;
        }
    printf("input wrong vertex id in getEdgesByNode()!\n");
    exit(0);
}

//查找给定编号的边，返回其边数组下标
inline long getEdgeIdx(Graph *g, long id)
{
    for (long i = 0; i < g->ne; i++)
        if (g->e[i].id == id)
            return i;
    printf("input wrong edge id in getEdgeIdx()!\n");
    exit(0);
}

//查找给定编号的边，返回其权值
inline int getEdgeW(Graph *g, long id)
{
    for (long i = 0; i < g->ne; i++)
        if (g->e[i].id == id)
            return g->e[i].w;
    printf("input wrong edge id in getEdgeW()!\n");
    exit(0);
}

//查找给定编号的边，返回其两个顶点
inline void getVexByEdge(Graph *g, long id, int *h, int *t)
{
    long i;
    for (i = 0; i < g->ne; i++)
        if (g->e[i].id == id)
        {
            *h = g->e[i].h;
            *t = g->e[i].t;
            return;
        }
    printf("input wrong edge id in getVexByEdge()!\n");
    exit(0);
}

//通过权值查找顶点编号
inline int locateVex(Graph *g, int w)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].w == w)
            return g->v[i].id;
    return -1;
}

//通过权值查找边编号
inline int locateEdge(Graph *g, int w)
{
    for (int i = 0; i < g->ne; i++)
        if (g->e[i].w == w)
            return g->e[i].id;
    return -1;
}

//设置顶点权值
inline void setVexW(Graph *g, int id, int w)
{
    for (int i = 0; i < g->nv; i++)
        if (g->v[i].id == id)
        {
            g->v[i].w = w;
            return;
        }
    printf("input wrong vertex id in setVexW()!\n");
    exit(0);
}

//设置边权值
inline void setEdgeW(Graph *g, int id, int w)
{
    for (long i = 0; i < g->ne; i++)
        if (g->e[i].id == id)
        {
            g->e[i].w = w;
            return;
        }
    printf("input wrong edge id in setEdgeW()!\n");
    exit(0);
}

//删除边
void deleteEdge(Graph *g, int id)
{
    int tid;
    long E;
    E = getEdgeIdx(g, id);
    tid = g->e[E].t;
    int hid = g->e[E].h;
    int V;
    V = getVexIdx(g, tid);
    int key = 0;
    int i;
    for (i = 0; i < g->v[V].degree; i++)
    {
        if (g->v[V].edges[i] == id)
        {
            key = 1;
        }
        if (key == 1)
        {
            g->v[V].edges[i] = g->v[V].edges[i + 1];
        }
    }
    g->v[V].degree--;
    V = getVexIdx(g, hid);
    key = 0;
    for (i = 0; i < g->v[V].degree; i++)
    {
        if (g->v[V].edges[i] == id)
        {
            key = 1;
        }
        if (key == 1)
        {
            g->v[V].edges[i] = g->v[V].edges[i + 1];
        }
    }
    key = 0;
    for (i = 0; i < g->ne; i++)
    {
        if (g->e[i].id == id)
        {
            key = 1;
        }
        if (key == 1)
        {
            g->e[i] = g->e[i + 1];
        }
    }
    g->v[V].degree--;
    g->ne--;
}

//删除顶点
void deleteVex(Graph *g, int id, int d)
{
    int i;
    i = getVexIdx(g, id);
    int j;
    j = g->v[i].degree;
    int k;
    if (d == 0)
    {
        for (k = 0; k < j; k++)
        {
            deleteEdge(g, g->v[i].edges[0]);
        }
    }
    j = g->ne;
    for (k = 0; k < j; k++)
    {
        // printf("%d\n",g->e[k].h);
        if (g->e[k].h == id)
        {
            deleteEdge(g, g->e[k].id);
            k--;
        }
    }
    int key = 0;
    for (k = 0; k < g->nv; k++)
    {
        if (g->v[k].id == id)
        {
            key = 1;
        }
        if (key == 1)
        {
            g->v[k] = g->v[k + 1];
        }
    }
    g->nv--;
}

//增加顶点
void addVex(Graph *g, int w)
{
    int id = g->nv;
    int key = 1;
    int i;
    while (key)
    {
        key = 0;
        for (i = 0; i < g->nv; i++)
        {
            if (g->v[i].id == id)
            {
                key = 1;
            }
        }
        if (key == 0)
        {
            break;
        }
        else
        {
            id++;
        }
    }
    g->v[g->nv].id = id;
    g->v[g->nv].degree = 0;
    g->v[g->nv].w = w;
    g->nv++;
}

//增加边
void addEdge(Graph *g, int w, int h, int t, int d)
{
    int id = g->ne;
    int key = 1;
    int i;
    while (key)
    {
        key = 0;
        for (i = 0; i < g->ne; i++)
        {
            if (g->e[i].id == id)
            {
                key = 1;
            }
        }
        if (key == 0)
        {
            break;
        }
        else
        {
            id++;
        }
    }
    g->e[g->ne].id = id;
    g->e[g->ne].w = w;
    g->e[g->ne].h = h;
    g->e[g->ne].t = t;
    g->ne++;
    int V;
    V = getVexIdx(g, t);
    g->v[V].edges[g->v[V].degree] = id;
    g->v[V].degree++;
    if (d == 0)
    {
        V = getVexIdx(g, h);
        g->v[V].edges[g->v[V].degree] = id;
        g->v[V].degree++;
    }
}

long list[MAXVERTEX];
int mun;
void DFS2(Graph *g, int id, int d);

void DFS1(Graph *g, int id, int d)
{
    int i;
    mun = 0;
    for (i = 0; i < MAXVERTEX; i++)
    {
        g->visited[i] = 0;
        list[i] = -1;
    }
    DFS2(g, id, d);
    int j = 1;
    while (j)
    {
        j = 0;
        for (i = 0; i < g->nv; i++)
        {
            if (g->visited[i] == 0)
            {
                j = 1;
                id = i;
                break;
            }
        }
        if (j)
        {
            DFS2(g, id, d);
        }
    }
    printf("深度优先搜索序列：");
    for (i = 0; i < g->nv; i++)
    {
        printf("%d ", list[i]);
    }
    printf("\n");
}

void DFS2(Graph *g, int id, int d)
{
    g->visited[id] = 1;
    list[mun++] = id;
    int i;
    int V, W;
    V = getVexIdx(g, id);
    for (i = 0; i < g->v[V].degree; i++)
    {
        if (d == 1)
        {
            W = getVexIdx(g, g->e[g->v[V].edges[i]].h);
        }
        else
        {
            if (V == getVexIdx(g, g->e[g->v[V].edges[i]].h))
            {
                W = getVexIdx(g, g->e[g->v[V].edges[i]].t);
            }
            else
            {
                W = getVexIdx(g, g->e[g->v[V].edges[i]].h);
            }
        }
        if (g->visited[g->v[W].id] == 0)
        {
            DFS2(g, g->v[W].id, d);
        }
    }
}

Vertex temp[MAXVERTEX];
int sum = 0;

void EnQueue(Graph *g, Vertex v)
{
    g->visited[v.id] = 1;
    temp[sum] = v;
    sum++;
}

Vertex DeQueue()
{
    Vertex v;
    v = temp[0];
    int i;
    for (i = 0; i < sum - 1; i++)
    {
        temp[i] = temp[i + 1];
    }
    sum--;
    return v;
}

int BFS2(Graph *g, int id, int d);

void BFS1(Graph *g, int id, int d)
{
    int i;
    mun = 0;
    for (i = 0; i < MAXVERTEX; i++)
    {
        g->visited[i] = 0;
        list[i] = -1;
    }
    int V;
    V = getVexIdx(g, id);
    EnQueue(g, g->v[V]);
    BFS2(g, id, d);
    int j = 1;
    while (j)
    {
        j = 0;
        for (i = 0; i < g->nv; i++)
        {
            if (g->visited[i] == 0)
            {
                j = 1;
                id = i;
                break;
            }
        }
        if (j)
        {
            V = getVexIdx(g, id);
            EnQueue(g, g->v[V]);
            BFS2(g, id, d);
        }
    }
    printf("广度优先搜索序列：");
    for (i = 0; i < g->nv; i++)
    {
        printf("%d ", list[i]);
    }
    printf("\n");
}

int BFS2(Graph *g, int id, int d)
{

    list[mun++] = id;
    int i;
    int V, W;
    V = getVexIdx(g, id);
    for (i = 0; i < g->v[V].degree; i++)
    {
        if (d == 1)
        {
            W = getVexIdx(g, g->e[g->v[V].edges[i]].h);
        }
        else
        {
            if (V == getVexIdx(g, g->e[g->v[V].edges[i]].h))
            {
                W = getVexIdx(g, g->e[g->v[V].edges[i]].t);
            }
            else
            {
                W = getVexIdx(g, g->e[g->v[V].edges[i]].h);
            }
        }
        if (g->visited[g->v[W].id] == 0)
        {
            EnQueue(g, g->v[W]);
        }
    }
    DeQueue();
    if (sum == 0)
    {
        return 0;
    }
    BFS2(g, temp[0].id, d);
}

int OK[MAXVERTEX];
int D[MAXVERTEX];

void littleway2(Graph *g, int id, int s, int d);

void littleway1(Graph *g, int id, int d)
{
    int i;
    for (i = 0; i < g->nv; i++)
    {
        D[i] = INF;
    }
    D[id] = 0;
    OK[0] = id;
    littleway2(g, id, 1, d);
    for (i = 0; i < g->nv; i++)
    {
        if (D[i] != INF)
        {
            printf("%d到%d最短距离为%d\n", id, i, D[i]);
        }
        else
        {
            printf("%d到%d没有路径\n", id, i);
        }
    }
}

void littleway2(Graph *g, int id, int s, int d)
{
    if (s == g->nv)
    {
        return;
    }
    int i;
    for (i = 0; i < g->ne; i++)
    {
        if (g->e[i].t == OK[s - 1])
        {
            if (D[g->e[i].h] > D[OK[s - 1]] + g->e[i].w)
            {
                D[g->e[i].h] = D[OK[s - 1]] + g->e[i].w;
            }
        }
    }
    if (d == 0)
    {
        for (i = 0; i < g->ne; i++)
        {
            if (g->e[i].h == OK[s - 1])
            {
                if (D[g->e[i].t] > D[OK[s - 1]] + g->e[i].w)
                {
                    D[g->e[i].t] = D[OK[s - 1]] + g->e[i].w;
                }
            }
        }
    }
    int min = INF;
    int j, k;
    for (i = 0; i < g->nv; i++)
    {
        if (min > D[i])
        {
            for (j = 0; j < s; j++)
            {
                if (i == OK[j])
                {
                    break;
                }
            }
            if (j == s)
            {
                min = D[i];
                k = i;
            }
        }
    }
    OK[s] = k;
    littleway2(g, id, s + 1, d);
}

void Howmany2(Graph *g, int id);
void Howmany1(Graph *g)
{
    int i;
    for (i = 0; i < MAXVERTEX; i++)
    {
        g->visited[i] = 0;
        list[i] = -1;
    }
    int count = 0;
    int id;
    while (1)
    {
        id = -1;
        for (i = 0; i < g->nv; i++)
        {
            if (g->visited[i] == 0)
            {
                id = i;
                break;
            }
        }
        if (id == -1)
        {
            break;
        }
        count++;
        Howmany2(g, id);
    }
    printf("连通片数量:%d\n", count);
}

void Howmany2(Graph *g, int id)
{
    int i;
    g->visited[id] = 1;
    int key = 1;
    while (key)
    {
        key = 0;
        for (i = 0; i < g->ne; i++)
        {
            if (g->visited[g->e[i].h] == 1 && g->visited[g->e[i].t] == 0)
            {
                g->visited[g->e[i].t] = 1;
                key = 1;
            }
            if (g->visited[g->e[i].t] == 1 && g->visited[g->e[i].h] == 0)
            {
                g->visited[g->e[i].h] = 1;
            }
        }
    }
}

int main()
{
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g)
    {
        printf("error\n");
        exit(0);
    }
    int i = 1;
    randgengraph(10, 0.2, i, 1, "graph.txt", 5);
    initgraph(g, "graph.txt");
    printgraph(g);

    /*int* h;
    h = (int*)malloc(sizeof(int));
    int* t;
    t = (int*)malloc(sizeof(int));*/
    // addVex(g, 30);
    // addEdge(g, 50, 10, 1, i);
    // getVexByEdge(g, 10, h, t);
    // printf("%d %d\n", *h, *t);
    DFS1(g, 0, i);
    BFS1(g, 0, i);
    littleway1(g, 0, i);
    Howmany1(g);
    // deleteVex(g, 5, i);
    // printgraph(g);
}