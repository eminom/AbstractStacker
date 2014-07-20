
#include "global.h"
#include "parser.h"
#include "errorx.h"

#include <set>
using namespace std;

typedef set<int> OptVec;

void
ProcessOpts(int argc,const char **argv,OptVec &dopts)
{
	for(int i=0;i<argc;++i){
		//~
		if('-' == argv[i][0] ){
			switch( argv[i][1] ){
			case 'd':
				do{
					if( !strcmp(argv[i],"-dall") ){
						cout<<"debug options are all turned on"<<endl;
						int t;
						for(t=0;t<DEBUG_COUNT;t++)
							dopts.insert(t);
					}
					else
					switch( argv[i][2] ){
					case 'f':
						if( !strcmp(argv[i],"-dfor") ){
							dopts.insert( DEBUG_FOR);
							cout<<"debug for"<<endl;
						}
						if( !strcmp(argv[i],"-dfunction") ){
							dopts.insert( DEBUG_FUNCTION);
							cout<<"debug function"<<endl;
						}
						break;
					case 'w':
						if( !strcmp(argv[i],"-dwhile") ){
							dopts.insert(DEBUG_WHILE);
							cout<<"debug while"<<endl;
						}
						break;
					case 'r':	//~ 
						if( !strcmp(argv[i],"-drepeat") ){
							dopts.insert(DEBUG_REPEAT_UNTIL);
							cout<<"debug repeat"<<endl;
						}
						break;
					case 'b':	//~ break
						if( !strcmp(argv[i],"-dbreak") ){
							dopts.insert(DEBUG_BREAK);
							cout<<"debug break"<<endl;
						}
						break;
					case 'c':	//~ continue
						if( !strcmp(argv[i],"-dcontinue") ){
							dopts.insert(DEBUG_CONTINUE);
							cout<<"debug continue"<<endl;
						}
						else if( !strcmp(argv[i],"-dcase") ){
							dopts.insert(DEBUG_CASE_OF);
							cout<<"debug case"<<endl;
						}
					case 'i':
						if( !strcmp(argv[i],"-dif") ){
							dopts.insert(DEBUG_IF);
							cout<<"debug if"<<endl;
						}
						break;
					}
				}while(0);
				break;
			}
		}
	}
};

void UsageX()
{
	cout<<"[amkv0p82.Mac03.23th.keeper] \n"
		"-d[debug options]\n"
		"  if/while/for/repeat/break/continue/case/function\n"
		"  all: turn all debug options on\n"
		<<"built on "<<__TIME__<<";"<<__DATE__<<";\n"
		"eminem7409@163.com"<<endl;
}

void ProcessFileName(int argc,const char**argv,
					 char *inputName,char *outputName)
{
	if( argc >= 2 ){
		strcpy(inputName,argv[1]);
		strcat(inputName,".mpas");
		if( argc >= 3 && argv[2][0] != '-' ){
			strcpy(outputName,argv[2]);
		}
		else{
			char tname[BUFSIZ];
			strcpy(tname,inputName);
			char *const ptrc = strrchr(tname,'.');
			if( ptrc )
				*ptrc = 0 ;
			sprintf(outputName,"%s.stacker",tname);
		}
	}
}

int main(int argC,const char **argV)
{
	int rv(0);
	//` pre
	cout<<"[amk] stacker maker for Pascal source"<<endl;
	if( argC < 2 ){
		cerr<<"i need one more names\n"
			"one for input\n"
			"the other for output"<<endl;
		UsageX();
		return -1;	//~ parameters error
	}

	OptVec debug_opts;
	ProcessOpts(argC,argV,debug_opts);
	if( debug_opts.empty() )
		cout<<"debug modes off"<<endl;

	char inName[BUFSIZ],outName[BUFSIZ];
	ProcessFileName(argC,argV,inName,outName);
	//~
	try{
		cout<<"processing..with "<<inName<<"  >> "<<outName<<endl;
		Symbol symbol;
		Lexer lexer(inName,symbol);
		Parser manager(outName,lexer,symbol);
		//~
		OptVec::const_iterator pos;
		for(pos = debug_opts.begin();pos!=debug_opts.end();++pos)
			manager.set_debug_mode( *pos );
		//~ ok, GO!
		manager.parse();
		cout<<lexer.lineCount()<<" lines has been processed\n"
			<<"compiling succeeds"<<endl;
	}
	catch(exception&err){
		cerr<<err.what()<<endl;
		rv=1;
	}
	catch(...){
		cerr<<"something came up"<<endl;
		rv=2;
	}
	cout<<"done with code "<<rv<<endl;
	return rv;	//~ 0 indicates success compiling
}