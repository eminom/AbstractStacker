
#ifndef _MY_EMIT_H_
#define _MY_EMIT_H_
#include "global.h"
#include "symbol.h"

//~ mode for emit
enum{
RVALUE,
LVALUE,
SINGLE_OP};

std::string emit(int,
				 int,
				 const Symbol&,
				 int =RVALUE);

#endif //~ _MY_EMIT_H_
