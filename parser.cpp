

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

using namespace std;

bool Parser::is_debug_on(int debug_sel)const
{
	if( debug_sel < 0 || debug_sel >= debug_bits.size() )
		throw exception("debug bit query error");
	return debug_bits[debug_sel];
}

void Parser::set_debug_mode(int debug_sel)
{
	try{
		debug_bits.set(debug_sel);
	}
	catch(out_of_range &){
		throw exception("out of range when set debug mode");
	}
};


//~
std::string 
Parser::
emit_safe(const std::string& str,int prefix,int label_plus)const
{//~ in progress
	static const char* szPrefix[]={
		"gofalse ",
		"gotrue ",
		"goto ",
		"label ",
		"popwax ",	//~ EM_POPWAX
		"popwbx ",	//~
		"pushwax ",
		"pushwbx ",
		"zerowax ",		//~ new on Feb19th.2oo7
		"adjustlvalue ",
		"adjustrvalue ",
		"call ",		//~ new on Feb28th.2oo7
		"locarightv ",		//~;
		"localeftv ",		//~;
		"decstack ",		//~;
		"return ",			//~;
		"prestack ",		//~;
		"labelfunc ",		//~;
	};
	if( sizeof(szPrefix)/sizeof(szPrefix[0]) != EM_COUNT)
		throw exception("emit_safe symbol list error");
	switch(prefix)
	{
	case EM_GOFALSE:
	case EM_GOTRUE:
	case EM_GOTO:
		if( !labelGirl.isLabelValid(str) )
			throw exception("invalid label");
		labelGirl.visitLabel(label_plus,str);
		break;
	case EM_LABEL:
		labelGirl.virgin_mark(str);
		break;
		//~ ones of no more action
	case EM_PUSHWAX:
	case EM_PUSHWBX:
	case EM_POPWBX:
	case EM_POPWAX:
	case EM_ZEROWAX:
	case EM_ADJUST_LVALUE:
	case EM_ADJUST_RVALUE:
	case EM_CALL:
	case EM_LOCAL_RV:
	case EM_LOCAL_LV:
	case EM_DECSTACK:
	case EM_RETURN:
	case EM_PRESERVESTACK:
	case EM_LABEL_FUNCTION:
		if( EM_LOCAL_RV == prefix ||
			EM_LOCAL_LV == prefix ){
			if( '#' != str.at(0) )
				throw exception("loca rv/lv offset label error");
		}
		break;
	default:
		throw exception("invalid prefix for emit_safe");
		break;
	}
	return szPrefix[prefix]+str+'\n';
}

bool Parser::initCallerChain()
{
	callerChain["stmt"]		= &Parser::logicor_wrapped;
	callerChain["logicor"]	= &Parser::logicabig;
	callerChain["logicabig"]= &Parser::prexpr;
	callerChain["prexpr"]	= &Parser::expr;
	callerChain["expr"]		= &Parser::term;
	callerChain["term"]		= &Parser::factor;
	callerChain["factor"]	= 0;

	map<string,NextLevel>::const_iterator p1;
	map<string,NextLevel>::const_iterator p2;
	for(p1=callerChain.begin();p1!=callerChain.end();++p1)
		for(p2=p1;p2!=callerChain.end();++p2)
			if( p1 != p2 &&	p1->second == p2->second )
				throw exception("fatal error:caller chain corrupted!");
	return true;
}

Parser::Parser(const char *outputName,
			   Lexer &lexer,
			   Symbol &symbol):
	output(outputName),
	rlexer(lexer),
	rsymbol(symbol)
{
	initCallerChain();
	if( !input )
		throw exception("can not open input stream");
	if( !output )
		throw exception("can not open output stream");
}

void Parser::parse()
{
	string main_entri = labelGirl.newlabel(labelClass::COMPULSIVE);
	output<<emit_safe(main_entri,EM_GOTO);

	//~
	lookahead = rlexer.lexan();
	bool bPntMatched = false;
	while( lookahead != DONE ){
		switch( lookahead )
		{
		case KEYWORD_FUNCTION:
			output<<parse_function();
			break;
		default:
				//~ 二月二十八日:xlist不规范的,用stmt来标注主函数体
			output<<"//~main body >>"<<endl;
			output<<emit_safe(main_entri,EM_LABEL);
			output<<stmt("no_break","no_continue",funcMan.nullContext());	//~ the day after Valentine
			if( '.' == lookahead){
				prematch('.');
				bPntMatched = true;
				if( DONE != lookahead ){
					errorx("more statement there",rlexer.lineCount());
				}
			}
			else{
				errorx(". is needed to end this source",rlexer.lineCount());
			}
			break;
		}
	}
	if( !bPntMatched )
		errorx("can not locate end of source",rlexer.lineCount());
}

string Parser::xlist(const std::string breakpoint,
					 const std::string continuepoint,
					 const LocalVars& context)
{
	//~ 语义制导翻译模式
	//~ xlist是为了与STL里的list区别哈
	/*
		block -> begin xlist end (这后头没得;,不信就看Pascal嘛!嘿嘿)
				| dummy
		xlist -> stmt; xlist
				| dummy
		消归:
		block		-> begin xlist end
		xlist		-> stmt morelist
		morelist	->  ; stmt morelist
					  | dummy

		stmt -> .... 往下看!
		     | begin xlist end
			 | dummy (这点非常非常重要的哈)

			 //~ 我裹我裹~
	*/
	/*(不对应刚才那个往下看)
	 *		xlist-> stmt morelist
	 *		morelist-> ; xlist morelist
	 *				| dummy		(这没有多余的动作了,rv会很长...)
	 *
	 */
	//~ heyyy,be careful


	/*标记可选label,be careful
		如果当前label有效的话
	*/
	if( labelGirl.isLabelValid(breakpoint) )
		labelGirl.visitLabel(labelClass::OPTIONAL,breakpoint);
	if( labelGirl.isLabelValid(continuepoint) )
		labelGirl.visitLabel(labelClass::OPTIONAL,continuepoint);
	//~
	
	MySafeString retval(stmt(breakpoint,continuepoint,context));
	bool bActive(true);
	while(bActive)
		switch(lookahead)
		{
		case ';':	//~ 因为在这里匹配了; 所以lookahead可能转移至DONE
			match(';');	//~ xlist 里面只能carry on passing them
			retval += stmt(breakpoint,continuepoint,context);
			break;
		case '.':
		case KEYWORD_END:
		case KEYWORD_UNTIL:
		case DONE:
			bActive=false;
			break;
		default:
			errorx("syntax error",rlexer.lineCount());
			break;
		}

	retval+="\n";
	return retval;
}

string Parser::stmt(const std::string breakpoint,
					const std::string continuepoint,
					const LocalVars& context)
{
	/*
	//~ stmt -> id := logi_cor							[done][01]
	//~		|	stmt不会再产生logi_cor
	//~		|	xdim id num								[done][02]
	//~		|	if logi_cor then stmt					[done][03]
	//~		|	if logi_cor then stmt else stmt			[done][04]
	//~		|	begin xlist end							[done][06][重要]
	//~		|	break for				[done]				  [09]
				break while				[done]
				break repeat until		[done]
			|	continue for			[done]				  [10]
				continue while			[done]
				continue repeat until	[done]
	//~		|	while logi_cor do stmt					[done][05]
	//~		|for id:= digit_begin to digit_end do stmt	[done][07]
	//~		|   repeat xlist end 参考第6条				[done][08]
		二千零七年二月十七特别版本
	  //~	|	case id of num1: ... num(n) stmt(n) end	
	//~		|   dummy (可以为空野)					[done]
	//~  小帕斯卡的输出语句等等, 可以在这做撒!
	//~			这~
	*/

	//~ preprocesssss
	/*标记可选label,be careful
		如果当前label有效的话
	*/
	if( labelGirl.isLabelValid(breakpoint) )
		labelGirl.visitLabel(labelClass::OPTIONAL,breakpoint);
	if( labelGirl.isLabelValid(continuepoint) )
		labelGirl.visitLabel(labelClass::OPTIONAL,continuepoint);
	//~

	NextLevel fptrNext = callerChain["stmt"];
	if( !fptrNext )
		throw exception("caller chain corrupted");

	MySafeString retval;
	bool bActive(true);
	while(bActive)
		switch(lookahead)
		{
		case KEYWORD_CASE:
			assert(!retval.size());
			retval += parse_case_of(breakpoint,continuepoint,context);
			bActive = false;
			break;
		case KEYWORD_CONTINUE:
			match(KEYWORD_CONTINUE);
			assert( !retval.size() );
			if( is_debug_on(DEBUG_CONTINUE) )
				retval += "//~ continue\n";
			retval += emit_safe(continuepoint,EM_GOTO,labelClass::OPTIONAL);
			bActive = false;
			break;
		case KEYWORD_BREAK:
			match(KEYWORD_BREAK);	//~ don't forget MATCHING!
			assert( !retval.size() );
			if( is_debug_on(DEBUG_BREAK) )
				retval += "//~ break\n";
			retval += emit_safe(breakpoint,EM_GOTO,labelClass::OPTIONAL);
			bActive = false;
			break;
		case KEYWORD_BEGIN:
				//~ match begin xlist end
			match(KEYWORD_BEGIN);
			retval.SafeAssign( xlist(breakpoint,continuepoint,context));
			match(KEYWORD_END);
			bActive = false;	//~ done
			break;
		case KEYWORD_REPEAT:
			assert( !retval.size());
			retval += parse_repeat_until(context);
			bActive = false;
			break;
		case KEYWORD_XDIM:
			do{
				match(KEYWORD_XDIM);
				const int id_now = rlexer.tokenval;
				match(ID);
				const int sz = rlexer.tokenval;
				match(NUM);
				retval += emit(KEYWORD_XDIM,id_now,rsymbol,sz);
					//~ sz will be passed as the mode 
					//` but it will be reinterpreted!
			}while(0);
			bActive = false;//~ ok, it's done
			break;
		case ID:	//~ pre...(but it will never back off!)
			if( context.isNull() ){
				retval.SafeAssign(emit(ID,rlexer.tokenval,rsymbol,LVALUE));
			}
			else{
				const int id_index = rlexer.tokenval;
				const string vname( rlexer.rsymbol.symtable[id_index].lexptr );
				char szIndex[BUFSIZ];
				const int rindex = context.getIndexOfVar(vname.c_str());
				sprintf(szIndex,"#%d //:%s",rindex,vname.c_str());
				retval += emit_safe(szIndex,EM_LOCAL_LV);
			}

			//~
			match(ID);
			switch( lookahead )
			{
			case ASSIGN_TO:
					//~ ok, next, there is no need to adjust lvalue
				break;
			case '[':
				match('[');
				retval += (this->*fptrNext)(context);
				match(']');
				retval += emit_safe(string(),EM_ADJUST_LVALUE);
				if( lookahead != ASSIGN_TO )
					match(ASSIGN_TO);	//~ force an error
				break;	//~ ok, must be an assignment next
			default:
				match(ASSIGN_TO);	//~ force an error
				break;
			}
			break;
		case ASSIGN_TO://~ matched: stmt -> id := expr
			//~ stmt -> id  emit('lvalue',id);
			//			:= 
			//~			logi_cor emit(':=');
			assert( retval.size() > 0 );
			match(ASSIGN_TO);	//~ move on
			retval += (this->*fptrNext)(context);
			retval += emit(ASSIGN_TO,NONE,rsymbol);
			bActive = false;		//~ matched , and it's been processed. 
			break;
		case KEYWORD_IF: //~matched
			assert( retval.size() == 0 );
			retval += parse_if_else(breakpoint,continuepoint,context);
			bActive = false;	//~ matched, and it's been processed
			break;
		case KEYWORD_WHILE:
			assert( retval.size() == 0 );
			retval += parse_while(context);
			bActive = false;
			break;
		case KEYWORD_FOR:
			assert( !retval.size() );
			retval += parse_for(context);
			bActive = false;
			break;
		default:	//~ 例如DONE,';',KEYWORD_END,KEYWORD_UNTIL
			bActive = false;
			break;
		}
	return retval;
}

string Parser::logicor_wrapped(const LocalVars&context)
{
	MySafeString retval( logicor_inside(context) );
	//~ in order to balance our stack
	if( is_debug_on(DEBUG_LOGICOR_WRAPPED) )
		retval += "//~ influence wax \n";
	return retval+emit_safe(string(),EM_POPWAX);
}

string Parser::logicor_inside(const LocalVars&context)
{
	//~ 翻译模式
	//~ logi_cor     ->		logicabig morelogicor
	//~	morelogicor	 ->		or logicabig {print('or')} morelogicor
	//~					|	dummy
	NextLevel fptrNext = callerChain["logicor"];
	assert(fptrNext);

	int t;
	MySafeString retval((this->*fptrNext)(context));
	bool bActive(true);
	while(bActive)
		switch(lookahead){
		case LOGICAL_OR:
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		default:
			bActive=false;
			break;
		}
	return retval;
}

string Parser::logicabig(const LocalVars& context)
{
	//~ logicabig	->		prexpr morelogica
	//~ morelogica	->		and prexpr {print('and')} morelogica
	//~					|	xor prexpr {print('xor')} morelogica
	//~					|	dummy
	NextLevel fptrNext = callerChain["logicabig"];
	assert(fptrNext);

	int t;
	MySafeString retval((this->*fptrNext)(context));
	bool bActive(true);
	while(bActive)
		switch(lookahead){
		case LOGICAL_XOR:
		case LOGICAL_AND:
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		default:
			bActive = false;
			break;
		}
	return retval;
}

string Parser::prexpr(const LocalVars& context)
{
	//~ 翻译模式
	//~ prexpr		-> expr moreprexp
		//~ 从左至右的(逻辑)运算
	//~ moreprexp	->		=  expr { print('=') } moreprexpr
	//~					|	<> expr { print('<>')} moreprexpr
	//~					|   <  expr { print('<') } moreprexpr
	//~					|    > expr { print('>') } moreprexpr
	//~					|   <= expr { print('<=')} moreprexpr
	//~					|   >= expr { print('>=')} moreprexpr
	//~					|	dummy
	NextLevel fptrNext = callerChain["prexpr"];
	assert(fptrNext);

	int t;
	MySafeString retval((this->*fptrNext)(context));
	bool bActive(true);
	while( bActive )	//~ from left to right
		switch(lookahead){
		case '=':
		case '>':
		case '<':
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		case NOT_EQUAL_TO:	//~ <>
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		case LESS_EQUAL:	//~ <=
			t = lookahead;
			match(lookahead);	//~ move on
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		case GREATER_EQUAL:	//~ >=
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			break;
		default:
			bActive=false;
			break;
		}
	return retval;
}

string Parser::expr(const LocalVars& context)
{
	//~ expr		-> term moreterms
	//~ moreterms   -> + term { print('+') } moreterms
	//~				|  - term { print('-') } moreterms
	//~				|  dummy
	NextLevel fptrNext = callerChain["expr"];
	assert(fptrNext);

	int t;
	MySafeString retval ((this->*fptrNext)(context));
	bool bActive=true;
	while(bActive)
		switch(lookahead){
		case '+':
		case '-':
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			continue;
		default:
			//~ just a normal term
			bActive = false;
			break;
		}
	return retval;
}

string Parser::term(const LocalVars& context)
{
	//~ term ->   factor morefactors
	//~ morefactors ->  *   factor { print('*') }   morefactors
	//~				  | /   factor { print('/') }   morefactors
	//~				  | div factor { print('DIV') } morefactors
	//~				  | mod factor { print('MOD') } morefactors
	//~				  |	dummy
	NextLevel fptrNext = callerChain["term"];
	assert(fptrNext);

	int t;
	MySafeString retval ((this->*fptrNext)(context));
	bool bActive = true ;
	while(bActive)
		switch(lookahead){
		case '*':
		case '/':
		case DIV:
		case MOD:
			t = lookahead;
			match(lookahead);
			retval += (this->*fptrNext)(context);
			retval += emit(t,NONE,rsymbol);
			continue;
		default:
			//~ just a "factor term" !
			bActive = false;
			break;
		}
	return retval;
}

string Parser::factor(const LocalVars& context)
{
	//~ factor		->		(logicabig)
	//~					|	id		{print(id.lexme)}
	//~					|	num		{print(num.value)}
	NextLevel zeroPtr = callerChain["factor"];
	if( zeroPtr )
		throw exception("factor should have no next");

	MySafeString retval;
	switch(lookahead){
	case ')':	//~ 函数calling,不甩吧,也许没有参数呢
		break;
	case '(':
		match('('); 
		retval.SafeAssign( logicor_inside(context) );
		match(')');
		break;
	case NUM:
		retval.SafeAssign( emit(NUM,rlexer.tokenval,rsymbol) );
		match(NUM);
		break;
	case ID:
		do{
				//~ 当且仅当adjust_rvalue后rvalue放在栈顶的值才是正确的值
			const int id_now = rlexer.tokenval;	//~ record its name
			//~retval = emit(ID,rlexer.tokenval,rsymbol);	//~ rvalue
			match(ID);
				//~ 这里的解析有三种情况
			/*
				第一, 只是			id
				第二,是数组的		varname[index]
				第三,函数CALLING	func(logicor_list)
			*/
			switch( lookahead)
			{
			case '(':	//~函数CALLING
				retval += emit_safe("1",EM_PRESERVESTACK);
				match('(');
				retval += logicor_list(context);
				match(')');
				retval += emit_safe( rlexer.rsymbol.symtable[id_now].lexptr,EM_CALL);
				break;
			case '[':	//~数组解析
				retval += emit(ID,id_now,rsymbol);
				match('[');
				retval += logicor_wrapped(context);
				match(']');
				retval += emit_safe(string(),EM_ADJUST_RVALUE);
				break;
			default:	//~ 最简单的情况
					//~ 现在看看有没有Context
				if( context.isNull() ){
					retval += emit(ID,id_now,rsymbol);
					retval += emit_safe(string(),EM_ZEROWAX);
					retval += emit_safe(string(),EM_ADJUST_RVALUE);
				}
				else{
					const string id_name(rlexer.rsymbol.symtable[id_now].lexptr);
					const int vindex = context.getIndexOfVar(id_name.c_str());
					//~ in progress...
					//~ function里面的右值
					char szIndex[BUFSIZ];
					sprintf(szIndex,"#%d //:%s",vindex,id_name.c_str());
					retval += emit_safe(szIndex,EM_LOCAL_RV);
				}
				break;
			}
		}while(0);
		break;
			//~ 单目运算真是&*$*(&!$&(*!@
	case LOGICAL_NOT:
		do{
			const int t = lookahead;
			match(lookahead);
			retval.SafeAssign(factor(context));	//~ 优先级别在这改哈
			retval += emit(t,NONE,rsymbol);
		}while(0);
		break;
	case '+':
	case '-':
		do{
			const int t = lookahead;
			match(lookahead);
			retval.SafeAssign(factor(context));
			retval += emit(t,NONE,rsymbol,SINGLE_OP);
		}while(0);
		break;
	default:
		errorx("syntax error",rlexer.lineCount());
		break;
	}
	return retval;
}

void Parser::match(int t)
{
	if( lookahead == t ){
		rlexer.so_far_so_good_update();
		lookahead = rlexer.lexan();
	}
	else{
		char msgBuf[BUFSIZ];
		sprintf(msgBuf,"syntax error:symbol No.%d(%c) expected",t,t);
		errorx(msgBuf,rlexer.lineCount());
	}
}

void Parser::prematch(int t)
{
	if( lookahead == t )
		lookahead = rlexer.lexan();
	else{
		char msgBuf[BUFSIZ];
		sprintf(msgBuf,"syntax error:symbol No.%d expected",t);
		errorx(msgBuf,rlexer.lineCount());
	}
}
