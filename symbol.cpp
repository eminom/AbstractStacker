
#include "global.h"
#include "symbol.h"
#include "errorx.h"

Symbol symbol;

//~ 同学们,大小写敏感哈~
	//~ these are the necessary ones
	//~ things such as =,*, are defined by their chars trait
	//~ additonal 四则,模运算
namespace {
	struct entryX keywords[]={
	"div",DIV,
	"mod",MOD,
		//~
	":=",ASSIGN_TO,
		//~ 比较
	"<>",NOT_EQUAL_TO,
	"<=",LESS_EQUAL,
	">=",GREATER_EQUAL,
		//~
	"and",LOGICAL_AND,
	"xor",LOGICAL_XOR,
		//~
	"or",LOGICAL_OR,
	"not",LOGICAL_NOT,
		//~以下是关键字定义,大小写注意了哈
	"if",KEYWORD_IF,
	"then",KEYWORD_THEN,
	"else",KEYWORD_ELSE,
	"while",KEYWORD_WHILE,
	"do",KEYWORD_DO,
	"begin",KEYWORD_BEGIN,
	"end",KEYWORD_END,
	"for",KEYWORD_FOR,
	"to",KEYWORD_TO,
	"repeat",KEYWORD_REPEAT,
	"until",KEYWORD_UNTIL,
	"break",KEYWORD_BREAK,
	"continue",KEYWORD_CONTINUE,
	"case",KEYWORD_CASE,
	"of",KEYWORD_OF,
	"xdim",KEYWORD_XDIM,
	"function",KEYWORD_FUNCTION,
	"integer",KEYWORD_INTEGER,
	"var",KEYWORD_VAR,
	0,0
	};
}//~ anonymous namespace 

Symbol::Symbol():
	lastchar(-1),
	lastentry(0)
{
	struct entryX *p;
	for(p = keywords;p->token;p++)
		insert(p->lexptr,p->token);
	//~ check check
	int ck,cj;
	for(ck=lastentry; ck > 1 ; --ck ){
		for(cj = ck - 1 ; cj > 0 ; --cj )
			assert( symtable[ck].token != symtable[cj].token );
	}
}

int Symbol::lookup(const char *s)const
{
	int p;
	for(p = lastentry ; p > 0 ; --p)
		if(0 == strcmp(symtable[p].lexptr,s))
			break;
	return p;	//~ 
}

int Symbol::lookup(int t)const
{
	int i;
	for(i= lastentry; i > 0 ; --i)
		if( t == symtable[i].token )
			break;
	return i;
}

int Symbol::insert(char s[],int tok)
{
	int len;
	len = strlen(s);
	if( lastentry + 1 >= SYMMAX )
		errorx("symbol table full",0);
	if( lastchar + len + 1 >= STRMAX )
		errorx("lexemes buffer full",0);
	//~ Entry No.0 is purposely empty
	lastentry ++;
	symtable[lastentry].token = tok;
	symtable[lastentry].lexptr = &lexemes[lastchar+1];
	lastchar = lastchar + len + 1;
	strcpy(symtable[lastentry].lexptr , s );
	return lastentry;
}

