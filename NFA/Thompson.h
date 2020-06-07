#ifndef THOMPSON_H
#define THOMPSON_H

#include <iostream>
#include <stdio.h>
#include <cctype>
#include <stack>
#include <string>

using namespace std;

#define MAX 100

//节点，定义成结构体，便于以后扩展
struct state
{
	string StateName;
};

//边，空转换符用'#'表示
struct edge
{
	state StartState;
	state EndState;
	char TransSymbol;
};

//NFA单元
struct cell
{
	edge EdgeSet[MAX];
	int EdgeCount;
	state StartState;
	state EndState;
};

/***************NFA的矩阵结构****************/

struct node
{
	edge* In_edges;
	edge* Out_edges;
	//state
};

/********************************************/

//函数声明
void input(string&);
int check_legal(string);
int check_character(string);
int check_parenthesis(string);
int is_letter(char);
//添加“+”，便于转后缀表达式
string add_join_symbol(string);
//中缀转后缀
string postfix(string);
//优先级 in stack priority
int isp(char);
//优先级 in coming priority
int scp(char);
//表达式转NFA
cell express_2_NFA(string);
//处理 a|b
cell do_Unite(cell, cell);
//处理 ab
cell do_Join(cell, cell);
//处理 a*
cell do_Star(cell);
//处理 a
cell do_Cell(char);
//将一个单元的边的集合复制到另一个单元
void cell_EdgeSet_Copy(cell&, cell);

//产生一个新的状态节点，便于管理
state new_StateNode();
//显示
void Display(cell);

#endif  //THOMPSON.H
