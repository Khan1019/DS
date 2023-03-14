#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MOD0 //定长存储
//#define MOD1 //堆存储
#define MOD1

/*
InitSring(int length) 初始化串
Insert(Stringptr str,int k,int ch) 在第k个位置前插入字符ch
StringAt(Stringptr str,int k) 返回第k个字符
Length(Stringptr str) 返回字符串的长度
StringClear(Stringptr str) 清空字符串
StringDestroy(Stringptr str) 销毁字符串
StringPrint(Stringptr str) 打印所有元素
StringConcat(Stringptr ans,Stringptr str) 在ans后面插入str
Next(Stringptr pat,int *next) 求pat的next数组
KMP(Stringptr txt,Stringptr pat)
串的模式匹配，txt为主串，pat为模式串，返回值为模式串在主串中第一次出现的位置
SubString(Stringptr str,int begin,int size)
求子串，结果为主串中从第begin个字符开始后的size个字符
StringReplace(Stringptr *str,Stringptr T,Stringptr P)
字符串替换，将主串str中每个与串T相同的子串替换为串P
*/

//微调 定长 和 变长 的存储结构 使其更加统一

#ifdef MOD0

#define MAXLENGTH 255

typedef struct String
{
	char s[MAXLENGTH];
	int length;
} String, *Stringptr;

Stringptr InitString(int length) //函数参数表保持和变长统一
{
	Stringptr tmp = NULL;
	tmp = (Stringptr)malloc(sizeof(String));//分配空间
	if (!tmp)
		printf("初始化分配空间失败\n"), exit(0);
	tmp->length = length;
	return tmp;
}

void Insert(Stringptr str, int k, char ch)//在第k个位置前插入字符ch
{
	if (k < 0 || k > str->length)//k小于0或者大于串本身长度不合法
	{
		printf("插入位置不合法\n");
		return;
	}
	if (str->length == MAXLENGTH)
	{
		printf("存储空间不足\n");
		return;
	}
	for (int i = str->length - 1; i >= k; --i)//通过for循环从后往前插
		str->s[i + 1] = str->s[i];
	str->s[k] = ch;
	str->length += 1;
}

void StringClear(Stringptr str)//清空字符串
{
	str->length = 0;
}

#endif

#ifdef MOD1
//堆储存不规定长度
typedef struct String
{
	char *s;
	int length;
} String, *Stringptr;

Stringptr InitString(int length)
{
	Stringptr tmp = NULL;
	tmp = (Stringptr)malloc(sizeof(String));
	if (!tmp)
		printf("初始化分配空间失败\n"), exit(0);
	tmp->s = (char *)malloc(sizeof(char) * length);
	if (!tmp->s)
		printf("初始化分配空间失败\n"), exit(0);
	tmp->length = length;
	return tmp;
}

void Insert(Stringptr str, int k, char ch)
{
	if (k < 0 || k > str->length)
	{
		printf("插入位置不合法\n");
		return;
	}
	//重新分配空间
	str->s = (char *)realloc(str->s, sizeof(char) * (str->length + 1));
	for (int i = str->length - 1; i >= k; --i)//通过for循环从后往前插
		str->s[i + 1] = str->s[i];
	str->s[k] = ch;
	str->length += 1;
}

void StringClear(Stringptr str)
{
	if (str->s)
		free(str->s);//区别在于堆存储要把它释放掉
	str->s = NULL;//置空
	str->length = 0;
}

#endif

//以下函数不依赖于串的存储方式
char StringAt(Stringptr str, int k) { return str->s[k]; }//返回第k个字符

int Length(Stringptr str) { return str->length; }//返回字符串的长度

void StringDestroy(Stringptr str)//销毁字符串
{
	if (str)
		free(str);
}

void StringPrint(Stringptr str)//打印所有元素
{
	for (int i = 0; i < str->length; ++i)
		printf("%c", str->s[i]);
}

void StringConcat(Stringptr ans, Stringptr str)//在ans后面插入str
{
	for (int i = 0; i < str->length; ++i)
		Insert(ans, ans->length, str->s[i]);
}

void Next(Stringptr pat, int *next)//求pat的next数组
{  
	next[0] = -1;
	for (int i = 0, j = -1; i < pat->length - 1;)
	{
		if (j == -1 || pat->s[i] == pat->s[j])//如果相等比较下一位
		{
			next[++i] = ++j;
		}
		else
			j = next[j];//next[j]的值是看在s[j]之前的字符串中重复的子串长度,也就相当于是从新看重复字段
	}
}

//返回子串T在主串S中第pos个字符之后的位置，若不存在，返回-1
int KMP(Stringptr txt, int pos, Stringptr pat, int *next)//串的模式匹配，txt为主串，pat为模式串，返回值为模式串在主串中第一次出现的位置
{
	int i = pos, j = 0;
	while (i < txt->length && j < pat->length)//和寻找next步骤一样
	{
		if (j == -1 || txt->s[i] == pat->s[j])
		{ 
			++i;
			++j;
		}
		else
			j = next[j];
	}
	if (j >= pat->length)
		return i - pat->length;
	else
		return -1; //匹配失败
}

Stringptr SubString(Stringptr str, int begin, int size)//求子串，结果为主串中从第begin个字符开始后的size个字符
{
	if (begin < 0 || begin > str->length || size < 1 || begin + size > str->length)//无法寻找
		printf("无法查找子串");
	Stringptr tmp = InitString(size);//给临时串分配size的空间
	tmp->length = size;
	for (int i = 0; i < size; ++i)//for循环一一赋值 
		tmp->s[i] = str->s[begin + i];
	return tmp;
}

void StringReplace(Stringptr *str, Stringptr T, Stringptr P)//字符串替换，将主串str中每个与串T相同的子串替换为串P
{
	Stringptr txt = *str;
	Stringptr tmp = InitString(0);
	int *next = (int *)malloc(sizeof(int) * (T->length));
	Next(T, next);//取得*next
	int begin = 0, end = -1;
	while ((end = KMP(txt, begin, T, next)) != -1) //找T在txt中第一个出现的位置 找一个换一个，str中仍然有子串T
	{
		for (int i = begin; i < end; ++i)
			Insert(tmp, tmp->length, txt->s[i]);//先用tmp存储txt中被替换部分前的元素
		for (int i = 0; i < P->length; ++i)
			Insert(tmp, tmp->length, P->s[i]);//把p插到刚才存储的部分后面
		begin = end + T->length;//从刚被替换掉的T后重新开始找T，重复刚才的步骤
		end = -1;
	}
	for (int i = begin; i < txt->length; ++i)//begin已经到了最后一个T之后
		Insert(tmp, tmp->length, txt->s[i]);//把txt后面的接上
	*str = tmp;
}

int main()
{
	Stringptr txt = InitString(0);
	Insert(txt, 0, 'b');
	Insert(txt, 0, 'c');
	Insert(txt, 0, 'b');
	Insert(txt, 0, 'a');
	Insert(txt, 0, 'a');
	Insert(txt, 0, 'c');
	Insert(txt, 0, 'b');
	Insert(txt, 0, 'a');
	Insert(txt, 0, 'a');
	Insert(txt, 0, 'b');
	Insert(txt, 0, 'a');
	printf("txt:");
	StringPrint(txt);
	printf("\n");

	Stringptr pat = InitString(0);
	Insert(pat, 0, 'c');
	Insert(pat, 0, 'b');
	printf("pat:");
	StringPrint(pat);
	printf("\n");
	// printf("%d\n",pat->length);
	int *next = (int *)malloc(sizeof(int) * (pat->length));
	Next(pat, next);
	// for(int i=0;i<pat->length;++i)
	// printf("%d ",next[i]);
	// printf("\n");
	printf("第一个匹配位置:%d\n", KMP(txt, 0, pat, next));

	StringConcat(txt, pat);
	printf("txt:");
	StringPrint(txt);
	printf("\n");

	StringClear(pat);
	Insert(pat, 0, 'b');
	printf("pat:");
	StringPrint(pat);
	printf("\n");

	Stringptr str = InitString(0);
	Insert(str, 0, 'b');
	Insert(str, 0, 'b');
	printf("str:");
	StringPrint(str);
	printf("\n");
	StringReplace(&txt, pat, str);
	printf("txt:");
	StringPrint(txt);
	printf("\n");

	StringDestroy(str);
	StringDestroy(txt);
	StringDestroy(pat);
	return 0;
}
