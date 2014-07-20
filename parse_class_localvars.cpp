

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

using namespace std;

#define __LOCAL_VARS_NULL_NAME__	"##GlobalNull"
#define _FUNC_RESULT_				"result"
#define _FUNC_RETADDR_				"==retaddr=="

LocalVars::LocalVars(const char*name)
		:funcname(name),param_count(0),local_count(0)
{
	//~ the return value here
	local_varsname.push_back(_FUNC_RESULT_);
	local_varsname.push_back(_FUNC_RETADDR_);
}

void LocalVars::addVar(const char*varname,int type){
	std::deque<std::string>::const_iterator pos;
	for(pos = local_varsname.begin();pos!=local_varsname.end();++pos)
		if( *pos == varname ){
			char buf[BUFSIZ];
			sprintf(buf,"again var %s redefinition",varname);
			throw exception(buf);
		}
	//~
	switch(type)
	{
	case IN_LOCAL_AREA:
		local_varsname.push_back(varname);
		++local_count;
		break;
	case IN_PARAM_LIST:
		do{
			deque<string>::iterator pos;
			pos = find(local_varsname.begin(),local_varsname.end(),
				string(_FUNC_RETADDR_));
			if( local_varsname.end() == pos )
				throw exception("can not do the param list, fatal error");
			local_varsname.insert(pos,varname);
		}while(0);
		++param_count;
		break;
	default:
		throw exception("what ? invalid type for add Var");
		break;
	}
	
	return;//~ it's at local_varsname.size()-1;
};

int LocalVars::getIndexOfVar(const char *varname)const
{
	int rv = distance( local_varsname.begin(),
		find(	local_varsname.begin(),
				local_varsname.end(),
				string(varname)) );
	if( local_varsname.size() == rv ){
		char buf[BUFSIZ];
		sprintf(buf,"can not get var \"%s\" in func local \"%s\"",varname,funcname.c_str());
		throw exception(buf);
	}
		//~ offset this, the return address is at the very zero
	return rv - (param_count+1);
}

bool LocalVars::isNull()const{
	return !strcmp(funcname.c_str(),__LOCAL_VARS_NULL_NAME__);
}

const 
deque<string>& LocalVars::getNameList()const{
	return local_varsname;
}

int LocalVars::getParamCount()const{
	return param_count;
}

int LocalVars::getLocalCount()const{
	return local_count;
}

bool LocalVars::operator ==(const char *name)const{
	return !strcmp(funcname.c_str(),name);
}

const std::string&LocalVars::getFuncName()const{
	return funcname;
}

//~static
LocalVars LocalVars::MakeNull()
{
	return LocalVars(__LOCAL_VARS_NULL_NAME__);
}