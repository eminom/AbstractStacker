
#include "global.h"
using namespace std;

void errorx(const char *m,int lineCount)
{
	cerr<<m<<endl;
	if( lineCount > 0 )
		cerr<<"line "<<lineCount<<endl;
	throw exception("error issued");
}