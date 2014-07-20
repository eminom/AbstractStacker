#include "global.h"

using namespace std;

//~ ctor
MySafeString::MySafeString():string()
{}

MySafeString::MySafeString(const std::string& rhs):string(rhs)
{}

MySafeString&
MySafeString::operator +=(const std::string& rhs)
{
	//~ check check
	if( rhs.size() > 0 && 
		rhs.at( rhs.size() - 1 ) != '\n' ){
		cerr<<"warning:+=withour \\n"<<endl;
	}

	string &stringNow = *this;
	stringNow.operator +=(rhs);
	return *this;
}


void MySafeString::SafeAssign(const std::string &rhs,bool warning_off)
{
	string &stringNow = (*this);
	if( !warning_off ){
		if( stringNow.length() > 0 )
			cerr<<"warning:"<<stringNow<<" not empty"<<endl;
	}
	stringNow.operator = (rhs);
}