

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"


std::string
Parser::logicor_list(const LocalVars& context)
{
	MySafeString retval( logicor_inside(context) );
	bool bActive(true);
	while(bActive)
		switch(lookahead)
		{
		case ',':
			match(',');
			retval += logicor_inside(context);
				//~ don't pop
			break;
		default:
			bActive=false;
			break;
		}
	return retval;	//~ nothing at all;
}