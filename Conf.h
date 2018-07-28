#include <map>
#pragma once
using namespace std;

typedef map<string, string> ConfMap;
class Conf
{
private:
	ConfMap opmap;
public:
	Conf();	
	~Conf();
	string getOp(string op_name);
	void setOp(string op_name, string op_val);
	
	//whether we are auto-calculating the descriptors'(properties and string words) importance
	bool autoWeights(){
		return getOp("auto_weights") == "1";
	}

	//whether we are incuding the literal nodes in the similarity calculations and using a string sim measure for them
	bool literalSims(){
		return getOp("literal_sims") == "1";
	}

};

