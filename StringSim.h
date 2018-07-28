// String similarity functions
#ifndef _SITRINGSIM_H
#define _SITRINGSIM_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include<set>
#include<sstream>
#include "Util.h"
using namespace std;

// Define some constants to make simMethod processing easier to read:
const int LEVEN = 0;
const int JACCARD = 1;
const int JACCARD_WITH_TFIDF = 5;
const int LCSM = 2;
const int NGRAM = 3;
const int JARO = 4;

#define MAX_LCS 256	/*Maximum size of the longest common sequence. You might wish to change it*/
#define MAX_CHAR_ARRAY_SIZE  256
#define SCALING_FACTOR 0.1 


class StringSim
{
private:
	void swap( unsigned **first, unsigned **second);
public:
	StringSim(void){};
	float GetSim(string s1, string s2, int method);

	float similLCS(const char *str1, const char *str2);
	float similLEVEN(const char *str1, const char *str2);
	double jaro_winkler_distance(const char *s, const char *a);
	//double similJACCARD_WITH_TFIDF(Graph& g, int vid1, int vid2, string s1, string s2);


	unsigned int levenshtein_distance(const char *s1, const char *s2) {
		int len1 = strlen(s1), len2 = strlen(s2);
		vector<unsigned int> col(len2+1), prevCol(len2+1);

		for (unsigned int i = 0; i < prevCol.size(); i++)
			prevCol[i] = i;
		for (unsigned int i = 0; i < len1; i++) {
			col[0] = i+1;
			for (unsigned int j = 0; j < len2; j++)
				col[j+1] = min( min( 1 + col[j], 1 + prevCol[1 + j]),
				prevCol[j] + ( tolower(s1[i])== tolower( s2[j] ) ? 0 : 1) );
			col.swap(prevCol);
		}
		return prevCol[len2];
	}

	string *LCS( string *str1, string *str2);
};

class LCS
{
	class LCSTable
	{
		size_t   m_;
		size_t   n_;
		size_t*  data_;
	public:
		LCSTable(size_t m, size_t n)
			: m_(m)
			, n_(n)
		{
			data_ = new size_t[(m_ + 1) * (n_ + 1)];
		}
		~LCSTable()
		{
			delete [] data_;
		}

		void setAt(size_t i, size_t j, size_t value)
		{
			data_[i + j * (m_ + 1)] = value;
		}

		size_t getAt(size_t i, size_t j) const
		{
			return data_[i + j * (m_ + 1)];
		}

		template<typename T> void
			build(const T* X, const T* Y)
		{
			for (size_t i=0; i<=m_; ++i)
				setAt(i, 0, 0);

			for (size_t j=0; j<=n_; ++j)
				setAt(0, j, 0);

			for (size_t i = 0; i < m_; ++i)
			{
				for (size_t j = 0; j < n_; ++j)
				{
					if ( tolower(X[i]) == tolower(Y[j]))
						setAt(i+1, j+1, getAt(i, j)+1);
					else
						setAt(i+1, j+1, std::max(getAt(i+1, j), getAt(i, j+1)));
				}
			}
		}
	};

	template<typename T> static void
		backtrackOne(const LCSTable& table,
		const T* X, const T* Y, size_t i, size_t j,
		std::vector<T>& result)
	{
		if (i == 0 || j == 0)
			return;
		if ( tolower( X[i - 1] ) == tolower( Y[j - 1] ))
		{
			backtrackOne(table, X, Y, i - 1, j - 1, result);
			result.push_back(X[i - 1]);
			return;
		}
		if (table.getAt(i, j - 1) > table.getAt(i -1, j))
			backtrackOne(table, X, Y, i, j - 1, result);
		else
			backtrackOne(table, X, Y, i - 1, j, result);
	}

public:
	template<typename T> static void
		findOne(const T* X, size_t m, const T* Y, size_t n, std::vector<T>& result)
	{
		LCSTable table(m, n);
		table.build(X, Y);
		backtrackOne(table, X, Y, m, n, result);
	}
};

#endif
