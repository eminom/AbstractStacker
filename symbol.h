
#ifndef _SYMBOL_H_
#define _SYMBOL_H_

class Symbol
{
	enum{ STRMAX = 999};//~lexemes's size
	enum{ SYMMAX = 100};//~size of symtable
public:
	Symbol();
public:
	//~
	char lexemes[STRMAX];
	int lastchar;
	struct entryX symtable[SYMMAX];
	int lastentry;

public:
	int lookup(const char *s)const;
	int lookup(int t)const;
	int insert(char s[],int tok);
};

#endif	//~_SYMBOL_H_