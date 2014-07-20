

#include "emit.h"

using namespace std;

string emit(int t,
			int tval,
			const Symbol &rsymbol,
			const int mode)
{	//~ output
	MySafeString retval;
	char buf[BUFSIZ];
	switch(t){
			//~ 到这就不谈论什么优先级了哈
	case '=':
	case '+':
	case '-':
	case '*':
	case '/':
	case '<':
	case '>':
		sprintf(buf,"%c",t);
		retval.SafeAssign(buf);
		if( SINGLE_OP == mode)
			retval+="[s]\n";
		else
			retval+="\n";
		break;
	case LOGICAL_OR:
	case LOGICAL_XOR:
	case LOGICAL_AND:
	case LOGICAL_NOT:
	case ASSIGN_TO:
	case NOT_EQUAL_TO:
	case LESS_EQUAL:
	case GREATER_EQUAL:
	case DIV:
	case MOD:{
		int p = rsymbol.lookup(t);
		if( !p )
			throw exception("exception in emit");
		sprintf(buf,"%s\n",rsymbol.symtable[p].lexptr);
		retval.SafeAssign(buf);
		}break;
	case NUM:
		sprintf(buf,"%d \n",tval);
		retval.SafeAssign(buf);
		if( RVALUE == mode ){
			retval.SafeAssign("push " + retval,true);
		}
		else{
			throw exception("num what ? rvalue or not?!");
		}
		break;
	case ID:
		sprintf(buf,"%s \n",rsymbol.symtable[tval].lexptr);
		retval.SafeAssign(buf);
		switch(mode){
		case RVALUE:
			retval.SafeAssign("rvalue " + retval,true);
			break;
		case LVALUE:
			retval.SafeAssign("lvalue " + retval,true);
			break;
		default:
			throw exception("emitter mode error for ID");
		}
		break;
	case NEWLINE:
		retval.SafeAssign("\n\n");
		break;
	case KEYWORD_XDIM:
		sprintf(buf,"movwax %d\nxdim %s\n",mode,rsymbol.symtable[tval].lexptr);
		retval.SafeAssign(buf) ;
		break;
	default:
		cerr<<"token "<<t<<"tokenval "<<tval<<endl;
		throw exception("token not emitted");
		break;
	}
	return retval;
}
