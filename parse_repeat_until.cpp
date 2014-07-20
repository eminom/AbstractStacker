
//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

std::string Parser::parse_repeat_until(const LocalVars& context)
{
	//~ 两个point,:生成与标记
	//~ 1. 循环入口, 2.脱离口,3.until测试入口
	using namespace std;
	typedef Parser::labelClass _lc;
	MySafeString retval;
	match(KEYWORD_REPEAT);
	const string rpt_ul_entry = labelGirl.newlabel(_lc::COMPULSIVE);
	const string rpt_ul_breakpoint = labelGirl.newlabel(_lc::OPTIONAL);
	const string rpt_ul_until_test_point = labelGirl.newlabel(_lc::OPTIONAL);
	assert( !retval.size() );
	if( is_debug_on(DEBUG_REPEAT_UNTIL) )
		retval += "//~ repeat entry:" + rpt_ul_entry +"\n";
	retval += emit_safe(rpt_ul_entry,EM_LABEL);//~1.循环入口标记ed
	retval += xlist(rpt_ul_breakpoint,rpt_ul_until_test_point,context);	
	//~ 内层breakpoint,continuepoint更新
	match(KEYWORD_UNTIL);

	if( is_debug_on(DEBUG_REPEAT_UNTIL) )
		retval += "//~ until test entry(/continue):" + rpt_ul_until_test_point +"\n";
	retval += emit_safe(rpt_ul_until_test_point,EM_LABEL);//~3.until测试入口
	retval += logicor_wrapped(context);
	retval += emit_safe(rpt_ul_entry,EM_GOFALSE);
	if( is_debug_on(DEBUG_REPEAT_UNTIL) )
		retval += "//~ repeat until out(/break):" + rpt_ul_breakpoint +"\n";
	retval += emit_safe(rpt_ul_breakpoint,EM_LABEL);//~2.脱逸口标记ed
	return retval;
}