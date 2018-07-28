#include "Conf.h"

Conf::Conf()
{
}


Conf::~Conf()
{
}
void Conf::setOp(string op_name, string op_val){
	opmap[op_name] = op_val;
}
string Conf::getOp(string op_name){
	if (opmap.find(op_name) != opmap.end())
		return opmap[op_name];
	else
		return "";
}
