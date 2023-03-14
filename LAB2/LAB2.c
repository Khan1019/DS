#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Digit
{
	int weight;		//位权 
	int data;		//位上的数据 
	struct Digit *next;
} Digit, *ToDigit;

Digit *InitDigit()
// 创建一个高精度数，哨兵节点权为1时为正数，为-1时为负数 ，空数为0，0为正数
{
	Digit *p = (Digit *)malloc(sizeof(Digit));
	if (!p)
	{
		printf("分配空间失败！\n");
		exit(0);
	}
	p-> next = NULL;
	p-> weight = 0;		//此时为空数 
	return p;
}

void GetDigit(ToDigit p,char str[])
//将字符串转化为高精度数 
{
	if (str[0] == '-')		//判断是否为负数 
	{
		p-> weight = -1;
	}
	else
	{
		p-> weight = 1;
	}
	int point = 0;
	int i;
	int len;
	len = strlen(str);
	for (i = 1; i < len; i++)		//寻找小数点 ，小数点肯定不在第一位 
	{
		if (str[i] == '.')
		{
			point = i;
		}
	}
	if (point == 0)		//没有小数点 
	{
		point = len;		//小数点实际在数尾 
	}
	Digit *temp = p;		//临时节点，插入数据用 
	for (i = 0; i < len; i++)		//正式读数 
	{
		if(i == 0 && (str[0] == '+' || str[0] == '-'))
		{
			continue;		//符号不计 
		}
		Digit *q = (Digit *)malloc(sizeof(Digit));
		if (!q)
		{
			printf("分配空间失败！\n");
			exit(0);
		}
		if (i < point)		//整数部分，从小数点向前位权为0,1,2...... 
		{
			q-> weight = point - i - 1;
		}
		if (i == point)
		{
			free(q);
			continue;		//小数点不读入 
		}
		if (i > point)		//小数部分，从小数点向后位权为-1,-2...... 
		{
			q-> weight = point - i;
		}
		q-> data = str[i] - '0';
		q-> next = NULL;
		temp-> next = q;		//临时节点后移 
		temp = q;
		q = NULL;
	}
}

void Del0_Digit(ToDigit p)
//删去前后多余的0 
{
	Digit *temp;
	int key = 1;
	while(p->next->data == 0 && p->next->weight > 0)		//删去前导0 
	{
		temp = p->next;
		p->next = temp->next;
		free(temp);
	}
	while(key)
	{
		key = 0;	//操作关键数置0
		temp = p;
		while(temp->next->next)
		{
			temp = temp->next;
		}
		if (temp->next->data == 0 && temp->next->weight < 0)
		{
			free(temp->next);
			temp->next = NULL;
			key = 1;		//本轮有操作 
		}
	}
}

int Compare_absolute_Digit(ToDigit p, ToDigit q)
//比较两个高精度数的绝对值大小 
{
	Del0_Digit(p);
	Del0_Digit(q);
	if (p-> next-> weight > q-> next-> weight)		//最高位权不同 
	{
		return 1;
	}
	if (p-> next-> weight < q-> next-> weight)
	{
		return -1;
	}
	if (p-> next-> data > q-> next-> data)		//最高位权相同，比较最高位 
	{
		return 1;
	}
	if (p-> next-> data < q-> next-> data) 
	{
		return -1;
	}
	if (p-> next-> data == q-> next-> data) 
	{
		if (p-> next->next == NULL && (q-> next-> next != NULL))		//比较至某数末尾 
		{
			return -1;
		}
		if (p-> next->next != NULL && (q-> next-> next == NULL))		//比较至某数末尾 
		{
			return 1;
		}
		if (p-> next->next == NULL && (q-> next-> next == NULL))		//比较至某数末尾 
		{
			return 0;
		}
		p = p-> next;
		q = q-> next;
		return Compare_absolute_Digit(p,q);		//比较次一位 
	}
}

void DealDigit(ToDigit p)
//重整高精度数，将数据统一为0-9
{
	Digit *temp;
	int key = 1;
	while(key)
	{
		key = 0;		//操作关键数置零
		temp = p-> next;		//指针回到表头 
		while(temp-> next)
		{
			while(temp-> next-> data > 9)
			{
				temp-> data++;
				temp-> next-> data -= 10;
				key = 1;		//本轮有过操作 
			}
			while(temp-> next-> data < 0)
			{
				temp-> data--;
				temp-> next-> data += 10;
				key = 1;
			}
			temp = temp-> next;		//处理下一位 
		}
		if (p-> next-> data > 9)		//处理第一位
		{
			Digit *q = (Digit *)malloc(sizeof(Digit));
			if (!q)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			q-> data = 0;
			q-> weight = p-> next-> weight + 1;
			q-> next = p-> next;
			p-> next = q;
			q = NULL;
			while(p-> next-> next-> data > 9)
			{
				p-> next-> data ++;
				p-> next-> next-> data -= 10;
			}
		}
	}
}

Digit *AddDigit(ToDigit p, ToDigit q)
//高精度数加法
{
	Digit *res, *temp1, *temp2, *temp3, *temp;
	res = InitDigit();		//创建结果链表
	if (p-> weight == q-> weight)		//确定结果符号，确定算法 
	{
		res-> weight = p-> weight;
	}
	if(p-> weight != q-> weight && Compare_absolute_Digit(p, q) > -1)		//异号，将绝对值较小的数取反 
	{
		res-> weight = p-> weight;
		temp2 = q-> next;
		while(temp2)
		{
			temp2->data = temp2->data *(-1);
			temp2 = temp2->next;
		}
	}
	if(p-> weight != q-> weight && Compare_absolute_Digit(p, q) == -1)
	{
		res-> weight = q-> weight;
		temp1 = p-> next;
		while(temp1)
		{
			temp1->data = temp1->data *(-1);
			temp1 = temp1->next;
		}
	}
	
	//将两个链表合并 
	
	temp1 = p->next;
	temp2 = q->next;
	temp3 = res;
	while(temp1 && temp2)
	{
		if(temp1->weight > temp2->weight)		//某一个数最高位更高 
		{
			temp = (Digit *)malloc(sizeof(Digit));
			if (!temp)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			temp->data = temp1->data;
			temp->weight = temp1->weight;
			temp->next = NULL;
			temp3->next = temp;
			temp3 = temp3->next;
			temp = NULL;
			temp1 = temp1->next;
		}
		if(temp1->weight < temp2->weight)
		{
			temp = (Digit *)malloc(sizeof(Digit));
			if (!temp)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			temp->data = temp2->data;
			temp->weight = temp2->weight;
			temp->next = NULL;
			temp3->next = temp;
			temp3 = temp3->next;
			temp = NULL;
			temp2 = temp2->next;
		}
		if(temp1->weight == temp2->weight)		//在这一位上，两个数都存在 
		{
			temp = (Digit *)malloc(sizeof(Digit));
			if (!temp)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			temp->data = temp1->data + temp2->data;
			temp->weight = temp1->weight;
			temp->next = NULL;
			temp3->next = temp;
			temp3 = temp3->next;
			temp = NULL;
			temp1 = temp1->next;
			temp2 = temp2->next;
		}
	}
	while(temp1)		//某一个数已加完 
	{
		temp = (Digit *)malloc(sizeof(Digit));
		if (!temp)
		{
			printf("分配空间失败！\n");
			exit(0);
		}
		temp->data = temp1->data;
		temp->weight = temp1->weight;
		temp->next = NULL;
		temp3->next = temp;
		temp3 = temp3->next;
		temp = NULL;
		temp1 = temp1->next;
	}
	while(temp2)
	{
		temp = (Digit *)malloc(sizeof(Digit));
		if (!temp)
		{
			printf("分配空间失败！\n");
			exit(0);
		}
		temp->data = temp2->data;
		temp->weight = temp2->weight;
		temp->next = NULL;
		temp3->next = temp;
		temp3 = temp3->next;
		temp = NULL;
		temp2 = temp2->next;
	}
	DealDigit(res);		//对结果重整
	if(p-> weight != q-> weight && Compare_absolute_Digit(p, q) > -1)		//还原pq 
	{
		temp2 = q-> next;
		while(temp2)
		{
			temp2->data = temp2->data *(-1);
			temp2 = temp2->next;
		}
	}
	if(p-> weight != q-> weight && Compare_absolute_Digit(p, q) == -1)
	{
		temp1 = p-> next;
		while(temp1)
		{
			temp1->data = temp1->data *(-1);
			temp1 = temp1->next;
		}
	}
	return res;
}

Digit *SubDigit(ToDigit p, ToDigit q)
//高精度数减法 
{
	Digit *res;
	q->weight = q->weight * (-1);		//p - q == p + (-q)
	res = AddDigit(p, q);
	q->weight = q->weight * (-1);		//还原q 
	return res;
}

Digit *MulDigit(ToDigit p, ToDigit q, int n)
//高精度数乘法,小数点n位以后截尾 
{
	Digit *res, *temp1, *temp2, *temp3, *temp;
	res = InitDigit();
	res->weight = p->weight * q->weight;		//确定结果符号
	
	temp1 = p->next;
	temp2 = q->next;
	int top_p, top_q, bot_p, bot_q;		//求结果的最高位和最低位 
	top_p = temp1->weight;
	top_q = temp2->weight;
	while(temp1->next)
	{
		temp1 = temp1->next;
	}
	bot_p = temp1->weight;
	while(temp2->next)
	{
		temp2 = temp2->next;
	}
	bot_q = temp2->weight;
	
	temp3 = res;
	int i = top_p + top_q;
	for (;(i >= bot_p + bot_q && i >= -n); i--)		//将所需的链表空间分配好 
	{
		temp = (Digit *)malloc(sizeof(Digit));
		if (!temp)
		{
			printf("分配空间失败！\n");
			exit(0);
		}
		temp->data = 0;
		temp->weight = i;
		temp->next = NULL;
		temp3->next = temp;
		temp3 = temp3->next;
		temp = NULL;
	}
	temp1 = p->next;
	temp2 = q->next;
	temp3 = res->next;
	while(temp2)
	{
		while(temp1)
		{
			if (temp1->weight + temp2->weight < -n)		//超出精确度，截去 
			{
				break;
			}
			while(temp3->weight != temp1->weight + temp2->weight)		//在res中遍历，寻找对应权位 
			{
				if (!temp3->next)		//回到表头，继续遍历 
				{
					temp3 = res;
				}
				temp3 = temp3->next;
			}
			temp3->data = temp3->data + temp1->data * temp2->data;
			temp1 = temp1->next;
		}
		temp1 = p->next;		//复位 
		temp2 = temp2->next;
	}
	DealDigit(res);		//重整结果 
	return res;
}

void PutDigit(ToDigit p)
//输出一个高精度数 
{
	Del0_Digit(p);
	Digit *temp;
	temp = p->next;
	if (p->weight == -1)		//打印符号 
	{
		printf("-");
	}
	while(temp)		//打印数字 
	{
		if (temp->data < 10)		//10以内的数 
		{
			printf("%d",temp->data);
		}
		if (temp->data > 9)		//10以外的数 
		{
			printf("%c",'A' + temp->data - 10);
		}
		if (temp->weight == 0 && temp->next != NULL)		//小数点 
		{
			printf(".");
		}
		temp = temp->next;
	}
	printf("\n");
}

void CutDigit(ToDigit p, ToDigit q)
//q为空数，将p的小数部分截下，赋给q 
{
	Digit *temp;
	temp = p;
	while(temp->next->weight != 0)
	{
		temp = temp->next;
	}
	q->next = temp->next;
	q->weight = p->weight;		//将个位小数截走 
	temp->next = NULL;		//断开
	temp->next =(Digit *)malloc(sizeof(Digit));		//补上个位
	if (!temp->next)
	{
		printf("分配空间失败！\n");
		exit(0);
	}
	temp->next->data = q->next->data;
	temp->next->weight = 0;
	temp->next->next = NULL;
	q->next->data = 0;		//清空个位 
}

int Div_intDigit(ToDigit p, int N)
//p除以n,p为整数，返回余数，p变为商 
{
	Digit *temp;
	temp = p->next;
	int left;		//余数 
	while(temp)		//从高位向低位运算 
	{
		if (temp->next)
		{
			temp->next->data += (temp->data % N) * 10;
		}
		else
		{
			left = temp->data % N;		//到达个位 
		}
		temp->data = temp->data / N;
		temp = temp->next;
	}
	DealDigit(p);
	Del0_Digit(p);
	return left;
}

void Div_Digit(ToDigit p, int N, int n)
//除到精度的高精度数除法 
{
	Digit *temp, *temp1;
	temp1 = p->next;
	while(temp1)
	{
		if(temp1->next)
		{
			temp1->next->data += (temp1->data % N) * 10;
		}
		else if(temp1->data % N != 0 && temp1->weight > -n)		//未除尽且未到精度 
		{
			temp = (Digit *)malloc(sizeof(Digit));
			if(!temp)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			temp->data = (temp1->data % N) * 10;
			temp->weight = temp1->weight - 1;
			temp->next = NULL;
			temp1->next = temp;
			temp = NULL;
		}
		temp1->data = temp1->data / N;
		temp1 = temp1->next;
	}
	DealDigit(p);
	Del0_Digit(p);
}

Digit *MakeDigit(int n)
//将一个0-20整数转化为高精度数 
{
	Digit *p;
	p = InitDigit();
	if (n == 0)
	{
		GetDigit(p, "0");
	}
	if (n == 1)
	{
		GetDigit(p, "1");
	}
	if (n == 2)
	{
		GetDigit(p, "2");
	}
	if (n == 3)
	{
		GetDigit(p, "3");
	}
	if (n == 4)
	{
		GetDigit(p, "4");
	}
	if (n == 5)
	{
		GetDigit(p, "5");
	}
	if (n == 6)
	{
		GetDigit(p, "6");
	}
	if (n == 7)
	{
		GetDigit(p, "7");
	}
	if (n == 8)
	{
		GetDigit(p, "8");
	}
	if (n == 9)
	{
		GetDigit(p, "9");
	}
	if (n == 10)
	{
		GetDigit(p, "10");
	}
	if (n == 11)
	{
		GetDigit(p, "11");
	}
	if (n == 12)
	{
		GetDigit(p, "12");
	}
	if (n == 13)
	{
		GetDigit(p, "13");
	}
	if (n == 14)
	{
		GetDigit(p, "14");
	}
	if (n == 15)
	{
		GetDigit(p, "15");
	}
	if (n == 16)
	{
		GetDigit(p, "16");
	}
	if (n == 17)
	{
		GetDigit(p, "17");
	}
	if (n == 18)
	{
		GetDigit(p, "18");
	}
	if (n == 19)
	{
		GetDigit(p, "19");
	}
	if (n == 20)
	{
		GetDigit(p, "20");
	}
	return p;
}

Digit *int_10_to_N(ToDigit p, int N)
//整数p十进制转N进制 
{
	int place = 0;		//位权
	Digit *temp, *mun_0,  *res;
	mun_0 = MakeDigit(0);
	res = InitDigit();
	res->weight = p->weight;
	if (Compare_absolute_Digit(p, mun_0) == 0)		//任何进制下，0是0 
	{
		return p;
	}
	else
	{
		while(Compare_absolute_Digit(p, mun_0) == 1)
		{
			temp = (Digit *)malloc(sizeof(Digit));
			if (!temp)
			{
				printf("分配空间失败！\n");
				exit(0);
			}
			temp->data = Div_intDigit(p, N);
			Del0_Digit(p);		//删去多余的0 
			temp->weight = place;
			place++;		//位权推进 
			temp->next = res->next;
			res->next = temp;
			temp = NULL;
		}
	}
	return res;
}

int RemakeDigit(ToDigit p)
//将小高精度数转为整型 
{
	Digit *temp;
	temp = p->next;
	int mun = 0;
	while(temp)
	{
		mun += temp->data * pow(10, temp->weight);
		temp = temp->next;
	}
	return mun;
}

Digit *dec_10_to_N(ToDigit p, int N, int n)
//小数p十进制转N进制 
{
	Digit *temp, *q, *temp1, *temp2, *mun_N, *r;
	int place = -1;
	q = MakeDigit(0);
	mun_N = MakeDigit(N);
	q->weight = p->weight;
	temp2 = q->next;
	while(place >= -n)
	{
		temp = (Digit *)malloc(sizeof(Digit));
		if (!temp)
		{
			printf("分配空间失败！\n");
			exit(0);
		}
		p = MulDigit(p, mun_N, n);
		DealDigit(p);		//重整p
		Del0_Digit(p);		//删0
		//以下将结果加入q中 
		r = InitDigit();
		CutDigit(p, r);
		temp->data = RemakeDigit(p);
		p = r;
		r = NULL;
		temp->weight = place;
		temp->next = NULL;
		place--;		//位权后移
		temp2->next = temp;
		temp2 = temp2->next;
		temp = NULL;
		p->next->data = 0;
	}
	return q;
}

Digit *Ten_to_N(ToDigit p, int N, int n)
//十进制转N进制 
{
	Digit *q, *temp;
	q = InitDigit();
	CutDigit(p, q);		//分开进行 
	p = int_10_to_N(p, N);
	q = dec_10_to_N(q, N, n);
	temp = p;
	while(temp->next)		//合并 
	{
		temp = temp->next;
	}
	temp->next = q->next->next;
	return p;
}

Digit *Two_to_Ten(ToDigit p, int n)
//二进制转十进制 
{
	Digit *temp1, *q, *mun_2, *mun_1, *temp;
	temp1 = p->next;
	q =MakeDigit(0);
	q->weight = 1;		//先假定为正，方便计算 
	mun_2 = MakeDigit(2);
	mun_1 = MakeDigit(1);
	int i;
	while(temp1)
	{
		if(temp1->data == 1 && temp1->weight >= 0)
		{
			temp = MakeDigit(1);
			for(i = 0; i < temp1->weight; i++)
			{
				temp = MulDigit(temp, mun_2, n);
			}
			q = AddDigit(q, temp);
			temp = NULL;
		}
		if(temp1->data == 1 && temp1->weight < 0)
		{
			temp = MakeDigit(1);
			for(i = 0; i > temp1->weight; i--)
			{
				Div_Digit(temp, 2, n);
			}
			q = AddDigit(q, temp);
			temp = NULL;
		}
		temp1 = temp1->next;
	}
	q->weight = p->weight;		//确定符号 
	return q;	




























































	
}

int main()
{
	int n, N;
	int i;
	char str[1024];
	Digit *p, *q, *res;
	p = InitDigit();
	q = InitDigit();
	printf("输入精度：");
	scanf("%d", &n);
	printf("选择功能：\n1.计算加减乘\n2.十进制转N进制\n3.二进制转十进制\n4.计算f(x)=3/7*x^3-1/3*x^2+4\n");
	scanf("%d", &i);
	switch(i)
	{
		case 1:
			{
				printf("输入第一个数：");
				scanf("%s", str);
				GetDigit(p, str);
				printf("输入第二个数：");
				scanf("%s", str);
				GetDigit(q, str);
				printf("a + b = ");
				res = AddDigit(p, q);
				PutDigit(res);
				printf("a - b = ");
				res = SubDigit(p, q);
				PutDigit(res);
				printf("a * b = ");
				res = MulDigit(p, q, n);
				PutDigit(res);
				break;
			}
		case 2:
			{
				printf("输入进制：");
				scanf("%d", &N);
				printf("输入十进制数：");
				scanf("%s", str);
				GetDigit(p, str);
				q = Ten_to_N(p, N, n);
				PutDigit(q);
				break;
			}
		case 3:
			{
				printf("输入二进制数：");
				scanf("%s", str);
				GetDigit(p, str);
				q = Two_to_Ten(p, n);
				PutDigit(q);
				break;
			}
		case 4:
			{
				printf("输入x：");
				scanf("%s", str);
				GetDigit(p, str);
				res = MakeDigit(0);		//结果置零
				//计算三次项 
				q = MakeDigit(3)
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				






























































































































































				
				;
				Div_Digit(q, 7, n);
				for(i = 0; i < 3; i++)
				{
					q = MulDigit(q, p, n);
				}
				res = AddDigit(res, q);
				//计算二次项 
				Digit *mun_1 = MakeDigit(1);
				q = MakeDigit(0);
				q = SubDigit(q, mun_1);
				Div_Digit(q, 3, n);
				for(i = 0; i < 2; i++)
				{
					q = MulDigit(q, p, n);
				}
				res = AddDigit(res, q);
				//计算常数项
				q = MakeDigit(4);
				res = AddDigit(res, q);
				//输出
				PutDigit(res); 
			}
	}
	return 0;
}