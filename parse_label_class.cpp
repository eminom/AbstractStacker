
//~ eminem7409@sina.com
//~ 
//~
#include "global.h"
#include "parser.h"
#include "lexer.h"
#include "emit.h"
#include "errorx.h"

using namespace std;

class label_exception:public exception
{
public:
	typedef Parser::labelClass::label_detail labelType;
	label_exception(int nLabel,
		const labelType& srcLabel):iLabel(nLabel),labelItem(srcLabel){}
private:
	int iLabel;
	labelType labelItem;
};

Parser::
labelClass::
label_detail::label_detail(int type)
:d_type(type),label_bits(0){
	d_type |= Parser::labelClass::MARKBIT;
}

int
Parser::
labelClass::
label_detail::self_check()const
{	
	int ishould = (d_type & label_bits);
	return (d_type^ishould);
}

bool 
Parser::
labelClass::
label_detail::marked()const
{	return (1&label_bits);}

void 
Parser::
labelClass::
label_detail::mark()
{	
	assert( !marked() );
	label_bits |= Parser::labelClass::MARKBIT ;
}

void
Parser::
labelClass::
label_detail::
visit_compulsive_bit()
{	label_bits |= Parser::labelClass::COMPULSIVE ;}

void
Parser::
labelClass::
label_detail::
visit_optional_bit()
{	label_bits |= Parser::labelClass::OPTIONAL;}

Parser::
labelClass::labelClass():
	labelEnd(0)//~ begin from very zero
	{}

Parser::
labelClass::~labelClass()
{
	//~ who's gonna throw ? me!	
	//~ destructor 的throw, poor design ? probably not
	//~(因为是程序的自我验证,与输入无关)
	const int sz = labelv.size();
	assert( labelv.size() == labelEnd );
	for(int i=labelEnd-1;i>=0;--i){
		if( labelv[i].self_check() != 0 ){
			labelClass::label_detail &thisOne = labelv[i];
			throw label_exception(i,labelv[i]);
		}
	}
}

std::string 
Parser::
labelClass::newlabel(int type)
{
	//~ labelEnd is the one after the max valid label
	char buf[BUFSIZ];
	sprintf(buf,"%04x",labelEnd++);
	assert( labelEnd > labelv.size() );
	labelv.push_back(type);
	return buf;	//~ implicit conversion, is it effective ?
}

int 
Parser::
labelClass::getLabelIntVal(const std::string &labelNow)const
{
	int nLabel(-1);
	string zero("0");
	string::size_type pos = labelNow.find_first_not_of(zero);
	if( string::npos == pos )
		nLabel = 0 ;	//~ zero label;
	else
		sscanf(labelNow.c_str()+pos,"%x",&nLabel);
	return nLabel;
}

bool 
Parser::
labelClass::isLabelValid(const std::string &labelNow)const
{
	int nLabel = getLabelIntVal(labelNow);
	if( nLabel < 0 || nLabel >= labelEnd )
		return false;
	return true;
}

void 
Parser::
labelClass::virgin_mark(const std::string &labelNow)
{
	if(!isLabelValid(labelNow) )
		throw exception("mark an invalid label");
	//~ and it is a valid label
	int nLabel = getLabelIntVal(labelNow);
	if( labelv.at(nLabel).marked() )	//~ already on, not a virgin
		throw exception("mark a non-virgin label");
	labelv[nLabel].mark();
}

void 
Parser::labelClass::
visitCompulsiveLabel(const std::string &labelNow)
{
	if( !isLabelValid(labelNow) )
		throw exception("visit an invalid compulsive label");
	int nLabel = getLabelIntVal(labelNow);
	labelv.at(nLabel).visit_compulsive_bit();
}

void
Parser::labelClass::
visitOptionalLabel(const std::string &labelNow)
{
	if( !isLabelValid(labelNow) )
		throw exception("visit an invalid optional label");
	int nLabel = getLabelIntVal(labelNow);
	labelv.at(nLabel).visit_optional_bit();
}

void
Parser::labelClass::
visitLabel(int label_plus,const std::string &labelNow)
{
	switch(label_plus)
	{
	case COMPULSIVE:
		visitCompulsiveLabel(labelNow);
		break;
	case OPTIONAL:
		visitOptionalLabel(labelNow);
		break;
	default:
		throw exception("wrong label_plus param for visit_Label");
	}
}