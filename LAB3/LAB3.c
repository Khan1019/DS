//遗传算法：求解优化问题的通用算法，人工智能中会非常实用
//将问题的解x表示为0-1串，然后搜索最优的二进制串，使得目标函数值f(x)达到最小

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** 算法参数设置 **/
//定义0-1串的长度，可用于表示解的精度
#define SLEN 200

//定义进化最大代数
#define MAXGEN 100

//变异概率
#define mProb 1.0 / SLEN

//父群体与子群体大小，子代必须是偶数（通过交叉染色体生成后代时候，一次生成两个后代）
#define PSIZE 10
#define CSIZE 20
#define SIZE (PSIZE + CSIZE)

// const int PM = 1e5 + 7; //放大rand

//一个解的定义
typedef struct
{
    int x[SLEN]; // x:解的自变量，0-1串
    double y;    // y=f(x),要优化问题的目标函数值
} Solution;

//定义一个解集/解数组：称之为群体population
Solution pop[SIZE];               //解集，父代和子代都存储在这里
Solution *parent = pop;           //当前代，也就是父代
Solution *children = pop + PSIZE; //子代解集

//函数声明
void printPop(Solution *p, const char *str);

//将0-1串x解码为实数*xo ,假定整数4bits，SLEN-4bits为小数部分长度
void decode(int *x, double *xo)
{
    // todo
    double b = 0;
    //整数部分换成10进制
    for (int i = 0; i < 4; i++)
    {
        b += x[i] * pow(2, 3 - i);
    }
    //小数部分换成10进制
    for (int j = 4; j < SLEN; j++)
    {
        b += x[j] * pow(2, 3 - j);
    }
    *xo = b;
}

//计算y=f(x) ,  0-1串x的长度 SLEN
//例子：求y=x*x-3x+2的最小值 ，假设整数部分4bits，小数部分SLEN-4bits
double func1(int *x)
{
    double xo;
    decode(x, &xo);              //将0-1串x解码成真正的解xo
    return xo * xo - 3 * xo + 2; //计算目标函数值
}

int sp(double p)
{
    int pm = 1e4 ;
    int py = 1e7 ;
    double x = 1.0 * (rand() % py * pm) / py;
    return x < p ? 1 : 0;
}

//计算一个群体的所有解的目标函数值y ，给出了函数指针，支持个函数的优化
void evaluate(Solution *P, double ptrf(int *))
{
    // todo
    for (int i = 0; i < CSIZE; ++i)
    {
        /*double xo;
        decode(P[i].x,&xo);*/
        P[i].y = ptrf(P[i].x);
    }
    // y=f(x)
}

//算法初始化：分配两个解集所需的空间，随机生成parentPop中的解，并计算其y值
void initialize()
{
    for (int i = 0; i < PSIZE; ++i)
    {
        //初始化第一代父代
        for (int j = 0, k = i; j < 4; k /= 2, ++j) //保证每个整数都能取到
            parent[i].x[3 - j] = k % 2;
        for (int j = 4; j < SLEN; ++j) //对每个解的0-1串，随机生成
            parent[i].x[j] = rand() % 2;
        // double xo=0;
        // decode(parent[i].x,&xo);
        // printf("%lf\n",xo);
        evaluate(parent + i, func1);
    }
}

//从父代中选择两个解，通过杂交生成两个子代个体
//父代两个解通过PK选择出来（锦标选择）
void crossover()
{ //交叉算子
    int k = 0;
    int vis[PSIZE] = {0};
    while (k < CSIZE)
    { //逐步生成子代，一次两个
        // todo
        //随机选择两个父代个体
        //随机确定父代个体染色体交换位点
        //交换交叉位点前面的两个子串
        int p1 = rand() % PSIZE, p2 = rand() % PSIZE; //两个母体)
        while (vis[p1])
            p1 = rand() % PSIZE; //不等于0重新选，防止多次选中
        while (p2 == p1)
            p2 = rand() % PSIZE; //防止自交
        vis[p1] = 1;
        //交叉互换
        int pos = rand() % (SLEN - 1) + 1; //保证每个母体至少贡献一位，这个交叉互换的起点从1开始
        // int pos=rand()%SLEN;
        /*
        可以考虑引入一个新的随机函数
        更强的母体有更大的概率表达更多的基因
        */
        for (int i = 0; i < pos; ++i) // children[k]和children[k+1]相当于交叉后的两个相邻染色体
        {
            children[k].x[i] = parent[p1].x[i];
            children[k + 1].x[i] = parent[p2].x[i];
        }
        for (int i = pos; i < SLEN; ++i)
        {
            children[k].x[i] = parent[p2].x[i];
            children[k + 1].x[i] = parent[p1].x[i];
        }
        k = k + 2;
    }
}

//对子代中的个体进行变异：变异概率为mProb
//所谓变异就是x[j]的取值 0-1互换： 0 <--> 1
void mutate()
{ //变异算子
    for (int i = 0; i < CSIZE; ++i)
        for (int j = 0; j < SLEN; ++j)
            if ((rand() % 100000) / 100000.0 < mProb)
                children[i].x[j] = 1 - children[i].x[j];
}

//从parentPop和children中选择下一代个体，有多种选择算法，但是通常都是先把两个群体中最好的保留下来 ，然后
//方法1：选择最好的PSIZE个为下一代（截断选择）
//方法2：给每个个体一个选择概率，y值小（好）的被选择的概率就高，然后依据此概率分布随机采样PSIZE个
//方法3：锦标选择，随机选择k个，相互pk，留下最好的放入下一代，依次选择PSIZE个 （不删除被选择了的）
void select(int k)
{                  //选择算子 ：采用锦标 选择
    double best_y; //锦标赛选出来的子代的y值
    int best;      //锦标赛选择出来的子代下标
    Solution tmp[PSIZE];
    for (int i = 0; i < PSIZE; ++i)
    {              //一个一个子代选择
        best = -1; // todo: 从pop[2*psize]中用方法1或2或3选择一个适应度值（y值）好的
        for (int j = 0; j < k; ++j)
        {
            int p = rand() % SIZE; //不考虑是否重复选择
            //但会导致子代快速收敛于某个母体 可以引入一个很小概率选择一个较劣的母体
            //但这样需要迭代更多次
            if (best == -1 || pop[p].y < best_y || sp(mProb))
            {
                best = p;
                best_y = pop[p].y;
            }
        }
        memcpy(&(tmp[i]), &(pop[best]), sizeof(Solution)); //选择出来的解，复制到临时解中
    }
    memcpy(parent, tmp, sizeof(Solution) * PSIZE);
}

//输出群体的信息
void printPop(Solution *p, const char *str)
{
    printf("%s/解集信息如下: \n", str);
    for (int i = 0; i < PSIZE; ++i)
    {
        printf("个体 %3d : y=%10.6lf=f(", i, p[i].y);
        double xo = 0;
        decode(p[i].x, &xo);
        printf("%lf", xo);
        /*for (int j = 0; j < SLEN; ++j)
            printf("%d", p[i].x[j]);*/
        printf(")\n");
    }
}

int main()
{
    int seed = 991;
    srand(seed); //设置随机数种子，使得算法结果可以重现
    printf("f(x)=x^2-3x+2\n");
    initialize();
    printf("第 %d 代 ", 0);
    printPop(parent, "当前群体");

    for (int gen = 1; gen < MAXGEN; gen++)
    {
        crossover();
        mutate();
        evaluate(children, func1);
        select(2);
        // printf("第 %d 代 ",gen);
        // printPop(parent,"当前群体");
    }
    printf("第 %d 代 ", MAXGEN);
    printPop(parent, "最终群体");
    return 1;
}
