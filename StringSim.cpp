// String similarity functions
#include "StringSim.h"
#include <iostream>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h >


static int max(int x, int y) {
	return x > y ? x : y;
}

static int min(int x, int y) {
	return x < y ? x : y;
}


float StringSim::GetSim(const string s1, const string s2, int method)
{
	if(method == LCSM)
	{
		return similLCS(s1.c_str(),s2.c_str());
	}
	else if(method == LEVEN)
		return similLEVEN(s1.c_str(), s2.c_str()); 
	else if (method == JARO)
		return jaro_winkler_distance(s1.c_str(), s2.c_str());
	else if (method == JACCARD_WITH_TFIDF)
		//return similJACCARD_WITH_TFIDF(s1, s2);
		throw std::logic_error("Please call this method directly from the Graph instance!. method: " + method);
	else
		throw std::logic_error("Similarity method has not been implemented!. method: " + method);
	return -1;
}

/*double StringSim::similJACCARD_WITH_TFIDF(Graph& g, int vid1, int vid2, string s1, string s2){
	set<string> set1;
	set<string> set2;
	stringstream ssin1(s1);
	stringstream ssin2(s2);
	string word;
	while (ssin1.good()){
		ssin1 >> word;
		set1.insert(word);
	}
	while (ssin2.good()){
		ssin2 >> word;
		set2.insert(word);
	}
	std::set<string>::iterator it;
	float nom = 0.0;
	float denom = set1.size() + set2.size();
	float num_common = 0.0;
	for (it = set1.begin(); it != set1.end(); ++it){
		word = *it;
		if (set2.find(word) != set2.end()){
			num_common++;
			//common word on both
			nom+= (g.getStringWords()[word].Importancy_PerVertex[vid1] + g.getStringWords()[word].Importancy_PerVertex[vid2]) / 2;
		}
	}
	denom = denom - num_common;
	if (denom == 0)
		return 0.0;
	return nom / denom;
}*/

float StringSim::similLEVEN(const char *X,const char *Y)
{
	float lenLEVEN, lenX, lenY;
	lenX = strlen(X);
	lenY = strlen(Y);
	if(lenX ==0 || lenY ==0)
		return 0;
	lenLEVEN= levenshtein_distance(X,Y);
	//return ( (lenX-lenLEVEN)/lenX + (lenY-lenLEVEN)/lenY)/2;
	return (max(lenX, lenY) - lenLEVEN) / max(lenX, lenY);
}

/* Quick and dirty swap of the address of 2 arrays of unsigned int
*/
void StringSim::swap( unsigned **first, unsigned **second)
{
	unsigned *temp;
	temp=*first;
	*first=*second;
	*second=temp;
}
/* A function which returns how similar 2 strings are
Assumes that both point to 2 valid null terminated array of chars.
Returns the similarity between them.
*/
float StringSim::similLCS(const char *X,const char *Y)
{
	vector<char> result;
	float lenLCS, lenX, lenY;
	lenX = strlen(X);
	lenY = strlen(Y);
	if(lenX ==0 || lenY ==0)
		return 0;
	LCS::findOne( X, lenX , Y, lenY, result);
	//cout<<"result: " << result << endl;
	//    std::string resultString(&result.front(), result.size());
	//cout<<"result: " << result << endl;
	lenLCS=result.size();
	return (lenLCS/lenX + lenLCS/lenY)/2;
}
/*	Returns a pointer to the Longest Common Sequence in str1 and str2
Assumes str1 and str2 point to 2 null terminated array of char
*/
string *StringSim::LCS( string *str1, string *str2)
{
	//static char lcs[MAX_LCS];
	string lcs;
	int i, r, c, len1=str1->length(), len2=str2->length();
	unsigned **align;
	if( len1==0 || len2==0)
		return 0;
	align=(unsigned **)calloc( len2+1, sizeof(unsigned *));
	for( r=0; r<=len2; ++r)
		align[r]=(unsigned *)calloc( len1+1, sizeof(unsigned));
	for(r=1; r<=len2; ++r)
		for(c=1; c<=len1; ++c)
			if( str1[c-1].compare(str2[r-1]) == 0)
				align[r][c]=align[r-1][c-1]+1;
			else align[r][c]=align[r-1][c]>=align[r][c-1]?align[r-1][c]:align[r][c-1];
			for(r=len2, c=len1,i=align[r][c], lcs[i]='\0';
				i>0 && r>0 && c>0; i=align[r][c]){
					if( align[r-1][c] ==i)
						--r;
					else if( align[r][c-1]==i)
						--c;
					else if(align[r-1][c-1]==i-1){
						//lcs[i-1]=str2[--r];
						lcs+= str2[--r];
						--c;
					}
			}
			for( r=len2;r>=0; --r)
				free( align[r]);
			free(align);

			cout <<"lcs: "<< &lcs<<endl;
			return &lcs;
}


/*	Returns a jaro_winkler distance measure between in str1 and str2
Assumes str1 and str2 point to 2 null terminated array of char
*/
double StringSim::jaro_winkler_distance(const char *s, const char *a) {
	int i, j, l;
	int m = 0, t = 0;
	int sl = strlen(s);
	int al = strlen(a);

	int* sflags = new int[sl];
	int* aflags = new int[al];

	int range = max(0, max(sl, al) / 2 - 1);
	double dw;

	if (!sl || !al)
		return 0.0;

	for (i = 0; i < al; i++)
		aflags[i] = 0;

	for (i = 0; i < sl; i++)
		sflags[i] = 0;

	/* calculate matching characters */
	for (i = 0; i < al; i++) {
		for (j = max(i - range, 0), l = min(i + range + 1, sl); j < l; j++) {
			if (a[i] == s[j] && !sflags[j]) {
				sflags[j] = 1;
				aflags[i] = 1;
				m++;
				break;
			}
		}
	}

	if (!m)
		return 0.0;

	/* calculate character transpositions */
	l = 0;
	for (i = 0; i < al; i++) {
		if (aflags[i] == 1) {
			for (j = l; j < sl; j++) {
				if (sflags[j] == 1) {
					l = j + 1;
					break;
				}
			}
			if (a[i] != s[j])
				t++;
		}
	}
	t /= 2;

	/* Jaro distance */
	dw = (((double)m / sl) + ((double)m / al) + ((double)(m - t) / m)) / 3.0;

	/* calculate common string prefix up to 4 chars */
	l = 0;
	for (i = 0; i < min(min(sl, al), 4); i++)
	if (s[i] == a[i])
		l++;

	/* Jaro-Winkler distance */
	dw = dw + (l * SCALING_FACTOR * (1 - dw));

	return dw;
}

