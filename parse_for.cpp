

//~ eminem7409@sina.com
//~  
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

	//~ ѡ��:������?��ѡ����ȷ��lvalue/localeftv
#define PLACE_ID_LEFT_V()	\
	if( context.isNull() ){\
		retval += emit(ID,id_now,rsymbol,LVALUE);\
	}\
	else{\
		rindex = context.getIndexOfVar(vname.c_str());\
		char szIndex[BUFSIZ];\
		sprintf(szIndex,"#%d //:%s",rindex,vname.c_str());\
		retval += emit_safe(szIndex,EM_LOCAL_LV);\
	}\
	//~

	//~ ѡ����ȷ��rvalue��locarightv
#define PLACE_ID_RIGHT_V()	\
	if( context.isNull() ){\
		retval += emit(ID,id_now,rsymbol,RVALUE);\
		retval += emit_safe(string(),EM_ZEROWAX);\
		retval += emit_safe(string(),EM_ADJUST_RVALUE);\
	}\
	else{\
		rindex = context.getIndexOfVar(vname.c_str());\
		char szIndex[BUFSIZ];\
		sprintf(szIndex,"#%d //:%s",rindex,vname.c_str());\
		retval += emit_safe(szIndex,EM_LOCAL_RV);\
	}\
	//~

std::string Parser::parse_for(const LocalVars &context)
{

	/* �����Ƶ�:
	stmt ->for id:=lower to upper do stmt
	  ����2��.2oo7:
	  �ر�˵��:Pascal��for��C�Ĳ�һ����,һ��for��,
	  �½���Ͻ��ֵֻ�ڳ�ʼ����ʱ��evaluate,(֮ǰ���Լ�Ҳ�Թ�,����,
	  ���쿴�����ֿ����˹������������,����,����ע�⵽��,
	  �˿�����Ҫ�޸�for�Ա�������function������(�������function���Ϊbug)
	  Current Goal: (lower��upper��û������,����ֻ�ǵ���id)
	 */

	using namespace std;
	MySafeString retval;
	typedef Parser::labelClass _lc;

	const string test_for_entry = labelGirl.newlabel(_lc::COMPULSIVE);
	const string next_for_entry = labelGirl.newlabel(_lc::OPTIONAL);
	const string good_for_out   = labelGirl.newlabel(_lc::COMPULSIVE);
	const string bad_for_out	= labelGirl.newlabel(_lc::COMPULSIVE);
	const string for_breakpoint = labelGirl.newlabel(_lc::OPTIONAL);
	const string for_out_final=labelGirl.newlabel(_lc::COMPULSIVE);

	if( is_debug_on(DEBUG_FOR) )
		retval +="//~ for start...\n";
	match(KEYWORD_FOR);
	const int id_now = rlexer.tokenval;
	const string vname( rlexer.rsymbol.symtable[id_now].lexptr);
	int rindex(-1);

	match(ID);
	match(ASSIGN_TO);
	retval += logicor_wrapped(context);
	retval += emit_safe(string(),EM_PUSHWAX);
	match(KEYWORD_TO);
	retval += logicor_wrapped(context);
	retval += emit_safe(string(),EM_POPWBX);	//~ stack balanced

	match(KEYWORD_DO);	//~
		//~ now val2 in wax, val1 in wbx
	//~��ջ
	retval += emit_safe(string(),EM_PUSHWAX);
	retval += emit_safe(string(),EM_PUSHWBX);
	retval += emit_safe(string(),EM_PUSHWBX);
	retval += emit_safe(string(),EM_PUSHWAX);
			/* stack now:
					top ->		val2
								val1
								val1
								val2
								bottom */
		//~<= if the range is valid
	retval += emit(LESS_EQUAL,NONE,rsymbol);
	retval += emit_safe(string(),EM_POPWAX);
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for range checking\n";
			/* stack now:
					top ->      val1
								val2
								bottom*/
	retval += emit_safe(bad_for_out,EM_GOFALSE);	//~ bad_for_out����ƽ��stack

	//~ ����ֵ
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for initialization\n";
	retval += emit_safe(string(),EM_POPWAX);
	PLACE_ID_LEFT_V()
	retval += emit(ASSIGN_TO,NONE,rsymbol);

	//~ �������
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for test entry:"+test_for_entry+"\n";
	retval += emit_safe(test_for_entry,EM_LABEL);
	retval += emit_safe(string(),EM_POPWBX);		//~��ʱ��ջƽ��

	//~ ����ȷ�ķ���id����ֵ
	PLACE_ID_RIGHT_V()
	retval += emit_safe(string(),EM_PUSHWBX);
	retval += emit('>',NONE,rsymbol);
	retval += emit_safe(string(),EM_POPWAX);
	retval += emit_safe(good_for_out,EM_GOTRUE);	//~ out, and stack��ƽ���
	retval += emit_safe(string(),EM_PUSHWBX);
	retval += stmt(for_breakpoint,next_for_entry,context);

	//~ NEXT���
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ next for(/continue):" + next_for_entry +"\n";
	retval += emit_safe(next_for_entry,EM_LABEL);
	//~ ��1Ȼ��������ͷ
	PLACE_ID_LEFT_V()
	PLACE_ID_RIGHT_V()
	retval += emit(NUM,1,rsymbol);	//~ push 1
	retval += emit('+',NONE,rsymbol);//+
	retval += emit_safe(string(),EM_POPWAX);
	retval += emit(ASSIGN_TO,NONE,rsymbol);
	retval += emit_safe(test_for_entry,EM_GOTO);

	//~ good for out:
	//~ ���ʱ��id��ֵ�պñ�upper��1,����Borland Pascal��˵��
	//~ ����Ҽ���1�űȽϽӽ���
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for normal termination:"+good_for_out+"\n";
	retval += emit_safe(good_for_out,EM_LABEL);
	PLACE_ID_LEFT_V()
	PLACE_ID_RIGHT_V()
	retval += emit(NUM,1,rsymbol);
	retval += emit('-',NONE,rsymbol);
	retval += emit_safe(string(),EM_POPWAX);
	retval += emit(ASSIGN_TO,NONE,rsymbol);
	retval += emit_safe(for_out_final,EM_GOTO);//~ת����

	//~ bad for out:
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for with invalid range, out:"+bad_for_out+"\n";
	retval += emit_safe(bad_for_out,EM_LABEL);
	retval += emit_safe(string(),EM_POPWBX);
	retval += emit_safe(string(),EM_POPWBX);
	retval += emit_safe(for_out_final,EM_GOTO);//~ת����

	//~ for break SPECIAL
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ break for:"+for_breakpoint+"\n";
	retval += emit_safe(for_breakpoint,EM_LABEL);
	retval += emit_safe(string(),EM_POPWBX);	//~ break����Ҫƽ��ջ
	retval += emit_safe(for_out_final,EM_GOTO);

	//~ final out
	if( is_debug_on(DEBUG_FOR) )
		retval += "//~ for over which start with test entry:"+test_for_entry+"\n";
	retval += emit_safe(for_out_final,EM_LABEL);

	return retval;
}