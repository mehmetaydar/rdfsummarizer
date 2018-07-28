#ifndef _UTIL_H
#define _UTIL_H

#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

class Util
{
private:	
public:

	// convert string to int
	static int string_to_int(string _s)
	{
		stringstream s(_s);
		int x = 0;
		s >> x;
		return x;
	}

	// convert int to string
	static string itos(int i) 
	{
		stringstream s;
		s << i;
		return s.str();
	}
	static string replaceChar(string str, char ch1, char ch2) {
		for (int i = 0; i < str.length(); ++i) {
			if (str[i] == ch1)
				str[i] = ch2;
		}
		return str;
	}

	static string removeChar(string str, char ch1) {
		std::string output;
		for (int i = 0; i < str.length(); ++i) {
			if (str[i] != ch1)
				output += str[i];
		}
		return output;
	}

	static string removePunc(string text){
		std::string result = text;
		//char c = result[0];
		try{
			int len;
			for (int i = 0, len = result.size(); i < len; i++)
			{
				if (result[i] > 0){
					if (ispunct(result[i]))
					{
						result.erase(i--, 1);
						len = result.size();
					}
				}
			}
		}
		catch(exception e){}
		return result;
	}

	static string toLower(string data){
		try{
			std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		}
		catch (exception e){}
		return data;
	}
	static string cleanValueNode(string data){
		return toLower(removePunc(data));
	}
		
};

#endif

