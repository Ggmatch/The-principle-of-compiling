#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
using namespace std;


#define P_Amount 20
#define RMAX 20
#define CMAX 20
#define NT_AMOUNT 20
#define T_AMOUNT 20
//文法结构体
class G
{
public:
	string T;  //终结符的集合
	string NT;  //非终结符的集合
	string begin;  //文法开始符号
	string P[P_Amount];  //产生式的集合
	int P_Length;  //产生式的个数
	bool FIRSTVT[RMAX][CMAX];  //非终结符的FIRSTVT集合
	bool LASTVT[RMAX][CMAX];  //非终结符的FIRSTVT集合
	int PRIORITY_TABLE[NT_AMOUNT][NT_AMOUNT];  //优先表，表内元素只有4种类型的值，-1代表行<列，1代表行>列，0代表行=列，2代表出错
	int FR;  //记录F的有效行数
	int FC;  //记录F的有效列数
	int f[T_AMOUNT];  //符号栈内的优先级,f优先函数
	int g[T_AMOUNT];  //符号栈外的优先级，g优先函数
	G()
	{
		//初始化
		T = "";
		NT = "";
		begin = "";
		P_Length = 0;
		for (int i = 0; i < P_Amount; i++)
			P[i] = "";
		for (int i = 0; i < RMAX; i++)
		{
			for (int j = 0; j < CMAX; j++)
			{
				FIRSTVT[i][j] = false;
			}
		}
		for (int i = 0; i < RMAX; i++)
		{
			for (int j = 0; j < CMAX; j++)
			{
				LASTVT[i][j] = false;
			}
		}
		for (int i = 0; i < NT_AMOUNT; i++)
		{
			for (int j = 0; j < NT_AMOUNT; j++)
			{
				PRIORITY_TABLE[i][j] = 2;  //默认为出错状态
			}
		}
		//初始状态下f和g内的元素都置为1
		for (int i = 0; i < T_AMOUNT; i++)
		{
			f[i] = 1; 
			g[i] = 1;
		}
	}
	~G()
	{
		
	}
};

class Temp
{
public:
	char NT;
	char T;
};

void findor(string &str, char c, int *pos,int cap,int &len)  //在str中寻找出所有|的位置,pos记录其位置,cap为位置数组的容量，len为pos的有效长度
{
	for (int i = 0; i < cap; i++)
	{
		//初始化
		pos[i] = 0;
	}
	int temp=0;
	for (int i = 0; i < str.length(); i++)
	{
		int j=str.find(c, temp);
		if (j == -1)
		{
			break;
		}
		pos[i] = j;
		len++;
		temp = j+1;
	}
}
//根据产生式得到终结符集合与非终结符集合
void getSyms(string *strs, int len, string &T,string &NT) //strs为产生式集合，len为产生式个数
{
	string strTemp="";  
	for (int i = 0; i < len; i++)
	{
		if (strTemp.find(strs[i][0]) != -1)  //若是找到已存在的符号，跳过
			continue;
		strTemp += strs[i][0];
	}
	NT = strTemp;
	for (int i = 0; i < len; i++)
	{
		for (int j = 3; j < strs[i].length(); j++)
		{
			if (strs[i][j] >= 'A' && strs[i][j] <= 'Z')  //非终结符
			{
				if (NT.find(strs[i][j]) != 0)  //该符号已存在,跳过
					continue;
				NT += strs[i][j];
			}
			else  //终结符,可能是算符或i
			{
				if (T.find(strs[i][j]) != -1)  //该符号已存在，跳过
					continue;
				T += strs[i][j];
			}
		}
	}
}

void zero(string *strs, int &len) //strs为输入的文法表达式，len为表达式的个数
{
	string StrTemp[P_Amount];
	int lens,count=0;  //count记录新生成表达式的个数,lens记录每个
	int pos[10];  
	for (int i = 0; i < len; i++)  //将所有产生式按|隔开，生成多个产生式，存储到StrTemp中
	{
		lens = 0;
		findor(strs[i], '|', pos, 10, lens);  //寻找这条表达式中所有“|”的位置，存放到pos中，10为其最大容量,lens为“|”个数

		for (int z = 0; z < lens+1; z++)  //将一个产生式按|隔开，生成多个产生式，存储到StrTemp中，(len+1)代表要生成的表达式个数
		{
			if (z == 0)
			{
				StrTemp[z + count] = strs[i].substr(0, 3) + strs[i].substr(3, (pos[z]-2 - 1));
				continue;
			}

			StrTemp[z + count] = strs[i].substr(0, 3) + strs[i].substr(pos[z - 1]+1, (pos[z] - pos[z - 1] - 1));

			if (z == lens)
			{
				StrTemp[count + lens] = strs[i].substr(0, 3) + strs[i].substr(pos[lens - 1]+1, (strs[i].length() - pos[lens - 1] - 1));
			}
		}

		count += lens + 1;
	}

	//把StrTemp赋给strs
	len = count;
	for (int i = 0; i < count; i++)
	{
		strs[i] = StrTemp[i];
	}
}
void test1(G &gm);  //声明
void first(G &grammer,string strs[],int len)  //grammer为需要构造的文法对象，strs为经过zero后的文法表达式，len为表达式的个数
{
	grammer.begin = strs[0][0];  //开始符号
	grammer.P_Length = len;  //产生式的个数
	for (int i = 0; i < len; i++)  //得到产生式
	{
		grammer.P[i] = strs[i];
	}
	getSyms(strs, len, grammer.T, grammer.NT);  //从产生式中得到非终结符集合与终结符集合

}

void test1(G &gm)  //检测函数,这里检查文法数据结构的正确性
{
	cout << endl << "检测：" << endl;
	cout << "文法的开始符号：" << gm.begin << endl;
	cout << "文法的非终结符集合：" << gm.NT << endl;
	cout << "文法的终结符集合：" << gm.T << endl;
	cout << "文法的产生式：" << endl;
	for (int i = 0; i < gm.P_Length; i++)
	{
		cout << gm.P[i] << endl;
	}
	cout << endl;
}

void insert(G &gm, stack<Temp> &stck, char A, char a)  //
{
	int iA = gm.NT.find(A);
	int ia = gm.T.find(a);
	Temp tmp;
	tmp.NT = A;
	tmp.T = a;
	
	if (!gm.FIRSTVT[iA][ia])
	{
		gm.FIRSTVT[iA][ia] = true;
		stck.push(tmp);
	}
}

void insert1(G &gm, stack<Temp> &stck, char A, char a)  //
{
	int iA = gm.NT.find(A);
	int ia = gm.T.find(a);
	Temp tmp;
	tmp.NT = A;
	tmp.T = a;

	if (!gm.LASTVT[iA][ia])
	{
		gm.LASTVT[iA][ia] = true;
		stck.push(tmp);
	}
}

void test2(G &gm);  //测试gm的FIRSTVT和LASTVT集合

void second(G &gm)  //gm为文法的四元组
{
	gm.FR = gm.NT.length();
	gm.FC = gm.T.length();
	stack<Temp> stck;  
	Temp tmpBa;

	//得到G的FIRSTVT集合
	for (int i = 0; i < gm.P_Length; i++)
	{
		if (gm.P[i].length() - 4 == 0)  //产生式右边只有一个符号
		{
			if (gm.P[i][3] < 'A' || gm.P[i][3]>'Z')  //这个符号是终结符
			{
				insert(gm, stck, gm.P[i][0], gm.P[i][3]);
			}
		}
		else if (gm.P[i][3] >= 'A' && gm.P[i][3] <= 'Z')  //右部的第一个符号为非终结符，那么加入右部的第二个符号
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][4]);
		}
		else  //右部的第一个符号为终结符
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][3]);
		}
	}

	while (!stck.empty())
	{
		tmpBa = stck.top();
		stck.pop();
		for (int i = 0; i < gm.P_Length; i++)
		{
			if (gm.P[i][0] != tmpBa.NT&&gm.P[i][3] == tmpBa.NT)  //找到形如A->B···的产生式，FIRSTVT(B)必然FIRSTVT(A)
			{
				insert(gm, stck, gm.P[i][0], tmpBa.T);
			}
		}
	}

	//得到G的LASTVT集合
	for (int i = 0; i < gm.P_Length; i++)
	{
		if (gm.P[i].length() - 4 == 0)  //产生式右边只有一个符号
		{
			if (gm.P[i][3] < 'A' || gm.P[i][3]>'Z')  //这个符号是终结符
			{
				insert1(gm, stck, gm.P[i][0], gm.P[i][3]);
			}
		}
		else if (gm.P[i][3] >= 'A' && gm.P[i][3] <= 'Z')  //右部的最后一个符号为非终结符，那么加入右部的倒数第二个符号
		{
			insert1(gm, stck, gm.P[i][0], gm.P[i][gm.P[i].length()-2]);
		}
		else  //右部的最后一个符号为终结符
		{
			insert1(gm, stck, gm.P[i][0], gm.P[i][gm.P[i].length()-1]);
		}
	}

	while (!stck.empty())
	{
		tmpBa = stck.top();
		stck.pop();
		for (int i = 0; i < gm.P_Length; i++)
		{
			if (gm.P[i][0] != tmpBa.NT&&gm.P[i][gm.P[i].length()-1] == tmpBa.NT)  //找到形如A->···B的产生式，LASTVT(B)必然LASTVT(A)
			{
				insert1(gm, stck, gm.P[i][0], tmpBa.T);
			}
		}
	}

}

void test2(G &gm)
{
	cout << "G的FIRSTVT集合：" << endl;
	cout << "\t";
	for (int i = 0; i < gm.T.length(); i++)
	{
		if (i == gm.T.length() - 1)
		{
			cout << gm.T[i] << endl;
			break;
		}
		cout << gm.T[i] << "\t";
	}

	for (int i = 0; i < gm.FR; i++)
	{
		cout << gm.NT[i] << "\t";
		for (int j = 0; j < gm.FC; j++)
		{
			if (j == gm.FC - 1)
			{
				cout << gm.FIRSTVT[i][j] << endl;
				break;
			}
			cout << gm.FIRSTVT[i][j] << "\t";
		}
	}

	cout << "G的LASTVT集合：" << endl;
	cout << "\t";
	for (int i = 0; i < gm.T.length(); i++)
	{
		if (i == gm.T.length() - 1)
		{
			cout << gm.T[i] << endl;
			break;
		}
		cout << gm.T[i] << "\t";
	}
	for (int i = 0; i < gm.FR; i++)
	{
		cout << gm.NT[i] << "\t";
		for (int j = 0; j < gm.FC; j++)
		{
			if (j == gm.FC - 1)
			{
				cout << gm.LASTVT[i][j] << endl;
				break;
			}
			cout << gm.LASTVT[i][j] << "\t";
		}
	}
	cout << endl;
}

void test3(G &gm);  //测试third函数

void third(G &gm)  //gm为文法的数据结构
{
	for (int i = 0; i < gm.P_Length; i++)
	{
		int lens = gm.P[i].length();
		for (int j = 3; j <= lens-2; j++)
		{
			if ((gm.P[i][j]<'A'||gm.P[i][j]>'Z')&&(gm.P[i][j + 1]<'A'||gm.P[i][j + 1]>'Z'))  //Xi和X(i+1)都为终结符
			{
				int temp = gm.T.find(gm.P[i][j]);
				int temp1 = gm.T.find(gm.P[i][j + 1]);
				gm.PRIORITY_TABLE[temp][temp1] = 0;  //置为同等优先级
			}
			if (j <= (lens - 3) && (gm.P[i][j]<'A' || gm.P[i][j]>'Z') && (gm.P[i][j + 2]<'A' || gm.P[i][j + 2]>'Z') && (gm.P[i][j + 1] >= 'A'&&gm.P[i][j + 1] <= 'Z'))  //Xi和X(i+2)为终结符，但X(i+1)为非终结符
			{
				int temp = gm.T.find(gm.P[i][j]);
				int temp1 = gm.T.find(gm.P[i][j + 2]);
				gm.PRIORITY_TABLE[temp][temp1] = 0;  //置为同等优先级
			}
			if ((gm.P[i][j]<'A' || gm.P[i][j]>'Z') && (gm.P[i][j + 1] >= 'A' && gm.P[i][j + 1] <= 'Z'))  //Xi为终结符而X(i+1)为非终结符
			{
				for (int z = 0; z < gm.T.length(); z++)
				{
					if (gm.FIRSTVT[gm.NT.find(gm.P[i][j + 1])][z])
					{
						int temp = gm.T.find(gm.P[i][j]);
						int temp1 = z;
						gm.PRIORITY_TABLE[temp][temp1] = -1;  //置于列优先于行
					}
				}
			}
			if ((gm.P[i][j+1]<'A' || gm.P[i][j+1]>'Z') && (gm.P[i][j] >= 'A' && gm.P[i][j] <= 'Z'))  //X(i+1)为终结符而Xi为非终结符
			{
				for (int z = 0; z < gm.T.length(); z++)
				{
					if (gm.LASTVT[gm.NT.find(gm.P[i][j])][z])
					{
						int temp = z;
						int temp1 = gm.T.find(gm.P[i][j+1]);
						gm.PRIORITY_TABLE[temp][temp1] = 1;  //置于行优先于列
					}
				}
			}
		}
	}
}

void test3(G &gm)
{
	cout << "G的优先分析表：" << endl;
	cout << "\t";
	for (int i = 0; i < gm.T.length(); i++)
	{
		if (i == gm.T.length() - 1)
		{
			cout << gm.T[i] << endl;
			break;
		}
		cout << gm.T[i] << "\t";
	}
	int lens = gm.T.length();
	for (int i = 0; i < lens; i++)
	{
		cout << gm.T[i] << "\t";
		for (int j = 0; j < lens; j++)
		{
			if (j == gm.T.length() - 1)
			{
				cout << gm.PRIORITY_TABLE[i][j] << endl;
				break;
			}
			cout << gm.PRIORITY_TABLE[i][j] << "\t";
		}
	}
}

void test4(G &gm);  //检测优先函数f和g

void four(G &gm)  //构造优先函数f和g
{
	bool isChanged = true;  //isChanged代表上一次迭代f和g函数有没有发生变化，有为true
	int lens = gm.T.length();
	while (isChanged)
	{
		isChanged = false;
		for (int i = 0; i < lens; i++)
		{
			for (int j = 0; j < lens; j++)
			{
				if (gm.PRIORITY_TABLE[i][j] == 2)
					continue;
				if (gm.PRIORITY_TABLE[i][j] == 1 && (gm.f[i] <= gm.g[j]))
				{
					gm.f[i] = gm.g[j] + 1;
					isChanged = true;
					continue;
				}
				if (gm.PRIORITY_TABLE[i][j] == -1 && (gm.f[i] >= gm.g[j]))
				{
					gm.g[j] = gm.f[i] + 1;
					isChanged = true;
					continue;
				}
				if (gm.PRIORITY_TABLE[i][j] == 0 && (gm.f[i] != gm.g[j]))
				{
					if (gm.f[i] > gm.g[j])
					{
						gm.g[j] = gm.f[i];
					}
					else
					{
						gm.f[i] = gm.g[j];
					}
					isChanged = true;
					continue;
				}
			}
		}
	}
}

void test4(G &gm)  //检测four函数
{
	cout << "\t";
	for (int i = 0; i < gm.T.length(); i++)
	{
		if (i == gm.T.length() - 1)
		{
			cout << gm.T[i] << endl;
			break;
		}
		cout << gm.T[i] << "\t";
	}
	int lens = gm.T.length();
	cout << "f函数： ";
	for (int i = 0; i < lens; i++)
	{
		if (i == lens - 1)
		{
			cout << gm.f[i] << endl;
			break;
		}
		cout << gm.f[i] << "\t";
	}
	cout << "g函数： ";
	for (int i = 0; i < lens; i++)
	{
		if (i == lens - 1)
		{
			cout << gm.g[i] << endl;
			break;
		}
		cout << gm.g[i] << "\t";
	}
}

void test5(bool b);  //检测GuiYueQi是否识别出待规约串,b为true，则说明能识别

bool GuiYueQi(G &gm, string str)  //gm为文法的数据结构，str为待规约串,能识别为true，否则为false
{
	string strStck="";  //用string对象做符号栈，方便取值
	strStck.append("#");  //初始化
	int k = 0,num=0;
	int j;  //j表示最左素短语的开头位置
	string a="";  //读入单元
	string Q = "";  //作为待审查的最左素短语的开头符号
	while (a!="#")  //到了输入串的最后一个字符，跳出
	{
		a = str[num++];  //把下一个输入符号读进a中
		if (gm.T.find(strStck[k]) != gm.T.npos)  //如果栈顶的符号为终结符
		{
			j = k;
		}
		else
		{
			j = k - 1;
		}
		while (gm.f[gm.T.find(strStck[j])] > gm.g[gm.T.find(a)])  //往下找，直到找到栈内符号优先级小于或等于当前输入符号的优先级
		{
			do
			{
				Q = strStck[j];
				if (gm.T.find(strStck[j - 1]) != gm.T.npos)  //
				{
					j = j - 1;
				}
				else
				{
					j = j - 2;
				}
			} while (gm.f[gm.T.find(strStck[j])] >= gm.g[gm.T.find(Q)]);

			//把strStck[j+1]~strStck[k]规约为某个N
			strStck.erase(strStck.begin() + j + 1, strStck.begin() + k);  //把规约的符号退栈
			strStck.append("N");  //压入某个非终结符N
			k = j + 1;
			break;
		}

		if (gm.f[gm.T.find(strStck[j])] <= gm.g[gm.T.find(a)])  
		{
			k = k + 1;
			strStck.append(a);
		}
		else
		{
			return false;
		}
	}
	

	return true;  //
}

void test5(bool b)
{
	if (b)
	{
		cout << "acc!" << endl;
	}
	else
	{
		cout << "fail!" << endl;
	}
}

int main()
{
	G gm;
	string str[10];
	int len;  //输入的表达式个数
	int i = 0;
	//从文件读入文法
	ifstream ifile("wenfa1.txt");  //打开wenfa.txt文件
	while (ifile)
	{
		ifile >> str[i++];  //读入文法
	}
	ifile.close();  //关闭文件
	len = i - 1;
	zero(str, len);  //依据‘|’，拆分表达式
	first(gm, str, len);  //得到文法数据结构
	//test1(gm);  //检查zero()、first()函数

	second(gm);  //得到每个非终结符的FIRSTVT和LASTVT集合
	//test2(gm);  //检测second函数

	third(gm);  //得到优先分析表
	//test3(gm);  //检测third函数

	four(gm);  //得到优先函数f和g
	//test4(gm);  //检测four函数

	string guiyuestr;  //规约串
	ifile.open("guiyuechuan3.txt");
	stringstream io;
	io <<ifile.rdbuf();
	io >> guiyuestr;
	cout << "待规约串为："<< guiyuestr <<endl;
	bool b = GuiYueQi(gm, guiyuestr);  //规约器，规约str
	test5(b);  //检测待规约串被规约器处理后的结果

	return 0;
}