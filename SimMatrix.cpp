// SimMatrix: a class for a node-node similarity matrix, with functions for RoleSim, SimRank, and related algorithms.
#include "SimMatrix.h"
#include <queue>

/* 
*  This is where the pair similarity methods and iterations are being defined
*/

SimMatrix::SimMatrix(void)
{
	N = 0;	
}

//mehmet, this is being called from the main
////////////////////////////////////////////////////////////////////////////
SimMatrix::SimMatrix(int size)
{
	N = size;		
}
////////////////////////////////////////////////////////////////////////////
SimMatrix::~SimMatrix(void) { }

////////////////////////////////////////////////////////////////////////
////    OurSim         										    ////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/*
this is the actual ourSim calculation being called from the main
mehmet. added. calculates OurSim for only the nodes that shares a common label (propery). 
*/
bool SimMatrix::UpdateOurSimGreedy( Graph& g, float beta, float inWeight, float threshold )
{
	float previousSim,newSim;
	float diff;
	float maxDiff = 0;
	double totDiff = 0;
	double Sum = 0;
	
	cout << "UpdateOurSimGreedy" << endl;
	VectIntType I_i, I_j, O_i, O_j, I_lb_intersect,I_lb_union, O_lb_intersect,O_lb_union;
	int sizeIi, sizeIj, sizeOi, sizeOj, size_I_lb_intersect,size_I_lb_union, size_O_lb_intersect,size_O_lb_union;
	float outWeight = 1 - inWeight;
	float denomI, denomO;
	
	PairOList &pairOList = g.getPairOList();
	LabelList &ll = g.getLabelList();
	int i,j;
	int pair_index=0;
	float outSimilarity,inSimilarity,outSimilarity_by_common_label;
	float out_common_label_importancy_for_pair, sum_out_common_label_importancy_for_pair;
	//for (  auto it = pairOList.begin(); it != pairOList.end(); ++it )
	//cout<< "PairOList size: "<< pairOList.size() << endl;
	for (pair_index = 0; pair_index< pairOList.size() && pairOList[pair_index].include_in_sim_iterations; pair_index++)
	{		
		//PairO &pair = pairOList[pair_index];
		i = pairOList[pair_index].i;
		j = pairOList[pair_index].j;
	//	cout << "pair_index: " << pair_index << "i: " << i << " j: " << j << endl;
		
		previousSim = (1 - pairOList[pair_index].DisSimilarity);

		//cout <<"Pair Sim before: "<<previousSim<<endl;

		/*
		note mehmet
		here added another loop for ( FOR EACH COMMON LABEL )
		2 for loops consequently, one for incoming one for outgoing
		*/
		O_lb_intersect = pairOList[pair_index].O_lb_intersect;
		size_O_lb_intersect = pairOList[pair_index].size_O_lb_intersect;
		size_O_lb_union = pairOList[pair_index].size_O_lb_union;

		outSimilarity = inSimilarity = 0;
		sizeOi = sizeOj =0;

		if(size_O_lb_union > 0) //to overcome division by zero
		{
			int common_O_label = -1;//common outgoing label
			for(int k = 0;k< size_O_lb_intersect; k++)
			{
				//get neighbors reached by the common label
				common_O_label = O_lb_intersect[k];
				O_i = g[i].edge_out_map[common_O_label].dest;
				O_j = g[j].edge_out_map[common_O_label].dest;

				outSimilarity_by_common_label = calcRoleSimGreedy2(g, i, j, O_i, O_j);				
				out_common_label_importancy_for_pair = pairOList[pair_index].pairCommonLabelImportancyMap[common_O_label];
				
				//outSimilarity_by_common_label = 1; //test,remove this
				//out_common_label_importancy_for_pair = out_common_label_importancy_for_pair * (size_O_lb_intersect ); 

				outSimilarity_by_common_label = outSimilarity_by_common_label * out_common_label_importancy_for_pair;

				outSimilarity+= outSimilarity_by_common_label;

				sizeOi += O_i.size();
				sizeOj += O_j.size();
				//cout << " k: " << k << endl;
			}

			//outSimilarity = (outSimilarity*(float)size_O_lb_intersect)/(float)g.pairOMaxLenCommonLabels ; //adding the pair size_O_lb_intersect factor. without this pairs that have less number of common properties will have more similarity(odd)

			//used to be this
			//outSimilarity = outSimilarity  * ( (float)size_O_lb_intersect / (float)size_O_lb_union );

			//changed to
			outSimilarity = outSimilarity  * pairOList[pair_index].prop_importance_ratio; //if not using auto-weight this will be equal to jaccard else this will be equal to the ratio of the common importance to the total union importance

			//cout << "Out Sim after: " << outSimilarity << endl;
		}
		if(inWeight <= 0 )//this is the actual case for now( ignoring incoming edges )
		{
			if(size_O_lb_union > 0){
				newSim = (1-beta)*(outSimilarity) + beta;
			}
			else
				//important for leaf nodes such as value nodes don't attempt to calculate the neighborhood sims cos there is no neighbor
				newSim = previousSim;
		}
		else //for now inComing edges are not being used
		{
			throw std::logic_error("Incoming edges properties are not computed in the g.pairIMap yet");
		}

		//cout <<"Pair Sim after: "<<newSim<<endl;

		pairOList[pair_index].DisSimilarity = (1 - newSim);

		diff = abs( newSim - previousSim );//similarity difference with previous iteration or state for the pair
		totDiff += 2*diff;		// count each cell twice
		if (diff > maxDiff) {
			maxDiff = diff;
		}
		// count the cell of the older matrix twice
		Sum = Sum + 2*previousSim;
	}

	cout << "maxDiff = " << maxDiff << ", totDiff = " << totDiff << ", Sum*threshold = " << Sum*threshold << endl;
	if( totDiff < Sum*threshold ) {
		return true;
	}
	else {
		return false;
	}
}

////////////////////////////////////////////////////////////////////////
////    RoleSim 
////    (with small modifications: instead of getting the pairs form Matrix, we are getting the pairs from a pre-defined pair map data structure /////
////	reference: Ruoming Jin, Victor E. Lee, and Hui Hong. 2011. Axiomatic ranking of network role similarity. In Proceedings of the 17th ACM SIGKDD international conference on Knowledge discovery and data mining (KDD '11). ACM, New York, NY, USA, 922-930         										    ////
////////////////////////////////////////////////////////////////////////
float SimMatrix::calcRoleSimGreedy2(Graph& g, int ref_vidi, int ref_vidj, const VectIntType& N_i, const VectIntType& N_j)
{
	vector<float> Weight;
	float weight;

	MatchItem matchItem;
	priority_queue<MatchItem, vector<MatchItem>, MatchLessThan> matchQueue;

	int size_i = N_i.size();
	int size_j = N_j.size();
	if( size_i == 0 || size_j == 0 )
	{
		return 0.0;
	}

	VectIntType::const_iterator iItr, jItr;
	matchItem.x = 0;
	for (iItr = N_i.begin(); iItr != N_i.end(); iItr++)
	{
		matchItem.y = 0;
		for (jItr = N_j.begin(); jItr != N_j.end(); jItr++)
		{
			if (*iItr == *jItr) //value nodes has a different calculation (remember about string words importancy )
			{
				matchItem.weight = 1.0;
			}
			else {
				//matchItem.weight = (1 - g.getPair(*iItr, *jItr).DisSimilarity);
				matchItem.weight = g.getPairSim(*iItr, *jItr, true, ref_vidi, ref_vidj);
			}

			if (matchItem.weight > 0.0)
			{
				matchQueue.push(matchItem);
			}

			matchItem.y += 1;
		}
		matchItem.x += 1;
	}

	// Find the top weighted match-pairs
	VectIntType Flag1 = VectIntType( size_i, FALSE );
	VectIntType Flag2 = VectIntType( size_j, FALSE );
	float numerator = 0.0;
	int matchSize = min(size_i, size_j);
	int matchCount = 0;

	while (!matchQueue.empty() && matchCount < matchSize)
	{
		matchItem = matchQueue.top();
		if ( Flag1[matchItem.x]==FALSE && Flag2[matchItem.y]==FALSE )
		{
			Flag1[matchItem.x] = TRUE;
			Flag2[matchItem.y]= TRUE;
			numerator += matchItem.weight;
			matchCount++;
		}
		matchQueue.pop();
	}

	//if (divideByMax)
	//{
	weight = numerator / (float)(size_i > size_j ? size_i : size_j);
	//}
	//else
	//{
	//	weight = numerator / sqrt ( (float)(size_i * size_j) );
	//}
	//assert(weight<1.1);
	return weight;
}


/*
mehmet, instead of this function we use calcRoleSimGreedy2
serkan: please compare this function with calcRoleSimGreedy to verify that we are preserving the main functionalities of RoleSim maximal matching
*/
/////////////////////////////////////////////////////////////////////////
/*
float SimMatrix::calcRoleSimGreedy(SimMatrix& PreMatrix, const VectIntType& N_i, const VectIntType& N_j, bool divMax )
{
	vector<float> Weight;
	float weight;

	MatchItem matchItem;
	priority_queue<MatchItem, vector<MatchItem>, MatchLessThan> matchQueue;

	int size_i = N_i.size();
	int size_j = N_j.size();
	if( size_i == 0 || size_j == 0 )
	{
		return 0.0;
	}

	SimMatrixType& mPrev = PreMatrix.matrix();

	VectIntType::const_iterator iItr, jItr;
	matchItem.x = 0;
	for (iItr = N_i.begin(); iItr != N_i.end(); iItr++)
	{
		matchItem.y = 0;
		for (jItr = N_j.begin(); jItr != N_j.end(); jItr++)
		{
			if (*iItr == *jItr)
			{
				matchItem.weight = 1.0;
			}
			else if (*iItr > *jItr)
			{
				matchItem.weight = mPrev[*iItr][*jItr];
			}
			else {
				matchItem.weight = mPrev[*jItr][*iItr];
			}

			if (matchItem.weight > 0.0)
			{
				matchQueue.push(matchItem);
			}

			matchItem.y += 1;
		}
		matchItem.x += 1;
	}

	// Find the top weighted match-pairs
	VectIntType Flag1 = VectIntType( size_i, FALSE );
	VectIntType Flag2 = VectIntType( size_j, FALSE );
	float numerator = 0.0;
	int matchSize = min(size_i, size_j);
	int matchCount = 0;

	while (!matchQueue.empty() && matchCount < matchSize)
	{
		matchItem = matchQueue.top();
		if ( Flag1[matchItem.x]==FALSE && Flag2[matchItem.y]==FALSE )
		{
			Flag1[matchItem.x] = TRUE;
			Flag2[matchItem.y]= TRUE;
			numerator += matchItem.weight;
			matchCount++;
		}
		matchQueue.pop();
	}

	//if (divideByMax)
	//{
	weight = numerator / (size_i > size_j ? size_i : size_j);
	//}
	//else
	//{
	//	weight = numerator / sqrt ( (float)(size_i * size_j) );
	//}
	//assert(weight<1.1);
	return weight;
}*/
