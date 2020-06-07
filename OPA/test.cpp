#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
using namespace std;

/************宏定义区****************/
#define P_Amount 20  //假定该文法展开后的产生式最多为20个
#define RMAX 20  //假定FIRSTVT和LASTVT集中行上非终结符的个数最多为20个
#define CMAX 20  //假定FIRSTVT和LASTVT集中列上终结符的个数最多为20个
#define T_AMOUNT 20   //假定分析的文法的终结符最多为20个
/************类声明区****************/
class G;
class Temp;
/************函数声明区**************/
void test1(G &);  //检测zero和first函数
void test2(G &, bool FIRSTVT[RMAX][CMAX], bool LASTVT[RMAX][CMAX], int &, int &);  //检测second函数
void test3(G &, int PRIORITY_TABLE[T_AMOUNT][T_AMOUNT]);  //检测third函数
void test4(G &);  //检测four函数
void test5(bool );  //检测GuiYueQi是否识别出待规约串,b为true，则说明能识别
/*
void findor(string &, char, int *, int, int &);  //在str中寻找出所有|的位置,pos记录其位置,cap为位置数组的容量，len为pos的有效长度
void getSyms(string *, int , string &, string &);  //根据产生式得到终结符集合与非终结符集合
void zero(string *, int &);  //展开输入的表达式
void first(G &, string, int);  //初始化文法的数据结构
void second(G &, bool, bool, int &, int &);  //得到每个非终结符的FIRSTVT和LASTVT集合
void third(G &, int, bool, bool);  //得到算符优先分析表
void four(G &, int);  //构造文法的优先函数f和g
bool GuiYueQi(G &, string );  //规约器
*/
//文法结构体
class G
{
public:
	string T;  //终结符的集合
	string NT;  //非终结符的集合
	string begin;  //文法开始符号
	string P[P_Amount];  //产生式的集合
	int P_Length;  //产生式的个数
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
	int temp=0;  //表明每次查找的出发点
	for (int i = 0; i < str.length(); i++)
	{
		int j=str.find(c, temp);
		if (j == str.npos)  //已经找完了所有的‘|’
		{
			break;
		}
		pos[i] = j;  //记录每个‘|’的位置
		len++;  //记录pos数组的有效长度
		temp = j+1; //更新出发点
	}
}
//根据产生式得到终结符集合与非终结符集合
void getSyms(string *strs, int len, string &T,string &NT) //strs为产生式集合，len为产生式个数
{
	string strTemp="";  
	for (int i = 0; i < len; i++)  //先在每个产生式的首部找非终结符
	{
		if (strTemp.find(strs[i][0]) != -1)  //若是找到已存在的符号，跳过
			continue;
		strTemp += strs[i][0];
	}
	NT = strTemp;

	for (int i = 0; i < len; i++)  //循环每个产生式，寻找非终结符与终结符集合
	{
		for (int j = 3; j < strs[i].length(); j++)  //从每个产生式的第四位开始，因为首部已经存入非终结符集合中去了，而第三位与第四位是“->”，不必讨论
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
	int lens,count=0;  //count记录新生成表达式的个数,lens记录每个产生式中‘|’的个数
	int pos[10];  //记录每个产生式中的‘|’的位置
	for (int i = 0; i < len; i++)  //将所有产生式按|隔开，生成多个产生式，存储到StrTemp中
	{
		lens = 0;
		findor(strs[i], '|', pos, 10, lens);  //寻找这条表达式中所有“|”的位置，存放到pos中，10为其最大容量,lens为“|”个数

		for (int z = 0; z < lens+1; z++)  //将一个产生式按|隔开，生成多个产生式，存储到StrTemp中，(lens+1)代表每个原产生式要生成新表达式的个数
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

		count += lens + 1;  //更新下一个产生式要产生的新产生式组在StrTemp的存储起始位置
	}

	//把StrTemp赋给strs
	len = count;
	for (int i = 0; i < count; i++)
	{
		strs[i] = StrTemp[i];
	}
}

void first(G &grammer,string strs[],int len)  //grammer为需要构造的文法对象，strs为经过zero函数后的文法表达式，len为表达式的个数
{
	//进行文法数据结构的基本初始化工作
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

void insert(G &gm, stack<Temp> &stck, char A, char a, bool VT[RMAX][CMAX])  //不分VT为FIRSTVT和LASTVT集合，栈不同自然使用的集合也就不同
{
	int iA = gm.NT.find(A);
	int ia = gm.T.find(a);
	Temp tmp;
	tmp.NT = A;
	tmp.T = a;

	if (!VT[iA][ia])
	{
		VT[iA][ia] = true;
		stck.push(tmp);
	}
}

void second(G &gm,bool FIRSTVT[RMAX][CMAX],bool LASTVT[RMAX][CMAX],int &FR,int &FC)  //gm为文法的四元组
{
	FR = gm.NT.length();
	FC = gm.T.length();
	stack<Temp> stck;
	Temp tmpBa;

	//得到G的FIRSTVT集合
	for (int i = 0; i < gm.P_Length; i++)
	{
		if (gm.P[i].length() - 4 == 0)  //产生式右边只有一个符号
		{
			if (gm.P[i][3] < 'A' || gm.P[i][3]>'Z')  //这个符号是终结符
			{
				insert(gm, stck, gm.P[i][0], gm.P[i][3], FIRSTVT);
			}
		}
		else if (gm.P[i][3] >= 'A' && gm.P[i][3] <= 'Z')  //右部的第一个符号为非终结符，那么加入右部的第二个符号
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][4], FIRSTVT);
		}
		else  //右部的第一个符号为终结符
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][3], FIRSTVT);
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
				insert(gm, stck, gm.P[i][0], tmpBa.T, FIRSTVT);
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
				insert(gm, stck, gm.P[i][0], gm.P[i][3], LASTVT);
			}
		}
		else if (gm.P[i][3] >= 'A' && gm.P[i][3] <= 'Z')  //右部的最后一个符号为非终结符，那么加入右部的倒数第二个符号
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][gm.P[i].length()-2], LASTVT);
		}
		else  //右部的最后一个符号为终结符
		{
			insert(gm, stck, gm.P[i][0], gm.P[i][gm.P[i].length()-1], LASTVT);
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
				insert(gm, stck, gm.P[i][0], tmpBa.T, LASTVT);
			}
		}
	}

}

void test2(G &gm, bool FIRSTVT[RMAX][CMAX], bool LASTVT[RMAX][CMAX], int &FR, int &FC)
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

	for (int i = 0; i < FR; i++)
	{
		cout << gm.NT[i] << "\t";
		for (int j = 0; j < FC; j++)
		{
			if (j == FC - 1)
			{
				cout << FIRSTVT[i][j] << endl;
				break;
			}
			cout << FIRSTVT[i][j] << "\t";
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
	for (int i = 0; i < FR; i++)
	{
		cout << gm.NT[i] << "\t";
		for (int j = 0; j < FC; j++)
		{
			if (j == FC - 1)
			{
				cout << LASTVT[i][j] << endl;
				break;
			}
			cout << LASTVT[i][j] << "\t";
		}
	}
	cout << endl;
}

void third(G &gm, int PRIORITY_TABLE[T_AMOUNT][T_AMOUNT], bool FIRSTVT[RMAX][CMAX], bool LASTVT[RMAX][CMAX])  //gm为文法的数据结构
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
				PRIORITY_TABLE[temp][temp1] = 0;  //置为同等优先级
			}
			if (j <= (lens - 3) && (gm.P[i][j]<'A' || gm.P[i][j]>'Z') && (gm.P[i][j + 2]<'A' || gm.P[i][j + 2]>'Z') && (gm.P[i][j + 1] >= 'A'&&gm.P[i][j + 1] <= 'Z'))  //Xi和X(i+2)为终结符，但X(i+1)为非终结符
			{
				int temp = gm.T.find(gm.P[i][j]);
				int temp1 = gm.T.find(gm.P[i][j + 2]);
				PRIORITY_TABLE[temp][temp1] = 0;  //置为同等优先级
			}
			if ((gm.P[i][j]<'A' || gm.P[i][j]>'Z') && (gm.P[i][j + 1] >= 'A' && gm.P[i][j + 1] <= 'Z'))  //Xi为终结符而X(i+1)为非终结符
			{
				for (int z = 0; z < gm.T.length(); z++)
				{
					if (FIRSTVT[gm.NT.find(gm.P[i][j + 1])][z])
					{
						int temp = gm.T.find(gm.P[i][j]);
						int temp1 = z;
						PRIORITY_TABLE[temp][temp1] = -1;  //置于列优先于行
					}
				}
			}
			if ((gm.P[i][j+1]<'A' || gm.P[i][j+1]>'Z') && (gm.P[i][j] >= 'A' && gm.P[i][j] <= 'Z'))  //X(i+1)为终结符而Xi为非终结符
			{
				for (int z = 0; z < gm.T.length(); z++)
				{
					if (LASTVT[gm.NT.find(gm.P[i][j])][z])
					{
						int temp = z;
						int temp1 = gm.T.find(gm.P[i][j+1]);
						PRIORITY_TABLE[temp][temp1] = 1;  //置于行优先于列
					}
				}
			}
		}
	}
}

void test3(G &gm, int PRIORITY_TABLE[T_AMOUNT][T_AMOUNT])
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
				cout << PRIORITY_TABLE[i][j] << endl;
				break;
			}
			cout << PRIORITY_TABLE[i][j] << "\t";
		}
	}
}

void four(G &gm, int PRIORITY_TABLE[T_AMOUNT][T_AMOUNT])  //构造优先函数f和g
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
				if (PRIORITY_TABLE[i][j] == 2)
					continue;
				if (PRIORITY_TABLE[i][j] == 1 && (gm.f[i] <= gm.g[j]))
				{
					gm.f[i] = gm.g[j] + 1;
					isChanged = true;
					continue;
				}
				if (PRIORITY_TABLE[i][j] == -1 && (gm.f[i] >= gm.g[j]))
				{
					gm.g[j] = gm.f[i] + 1;
					isChanged = true;
					continue;
				}
				if (PRIORITY_TABLE[i][j] == 0 && (gm.f[i] != gm.g[j]))
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
	cout << endl;
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
	cout << endl;
	if (b)
	{
		cout << "acc!" << endl;
	}
	else
	{
		cout << "fail!" << endl;
	}
}

void OPGPT(G &gm, string str[], int &len)  //gm为文法数据结构，str为输入的文法产生式，len为产生式的个数
{
	//下面定义的都是局部变量，在本过程结束后销毁，节省空间
	bool FIRSTVT[RMAX][CMAX];  //非终结符的FIRSTVT集合
	bool LASTVT[RMAX][CMAX];  //非终结符的LASTVT集合
	int PRIORITY_TABLE[T_AMOUNT][T_AMOUNT];  //优先表，表内元素只有4种类型的值，-1代表行<列，1代表行>列，0代表行=列，2代表出错
	int FR;  //记录F的有效行数
	int FC;  //记录F的有效列数
	//FIRSTVT和LASTVT集合初始化
	for (int i = 0; i < RMAX; i++)
	{
		for (int j = 0; j < CMAX; j++)
		{
			FIRSTVT[i][j] = false;
			LASTVT[i][j] = false;
		}
	}
	//优先分析表的初始化
	for (int i = 0; i < T_AMOUNT; i++)
	{
		for (int j = 0; j < T_AMOUNT; j++)
		{
			PRIORITY_TABLE[i][j] = 2;  //默认为出错状态
		}
	}

	zero(str, len);  //依据‘|’，拆分表达式   
	first(gm, str, len);  //得到文法数据结构
	test1(gm);  //检查zero()、first()函数

	second(gm,FIRSTVT,LASTVT,FR,FC);  //得到每个非终结符的FIRSTVT和LASTVT集合
	test2(gm,FIRSTVT,LASTVT,FR,FC);  //检测second函数

	third(gm, PRIORITY_TABLE, FIRSTVT, LASTVT);  //得到优先分析表
	test3(gm,PRIORITY_TABLE);  //检测third函数

	four(gm, PRIORITY_TABLE);  //得到优先函数f和g
	test4(gm);  //检测four函数
}

void OPGR(G &gm, string wenfaFilepath, string strRdceFilepath)  //gm为创建的文法数据结构，filepath为输入的文法文件路径,strRdceFilepath为待规约的输入串文件路径
{
	string str[10];  //产生式组
	int len;  //输入的表达式个数
	int i = 0;
	string guiyuestr;  //规约串
	//从文件读入文法
	ifstream ifile(wenfaFilepath);  //打开文法文件
	while (ifile)
	{
		ifile >> str[i++];  //读入该文法的产生式
	}
	ifile.close();  //关闭文件
	len = i - 1; 

	OPGPT(gm, str, len);  //根据输入的文法构建算符优先文法分析表，进而得到gm的优先函数
	
	//输入规约串，在语法分析器中规约
	ifile.open(strRdceFilepath);
	stringstream io;
	io << ifile.rdbuf();
	io >> guiyuestr;
	cout << "待规约串为：" << guiyuestr << endl;

	bool b = GuiYueQi(gm, guiyuestr);  //规约器，规约str
	test5(b);  //检测待规约串被规约器处理后的结果
}

int main()
{
	G gm;  //文法的数据结构
	string wenfaFilepath = "wenfa2.txt";
	string strRdceFilepath = "guiyuechuan1.txt";
	OPGR(gm, wenfaFilepath, strRdceFilepath);  	//OPGR为算符优先文法规约函数
	return 0;
}