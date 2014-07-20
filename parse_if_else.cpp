

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"
	
std::string Parser::parse_if_else(const std::string brkpoint,
								  const std::string ctnpoint,
								  const LocalVars& context)
{
	using namespace std;
	typedef Parser::labelClass _lc;
	MySafeString retval;
	/* stmt ->  if
	*			logcior { out:=newlabel; emit('gofalse',out);}
	*			then
	*			stmt { emit('label',out);}
				*/			
				//~ 超前scan, 看看有没有else(如果不是else那必须是;结束)

	if( is_debug_on(DEBUG_IF) )
		retval += "//~ if (\n";
	match(KEYWORD_IF);	//~ move forward
	retval += logicor_wrapped(context);
	if( is_debug_on(DEBUG_IF) )
		retval += "//~ ) if\n";
	//~ logi_cor: the next level
	//~ 语意动作 out:=newlabel; emit('gofalse',out);}
	const string out_if_label = labelGirl.newlabel(_lc::COMPULSIVE);
	retval += emit_safe(out_if_label,EM_GOFALSE);
	match(KEYWORD_THEN);	//~ then
	retval += stmt(brkpoint,ctnpoint,context);
	//~ 语义动作延后,看看有没有else
	if( KEYWORD_ELSE != lookahead ){
		retval += emit_safe(out_if_label,EM_LABEL);
	}
	else{
		match(KEYWORD_ELSE);
		const string not_else_label = labelGirl.newlabel(_lc::COMPULSIVE);
		//` 从if true 穿越下来, 请转else的outside
		retval += emit_safe(not_else_label,EM_GOTO);
		if( is_debug_on(DEBUG_IF) )
			retval += "//~ else \n";
		retval += emit_safe(out_if_label,EM_LABEL);	//~ if false is here(else)
		//~
		retval += stmt(brkpoint,ctnpoint,context);
		retval += emit_safe(not_else_label,EM_LABEL);//~ completely out
	}
	return retval;
}