

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
Parser::
parse_case_of(const std::string brk_point,
			  const std::string continue_point,
			  const LocalVars&context)
{
	/*
	每一个case的入口都是NUM: 如果NUM前面还有一个NUM
	则必须用一个;来分离;(这里是除了xlist之外唯一一处还要匹配;的地方)

  //~与Pascal的区别
	case of end里面可以为空
	case的end之前可以没有;
	*/

	typedef Parser::labelClass _lc;
	MySafeString retval;
	match(KEYWORD_CASE);
	//~ and now it is an id
	const int id_now = rlexer.tokenval;
	const string vname = rlexer.rsymbol.symtable[id_now].lexptr;
	char szIndex[BUFSIZ];

	match(ID);
	match(KEYWORD_OF);
	//~ ok la, i can handle this
		//~case id of 寓意至蹈

	if( is_debug_on(DEBUG_CASE_OF) ){
		char szBuffer[BUFSIZ];
		sprintf(szBuffer,"//~case %s of \n",rlexer.rsymbol.symtable[id_now].lexptr);
		retval += szBuffer;
	}


	const string case_exit=labelGirl.newlabel(0);
	string next_case_label = labelGirl.newlabel(_lc::COMPULSIVE);
	retval+= emit_safe(next_case_label,EM_GOTO);

	bool bActive(true);
	while(bActive){
		switch(lookahead)
		{
		case NUM:
			do{
				int nVal = rlexer.tokenval;
				if( is_debug_on(DEBUG_CASE_OF) ){
					char msgBuf[BUFSIZ];
					sprintf(msgBuf,"//~case of %d:\n",nVal);
					retval += msgBuf;
				}
				match(NUM);
				match(':');
				retval += emit_safe(next_case_label,EM_LABEL);
				next_case_label = labelGirl.newlabel(_lc::COMPULSIVE);
				
				if(context.isNull()){
					retval += emit(ID,id_now,rsymbol,RVALUE);
					retval += emit_safe(string(),EM_ZEROWAX);
					retval += emit_safe(string(),EM_ADJUST_RVALUE);
				}
				else{
					//~ 在栈上诶!
					int rindex = context.getIndexOfVar(vname.c_str());
					sprintf(szIndex,"#%d //:%s",rindex,vname.c_str());
					retval += emit_safe(szIndex,EM_LOCAL_RV);
				}

				retval += emit(NUM,nVal,rsymbol);
				retval += emit('=',NONE,rsymbol);
				retval += emit_safe(string(),EM_POPWAX);
				retval += emit_safe(next_case_label,EM_GOFALSE);
				//~ one stmt instead of one xlist
				retval += stmt(brk_point,continue_point,context);
				retval += emit_safe(case_exit,EM_GOTO);
				if( lookahead != KEYWORD_END )
					match(';');	//~
			}while(0);
			break;
		case KEYWORD_END:
			match(KEYWORD_END);
			if( is_debug_on(DEBUG_CASE_OF) )
				retval += "//~ the last next case label:"+next_case_label+"\n";
			retval += emit_safe(next_case_label,EM_LABEL);
			if( is_debug_on(DEBUG_CASE_OF) )
				retval += "//~case final:"+case_exit+"\n";
			retval += emit_safe(case_exit,EM_LABEL);
			bActive = false;
			break;
		default:
			match(NUM);//~ it forces an syntax error
			break;
		}
	}
	return retval;
}
