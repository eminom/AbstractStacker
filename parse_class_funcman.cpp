

//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

using namespace std;

int
Parser::FuncMan::AddFunc(const char *name)
{
	for(int i=func_of_mine.size()-1;i>=0;i--){
		LocalVars& thisLocalVar = func_of_mine[i];
		if( thisLocalVar == name ){
			char buf[BUFSIZ];
			sprintf(buf,"function %s again ? no",name);
			throw exception(buf);
		}
	}
	func_of_mine.push_back(LocalVars(name));
	return func_of_mine.size()-1;
}

LocalVars&
Parser::FuncMan::operator [](int index){
	return func_of_mine.at(index);
}

Parser::FuncMan::FuncMan()
{
	func_of_mine.push_back(LocalVars::MakeNull());
}

const LocalVars&
Parser::FuncMan::nullContext()const{
	return func_of_mine.at(0);	//~ 
}

Parser::FuncMan::~FuncMan()
{
	//~ debug, show me more
	if( 1 ){
		cout<<"func man s destruction.."<<endl;
		vector<LocalVars>::const_iterator it;
		for(it=func_of_mine.begin();it!=func_of_mine.end();++it){
			const LocalVars& thisLocalVar = *it;
			cout<<" function \""<<thisLocalVar.getFuncName()<<"\"  > \n"
				<<"param_count = "<<thisLocalVar.getParamCount()<<"\n"
				<<"local_count = "<<thisLocalVar.getLocalCount()<<endl;

			const deque<string>& varlist=thisLocalVar.getNameList();
			deque<string>::const_iterator pos;
			for(pos = varlist.begin();pos!=varlist.end();++pos)
				cout<<"\t["<<thisLocalVar.getIndexOfVar(pos->c_str())<<"]\t"<<*pos<<"\n";
			cout<<endl;
		}
	}
}