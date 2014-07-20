
//~
#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "symbol.h"

#include <deque>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <bitset>
#include <exception>

enum{
	EM_GOFALSE,
	EM_GOTRUE,
	EM_GOTO,
	EM_LABEL,
	EM_POPWAX,
	EM_POPWBX,
	EM_PUSHWAX,
	EM_PUSHWBX,
	EM_ZEROWAX,
	EM_ADJUST_LVALUE,
	EM_ADJUST_RVALUE,
		//~ function specification
	EM_CALL,
	EM_LOCAL_RV,
	EM_LOCAL_LV,
	EM_DECSTACK,
	EM_RETURN,	//~ back from function
	EM_PRESERVESTACK,
	EM_LABEL_FUNCTION,
		//`
	EM_COUNT
};

//~ 调试位
enum DEBUG_BITS{
	DEBUG_IF,
	DEBUG_FOR,
	DEBUG_WHILE,
	DEBUG_REPEAT_UNTIL,
	//~
	DEBUG_BREAK,
	DEBUG_CONTINUE,
	DEBUG_CASE_OF,
	DEBUG_FUNCTION,
	//~
	DEBUG_LOGICOR_WRAPPED,

	//~
	
	//~
	DEBUG_COUNT
};

class LocalVars
{
public:
	enum { 
		IN_PARAM_LIST,
		IN_LOCAL_AREA,
	};
private:
	std::string funcname;	//~ can not be const
	std::deque<std::string> local_varsname;
	int param_count;
	int local_count;
		//~total_count = 1+param_count+local_count
public:
	LocalVars(const char*name);
	void addVar(const char*varname,int type);
		//~ may throw


	int getIndexOfVar(const char *varname)const;
	bool isNull()const;
	int getParamCount()const;
	int getLocalCount()const;
	const std::string& getFuncName()const;
	const std::deque<std::string>& getNameList()const;

	bool operator==(const char *)const;	//~ name compare

	static LocalVars MakeNull();
};

class Parser
{
	friend class label_exception;
public:
	Parser(const char *outputName,Lexer &lexer,Symbol &symbol);
	//~
	void parse();
public:
	class labelClass{
		friend class label_exception;
		friend class label_detail;	//~ my own oh my God
	public:
		enum{
			MARKBIT=1,		//~ must be marked( all label must be marked)
			COMPULSIVE=2,		//~ must be refered to whatever
			OPTIONAL=4,			//~ must be refered in optional op if optional
		};
	public:
		labelClass();	//~ ctor
		~labelClass();	//~ sintenl
		std::string newlabel(int type);

		bool isLabelValid(const std::string&)const;
		//~ bit operation for labels
		void virgin_mark(const std::string&)throw(exception);

		void visitLabel(int label_plust,const std::string&);
	private:
		void visitCompulsiveLabel(const std::string&);
		void visitOptionalLabel(const std::string&);

	private:
		int getLabelIntVal(const std::string&)const;
		enum { M_INIT_SZ = 5};
		enum { M_EXPAND_SZ = 10};

		class label_detail{
		public:
			label_detail(int type);
			//~
			bool marked()const;				//~ bit 0;
			//~
			void mark();			//~ only once
			void visit_compulsive_bit();//~ may be more than once
			void visit_optional_bit();	//~ may be more than once
			//~ do xor
			int self_check()const;

		private:
			int d_type;	//~ label的类型
			int label_bits;
		};
		std::vector<label_detail> labelv;
		int labelEnd;
	};
private:
	mutable labelClass labelGirl;
	//~
	void prematch(int);
	void match(int);

	std::string xlist(
		const std::string breakpoint,
		const std::string continuepoint,
		const LocalVars&);
	std::string stmt(
		const  std::string breakpoint,
		const std::string continuepoint,
		const LocalVars&);

	std::string parse_function();	//~ this will build me a context

	std::string logicor_list(const LocalVars&);		//~ function call's param list
	std::string logicor_wrapped(const LocalVars&);
	std::string logicor_inside(const LocalVars&);
	std::string logicabig(const LocalVars&);
	std::string prexpr(const LocalVars&);
	std::string expr(const LocalVars&);
	std::string term(const LocalVars&);
	std::string factor(const LocalVars&);

	//~ statement in details( especially when it grows)
	std::string parse_for(const LocalVars&);
	std::string parse_repeat_until(const LocalVars&);
	std::string parse_while(const LocalVars&);
	std::string parse_case_of(const std::string,const std::string,const LocalVars&);
	std::string parse_if_else(const std::string,const std::string,const LocalVars&);
	
private:
	std::ifstream input;
	std::ofstream output;
	int lookahead;
	Lexer &rlexer;
	Symbol &rsymbol;
	class FuncMan{
	private:
		std::vector<LocalVars> func_of_mine;
	public:
		LocalVars& operator[](int index);
		int AddFunc(const char*);
		FuncMan();
		~FuncMan();
		const LocalVars& nullContext()const;
	};
	FuncMan funcMan;
	//~
private:
		//~modification on February28th.2oo7
		//~ these functions are evaluated in context
	typedef std::string (Parser::*NextLevel)(const LocalVars&);
private:
	std::map<std::string,NextLevel> callerChain;
	bool initCallerChain();
	//~
	std::string emit_safe(const std::string&,
		int,
		int = Parser::labelClass::COMPULSIVE )const;

	std::bitset<DEBUG_COUNT> debug_bits;
	bool is_debug_on(int)const;
public:
	void set_debug_mode(int);
};

#endif	//~_PARSER_H_