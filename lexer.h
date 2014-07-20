
#ifndef _LEXER_H_
#define _LEXER_H_
#include "symbol.h"

//~using namespace std;

class Lexer
{
public:
	Lexer(const char *inputName,Symbol &symbol);
	int lexan();
	
	int lineCount()const;
	//~ special
	void backoff();
	void so_far_so_good_update();
private:
	//~
	class input_wrapped:private std::ifstream
	{
		typedef std::ifstream baseInputClass;
	public:
		//~ctor
		input_wrapped(const char* name):
			baseInputClass(name),
			prh(0),
			pos(0),
			valid_sz(0),
			lineno(1){}
		//~
		int get_();
		void putback_(int);
		void backoff_();
		void so_far_so_good_();
		bool eof()const;
		//~ exempt
	public:
		using baseInputClass::operator !;

	public:
		int lineC()const;
		
	private:
		std::ios::off_type pos;
		std::ios::off_type prh;
		std::ios::off_type valid_sz;
		char _tracebuf[BSIZE<<4];
		int lineno;
	};
public:
	int tokenval;
	Symbol &rsymbol;
private:				//~ cuz inner space
	input_wrapped input;
	char lexbuf[BSIZE];	//~
};

#endif	//~_LEXER_H_