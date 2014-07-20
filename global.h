
//~ eminem7409@sina.com

#ifndef GLOBAL_H
#define GLOBAL_H

#pragma warning(disable:4786)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cassert>

#define BSIZE	512
#define NONE	-1
#define EOS		'\0'

enum{
NUM=256,
DIV,
MOD,
ID,
DONE,
NEWLINE,
	//~ additional Feb13th.2007
	//~
ASSIGN_TO,		//~		262	//~:=
	//~
NOT_EQUAL_TO,	//~		263	//~ <>
LESS_EQUAL,		//~		264	//~ <=
GREATER_EQUAL,	//~		265	//~ >=
	//~
LOGICAL_AND,	//~		266	//~ and
LOGICAL_NOT,	//~		267	//~ not
LOGICAL_OR,		//~

LOGICAL_XOR,		//~		Plus.Valentine Day
KEYWORD_IF,			//~ if
KEYWORD_THEN,		//~ then
KEYWORD_WHILE,		//~ while
KEYWORD_DO,			//~ do
KEYWORD_ELSE,		//~ else
KEYWORD_BEGIN,		//~ begin
KEYWORD_END,		//~ end
KEYWORD_FOR,		//~ for(终于来了耶)
KEYWORD_TO,			//~ to
KEYWORD_REPEAT,		//~ repeat
KEYWORD_UNTIL,		//~ until
KEYWORD_BREAK,		//~ break		//~ 281
KEYWORD_CONTINUE,	//~ continue;
KEYWORD_CASE,		//~ case
KEYWORD_OF,			//~ case's of
KEYWORD_XDIM,		//~ xdim (for数组声明)
KEYWORD_FUNCTION,	//~ function
KEYWORD_INTEGER,	//~ integer
KEYWORD_VAR,		//~ "var" for function
};

//~ end of SYMBOL definition

struct entryX
{
	char*	lexptr;
	int		token;
};

//~typedef std::string MySafeString;
class MySafeString:public std::string
{
public:
	MySafeString();
	MySafeString(const std::string&);//~conversion
	void SafeAssign(const std::string&,bool warning_off=false);
	//~
public:	
	//~ reinterpreting this
	MySafeString& operator+=(const std::string&);
protected:
	void operator=(const std::string&);
};


#endif	//~GLOBAL_H