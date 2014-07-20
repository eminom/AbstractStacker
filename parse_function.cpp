

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

using namespace std;

std::string
Parser::parse_function()
{
	MySafeString retval;
	match(KEYWORD_FUNCTION);
	const int func_name_id = rlexer.tokenval;
	const string func_name = rlexer.rsymbol.symtable[func_name_id].lexptr;
	retval += emit_safe(func_name,EM_LABEL_FUNCTION);
		//~ a new function,haha
	const int rindex = funcMan.AddFunc(func_name.c_str());
	LocalVars& my_local_context = funcMan[rindex];	//~ at() wrapped

	if( is_debug_on(DEBUG_FUNCTION) )
		retval +="//~function "+ func_name + "\n";

	match(ID);
		//~ 有一个参数列表
			/*   varlist -> var : type morevar
			   morevar -> ; varlist
			*/
	if( '(' == lookahead ){
		match('(');
		bool bStill(true);
		while( bStill )
			switch(lookahead)
			{
			case ID:
				do{
					const int id_name=rlexer.tokenval;
					const char *var_name=rlexer.rsymbol.symtable[id_name].lexptr;
					match(ID);
					my_local_context.addVar(var_name,LocalVars::IN_PARAM_LIST);
					match(':');
					match(KEYWORD_INTEGER);
				}while(0);
				break;
			case ';':
				match(';');
				break;
			case ')':
				bStill=false;
				break;
			default:
				throw exception("parsing param list errror");
				break;
			}
		match(')');
	}
	match(':');
	//~ 简单设计,类型只有一个哈
	match(KEYWORD_INTEGER);
	match(';');
	//~ now we take care of var
	if( KEYWORD_VAR == lookahead ){
		match(KEYWORD_VAR);
		bool bStill(true);
		while(bStill)
			switch(lookahead)
			{
			case ID:
				do{
					const int id_name=rlexer.tokenval;
					const char *var_name=rlexer.rsymbol.symtable[id_name].lexptr;
					match(ID);
					my_local_context.addVar(var_name,LocalVars::IN_LOCAL_AREA);
					match(':');
					match(KEYWORD_INTEGER);
				}while(0);
				break;
			case ';':
				match(';');
				break;
			case KEYWORD_BEGIN:
				bStill = false;	//~ reaches function body 
				break;
			default:
				throw exception("what ? var parsing error");
				break;
			}
	}
		//~ 在栈上开辟空间嘛
	do{
		if( !my_local_context.getLocalCount() ){
			if( is_debug_on(DEBUG_FUNCTION) )
				retval +="//~there is no additional vars for function\n";
		}
		else{
			char szbuf[BUFSIZ];
			//~ result由call来做
			if( is_debug_on(DEBUG_FUNCTION) ){
				sprintf(szbuf,"//~ preserve stack for %d local varibles\n",
					my_local_context.getLocalCount());
				retval += szbuf;
			}
			sprintf(szbuf,"%d",my_local_context.getLocalCount());
			retval += emit_safe(szbuf,EM_PRESERVESTACK);
		}
	}while(0);
	//~ var endz
		//~目前为止(function能力)唯一传递特别contex_t的地方
	retval += stmt("no_break","no_continue",my_local_context);

	//~ 平衡堆栈
	do{
		char szBuf[BUFSIZ];
		if( my_local_context.getLocalCount() > 0 ){
			if( is_debug_on(DEBUG_FUNCTION) )
				retval += "//~ balance our stack\n";
			sprintf(szBuf,"%d",my_local_context.getLocalCount());
			retval += emit_safe(szBuf,EM_DECSTACK);
		}
		else{
			if( is_debug_on(DEBUG_FUNCTION) )
				retval += "//~no local vars\n";
		}
		sprintf(szBuf,"%d",my_local_context.getParamCount());
		retval += emit_safe(szBuf,EM_RETURN);
	}while(0);

	//~ 标注函数体结束
	if( is_debug_on(DEBUG_FUNCTION) )
		retval += "//~ function "+func_name+" over\n";
	match(';');	//~ force to match this 
		//~ RVO
	return retval;
}
