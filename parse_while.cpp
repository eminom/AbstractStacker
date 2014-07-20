

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

std::string Parser::parse_while(const LocalVars& context)
{
	/* stmt ->	while	{ test:=newlabel; emit('label',test);}
	*			logi_cor	{ out:=newlabel; emit('gofalse',out);}
	*			do		
	*			stmt1   {	emit('goto',test);
	*						emit('label',out);}
	*/
	using namespace std;
	typedef Parser::labelClass _lc;
	MySafeString retval;
	match(KEYWORD_WHILE);
	const string while_test_label = labelGirl.newlabel(_lc::COMPULSIVE|_lc::OPTIONAL);
	retval += emit_safe(while_test_label,EM_LABEL);//~1.测试入口标记ed
	if( is_debug_on(DEBUG_WHILE) )
		retval += "//~while test entry(/continue):" + while_test_label +"(\n";
	retval += logicor_wrapped(context);	//~ logi_cor
	if( is_debug_on(DEBUG_WHILE) )
		retval += "//~ ) while test entry\n";

	const string while_out_label = labelGirl.newlabel(_lc::COMPULSIVE|_lc::OPTIONAL);
	retval += emit_safe(while_out_label,EM_GOFALSE);
	
	match(KEYWORD_DO);
	retval += stmt(while_out_label,while_test_label,context);
	//~ breakpoint替身,continuepoint替身
	retval += emit_safe(while_test_label,EM_GOTO);
	if( is_debug_on(DEBUG_WHILE) )
		retval += "//~while end(/break):" + while_out_label +"\n";
	retval += emit_safe(while_out_label,EM_LABEL);//~2.脱逸口标记ed
	return retval;
}