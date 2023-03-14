#include <stdio.h>
#include <stdlib.h>
#define ElemType int
#define _DEBUG_ 1

typedef struct Lnode
{
    ElemType data;      //数据域
    struct Lnode *next; //指针域
} Lnode, *LinkList;     //创建链表节点

Lnode *first; //创建头指针

Lnode *initList()//初始化链表
{
    Lnode *p = (Lnode *)malloc(sizeof(Lnode));//给这个头结点分配动态内存空间
    if (!p) //如果p = NULL那么直接退出程序，分配头结点失败
    {
        printf("初始化分配头结点失败\n");
        exit(0);
    }
    p->next = NULL; //让p指向的节点为NULL
    return p;//返回p，除了头结点p之外，都为空，那么初始化完成
}

int ListLength(LinkList p) //测定链表长度的函数
{
    int length = 0;
    while (p->next != NULL) //利用while循环从头到尾遍历测节点个数
    {
        length++;
        p = p->next;
    }
    return length; //最后返回节点个数也即长度
}

int ListEmpty(LinkList p) //判断链表是否为空
{
    if (p->next!=NULL) //若为空表返回FALSE，否则返回TRUE
        return 1;
    return 0;
}

void ClearList(LinkList p) //将线性表重置为空表
{
    Lnode *q;
    while (p->next != NULL)
    {
        q = p->next; // q作为p的下一个节点
        p->next = q->next;
        free(q); //往后遍历一一释放
    }
}

void DestroyList(LinkList p) //销毁线性表
{
    ClearList(p);
    free(p); //先让线性表置空，再把头结点释放，达到彻底销毁的目的
}

int GetElem(LinkList p, int i, ElemType *e) //用*e返回LinkList中第i个数据元素的值
{
    int k = 0;
    while (p = p->next) // while循环向后一一遍历查询
    {
        k++;
        if (k == i)
        {
            *e = p->data; //替换
            return k;
        }
    }
    return 0;
}

Lnode *LocateElem(LinkList p, ElemType e) //返回线性表中第一个e元素的位序
{
    while (p = p->next)
    {
        if (p->data == e)//数据域等于e
            return p;//返回数据域等于e对应的位序即p
    }
    return NULL;//寻找失败返回NULL
}

Lnode *PriorElem(LinkList p, Lnode *cur_e) //操作结果：若cur_e是L的数据元素，且不是第一个，则返回它的前驱p
{
    for (; p->next; p = p->next)//p->next只要不是空指针，那么for循环将进行下去
        if (p->next == cur_e)
            return p; // p即为前驱，p->next是cur_e
    return NULL;//寻找失败，返回前驱为空；
}

Lnode *NextElem(LinkList p, Lnode *cur_e) //若cur_e是链表中的数据元素，且不是最后一个，则返回它的后继cur_e->next;
{
    return cur_e->next;
}

Lnode *ListInsert(LinkList p, int i, ElemType e) //在线性表中第i个位置之前插入新的数据元素
{
    if (i < 1) //显然i不能小于1，否则必须返回空指针
        return NULL;
    for (; p; p = p->next)//只要p不是空指针，就向后遍历循环
        if (--i < 1)
        {
            Lnode *q = (Lnode *)malloc(sizeof(Lnode));//分配空间
            if (!q)//q=NULL就分配失败并退出程序
            {
                printf("插入节点时分配空间失败\n");
                exit(0);
            }
            q->next = p->next;
            p->next = q;//调整指针的指向
            q->data = e;//插入数据
            return q;//插入完成
        }
    return NULL;//如果插入失败就返回NULL
}

int ListDelete(LinkList p, int i, ElemType *e)//删除LinkList中的第i个元素，并用e返回，链表长度会减一
{
    if (i < 1)
        return 0;
    Lnode *q = p;
    for (p = p->next; p; p = p->next)//p从开头向后循环，循环实际上有i来判断是否结束
    {
        if (--i < 1)
        {
            q->next = p->next;
            *e = p->data;//将删掉的数据赋值给e
            free(p);//p释放删除
            return 1;//这几步操作实际在删除i处结点
        }
        q = p;
    }
    return 0;
}

void PrintLnode(Lnode *p) //输出数据域
{
    printf("%d,", p->data);
}

void Add2(Lnode *p) //+2函数
{
    p->data += 2;//结点中储存的数据+2
    printf("+2,");
}

void DebugLnode(Lnode *p)//打印节点对应的地址以及储存的数据；
{
    printf("结点-(*addr)=value: ");
    printf("(*%lx)=%d\n", p, p->data); 
}

void ListTraverse(LinkList p, void (*ptrFunc)(Lnode *ptr))//定义遍历函数，参数为p指针，以及函数指针ptrFunc；
{
    printf("链表(表长=%d):", ListLength(p));//输出表长；
    while (p = p->next) //执行DebugLinkNode函数输出地址以及数据；
        (*ptrFunc)(p);
    printf("\n");
}

int main()
{
    void (*ptrFunc)(Lnode * p) = DebugLnode;//*ptrFunc作为函数指针指向DebugLnode函数

    first = initList();//链表初始化用到initList函数

    ListTraverse(first, ptrFunc);
    ListInsert(first, 1, 2);
    ListInsert(first, 1, 3);
    ListInsert(first, 1, 4);
    ListInsert(first, 1, 5);
    ListTraverse(first, ptrFunc);
    ListInsert(first, 1, 6);
    ListInsert(first, 1, 7);
    ListInsert(first, 1, 8);
    ListInsert(first, 1, 9);
    ListTraverse(first, ptrFunc);
    ListInsert(first, 3, 66);
    ListInsert(first, 5, 77);
    ListInsert(first, 7, 88);
    ListInsert(first, 9, 99);
    ListTraverse(first, ptrFunc);//简单地调用以上定义的函数基本操作

    ElemType ei;
    printf("取数据之前 %d --", ei);
    GetElem(first, 10, &ei);
    printf("读取的数据为 %d \n", ei);//GetElem函数

    Lnode *q = LocateElem(first, 888);//LocateElem函数
    if (!q)
        printf("没有找对值所对应的结点\n");
    else
    {
        q = PriorElem(first, q);//PriorElem函数找前驱
        printf("找到结点前驱为 %d --", q->data);
        printf("找到结点为 %d --", q->next->data);
        if (q->next->next)
            printf("找到结点的后继为 %d ", NextElem(first, NextElem(first, q))->data);//NextElem函数找后继
        printf("\n");
    }
    printf("删除前的值 %d--", ei);
    if (ListDelete(first, 3, &ei) > 0)
        printf("删除的值为 %d\n", ei);//ListDelete函数删除第15处结点
    else
        printf("删除失败 %d \n", ei);
    ListTraverse(first, ptrFunc);//遍历

    ptrFunc = Add2;
    printf("每个数据元素准备+2\n");
    ListTraverse(first, ptrFunc);
    printf("完成+2后,新的链表为:");
    ListTraverse(first, PrintLnode);

    DestroyList(first);//最后销毁链表

    return 0;
}
