// SimMatrix: a class for a node-node similarity matrix, with functions for RoleSim, SimRank, and related algorithms.
#ifndef _SIMMATRIX_H
#define _SIMMATRIX_H

#include "Util.h"
#include "Graph.h"

#define TRUE 1
#define FALSE 0


//MatchItem is used to record a matching pair (calcRoleSimGreedy, find*TopPairs)
struct MatchItem {
public:
	int		x;
	int		y;
	float	weight;
};
struct MatchLessThan {
	bool operator() (MatchItem a, MatchItem b) {
		return (a.weight < b.weight);
	}
};
struct MatchGreaterThan {
	bool operator() (MatchItem a, MatchItem b) {
		return (a.weight > b.weight);
	}
};
typedef vector<MatchItem> MatchItemVec;

class SimMatrix
{
private:	
	int N;	

public:
	SimMatrix(void);
	SimMatrix(int N);//this is being called from the main
	virtual ~SimMatrix(void);
			
	bool UpdateOurSimGreedy          ( Graph& g, float beta, float inWeight, float threshold );
	
	//mod by mehmet	
	float calcRoleSimGreedy2(Graph& g, int ref_vidi, int ref_vidj, const VectIntType& N_u, const VectIntType& N_v);//added by mehmet. get sims from pairOList;
	//float calcRoleSimGreedy( SimMatrix& PreMatrix, const VectIntType& N_u, const VectIntType& N_v, bool divMax );
		
};

#endif
