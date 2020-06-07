
#include "Thompson.h"

//主函数
void main()
{
	string Regular_Expression = "(a|b)*abb";
	cell NFA_Cell;
	Regular_Expression = "(a|b)*abb";
	//接受输入
	input(Regular_Expression);//调试需要先屏蔽
	//添加“+”，便于转后缀表达式
	Regular_Expression = add_join_symbol(Regular_Expression);
	//中缀转后缀
	Regular_Expression = postfix(Regular_Expression);
	//表达式转NFA
	NFA_Cell = express_2_NFA(Regular_Expression);
	//显示
	Display(NFA_Cell);
}
