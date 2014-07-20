
//~ from "Compiler, Principle, Techniques and Tools"
//~ typed by eminem7409@sina.com

#include "global.h"
#include "lexer.h"
#include "errorx.h"

using namespace std;

//~ _tracebuf用循环队列吧

#define _TB_SIZE()		( sizeof(_tracebuf)/sizeof(_tracebuf[0]) )
#define _TB_EMPTY()		( prh == pos )
#define _TB_HAS_DATA()	( prh != pos )
#define _TB_FULL()		( (pos+1) % _TB_SIZE() == prh )

bool Lexer::input_wrapped::eof()const{
	return( baseInputClass::eof() && _TB_EMPTY() );
}

int Lexer::input_wrapped::get_()
{
	int t;
	if( _TB_HAS_DATA() ){
		t = _tracebuf[prh++];
	}
	else{
		assert( _TB_EMPTY() );
		//~
		t = get();
		++valid_sz;
		_tracebuf[pos++] = t;
		++prh;
		assert( _TB_EMPTY() );
	}
	if( '\n' == t )
		++lineno;
	return t;
	//~done
}

void Lexer::input_wrapped::putback_(int t)
{
	if( _TB_FULL() )
		throw exception("trace buf full");
	prh -- ;
	if( '\n' == _tracebuf[prh] )
		--lineno;
	assert( t == _tracebuf[prh] );	//~ redudancy, I hope
	//~ done
}

void Lexer::input_wrapped::backoff_(){
	//~ compute the dest val of prh
	int destprh = pos - valid_sz;
	while(destprh < 0 )
		destprh += _TB_SIZE();
	while(destprh >= _TB_SIZE() )
		destprh -= _TB_SIZE();

	while( prh != destprh ){
		int t = _tracebuf[--prh];
		if( '\n' == t )
			--lineno;
	}
	//~ done
}

void Lexer::input_wrapped::so_far_so_good_(){
	//~ recompute valid_sz
	int sz = pos - prh;
	while( sz < 0 )
		sz+=_TB_SIZE();
	valid_sz = sz;
	//~ done
}

int Lexer::input_wrapped::lineC()const
{	return lineno;}


Lexer::Lexer(const char *inputName,Symbol &symbol):
	tokenval(NONE),
	input(inputName),
	rsymbol(symbol)
{
	if( !input )
		throw exception("can not open input stream");
}

int Lexer::lineCount()const
{
	return input.lineC();
}

void Lexer::backoff()
{	input.backoff_();}

void Lexer::so_far_so_good_update()
{	input.so_far_so_good_();}

int Lexer::lexan()
{						//~ lexem analyzer
	int t;
	while(1){
		t = input.get_();
		if( t == ' ' || t == '\t')
			;	//~
		else if( t == '\n' )
			;
		else if( isdigit(t)){
			int val = t-'0';
			do{
				t = input.get_();
				if( isdigit(t) ){
					val*=10;
					val+=t-'0';
				}
				else break;
			}while(1);
			if( !isdigit(t) && t != EOF )
				input.putback_(t);
			tokenval  = val;
			return NUM;
		}
		else if( isalpha(t) ){
			int p, b = 0;
			while(isalnum(t)){
				lexbuf[b] = t;
				t = input.get_();
				b++;
				if( b>=sizeof(lexbuf)/sizeof(lexbuf[0]) )
					errorx("compiler error",input.lineC());
			}
			lexbuf[b] = EOS;	//~ \0
			if( t != EOF )
				input.putback_(t);
			p = rsymbol.lookup(lexbuf);
			if( 0 == p )
				p = rsymbol.insert(lexbuf,ID);
			tokenval = p;
			return rsymbol.symtable[p].token;
		}
		else if( t == EOF )
			return DONE;
		else{	//~ modification on February 13th.2007
			int b = 0;
			do{
				lexbuf[b++] = t;
				t = input.get_();
			}while( !input.eof() //~ not the end
				&& t!=' '		//~ not space
				&& t!= '\t'		//~ not delim
				&& !isalnum(t)  //~ being special
				&& t >' ' );	//~ not a control char
			input.putback_(t);	//~ in my buffer, in my own place, anything can be placed back!
			//~ perform a basic max munch
			int p(0);
			while( b > 0 ){
				lexbuf[b] = EOS;	//~ end of word
				p = rsymbol.lookup(lexbuf);
				if( p != 0 )
					break;
				input.putback_(lexbuf[b-1]);
				b--;
			}
			if( b > 0 ){
				tokenval = p;
				return rsymbol.symtable[p].token;
			}
			else{
				tokenval = NONE;
				t = input.get_();	//~ only one
				return t;
			}
		}
	}
}
