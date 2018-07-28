----------
- conf is okay now
. in graph.cpp in line 168 added in setLabelImportanciesForPairs add if (false /*say if auto_prop_importancy*/) / that does not auto-calculate property importance
   +made this conf based

. in graph.cpp in line 1035 added //pairSim = 1; //here overwriding the string similarity / basically ignoring string sim
   +made this conf based
. in graph.cpp in line 168 added in setLabelImportanciesForPairs add if (false /*say if auto_prop_importancy*/) / that does not auto-calculate property importance
   +made this conf based 
. in graph.cpp in line 220 added in setStringLiteralImportanciesForVertexes if (false)  //here add if using string sim
   +made this conf based
. in graph.cpp line 1136 in getPairSim added if (vl[vid1].valueNode && vl[vid2].valueNode) //this will not add value nodes as pairs
													return 1.0; //they are both value nodes so just return 1.0 for their similarities without adding them
  +made this conf based

-------
. Going back to paper's version 1:
   https://xp-dev.com/svn/RoleSimJaccard/branches/clean-v1 +
 . excluding string similarity: 
	- in Graph.cpp getPairSim , do not add pair if it does not exists. Or do not add pairs if both value nodes
	- this way you also excluded the words importancies
 . excluding importance of properties and words: 
	In Graph.cpp setLabelImportanciesForPairs, proportionally equalize the weights of each properyt
 . RDFS and stability measure can stay

 - So decided to do everything in the configuration. 
  . Create a conf class and pass it around. do it map based.

----------------------------
- todo: for unclustered resource nodes represent them as one cluster - in this case: <HearhChamberStatus:1>: OK
- cleaned the demonstration file in the demonstration/show0.txt file ( excluded unclustered nodes): OK
---------------
- When writing the clusters to a file also write the success rate if exists: OK
-------------------
- I would say for now forget about stability, prop coverage and hieracial. but you can do verificat+ion: such as in the verified cluster find pairs() if pairs sim is
  greater >= then your cluster sim threshold then success!
  . Works made it work both way: ( diff(real,generated) + diff(generated, real) ) / 2
---------------------------
- findings: stability idea and property coverage and verification idea is hard without hieracial clustering/ but hieracial clustering is another hard one too
- verification idea:
  read a cluster from verification, and find how many of them are grouped in different clusters:
   get max. so lets say you are suppose to have 3 clusters as 20, 10 and 5 and / you found max: 8, 7 and 5: then: 8*20+7*10+5*5/20*20+10*10+5*5 = ?
  . However lets say verification surgicalProc class is one class and you group yours in 3, then your results look bad. 
  . But if you do hieracial clustering then your verification results will look better
  . So think more on hieracial clustering based on similarity, stability and property coverage ratio
Stayed in: Graph.cpp.verifyClusterResults
. Todo: to verify the clustering results, create a verify data set. for example Event.encounters =<set of events which are encounters>, <heartrytms> = <set of heart rytms>, <surgicalProcedure>.attrium = <set of surgical procedures which are attriums>
   . run the clustering method with different clustering thresholds until you find the optimum with verification
   . but don't do re-clustering with this. use this to just to calculate how successful your results are
. Todo: re-visit the clustering properties such as stability and property coverage ratios, think if you can re-cluster based on that ?
  - run the clustering method with different clustering thresholds until you find the optimum with verification
  - you can use these properties for re-clustering
  
-----------------------------
. added naming the clusters 
-----------------------------
. added noise words and excluding noise words in the string similarity calculation
---------------
. In calculating string sim there is a bug. once it calculates sim for two nodes (s1, s2) it hash them as s1id, s2id however it should pair them with their reference nodes
  . fixed
. continue on your sample extraction

----------------------------------
. I think I fixed the similarity less or more issue did the correct weighing in Oursim calculation
 - now you need to re-define string measures with type with just strings etc

. Now investigating why the similarity results are so small:
 . For testing purposes added outSimilarity_by_common_label = 1; //test,remove this and out_common_label_importancy_for_pair = 1 / (float)size_O_lb_intersect;
         . with those in place the similarity before and after should not change
. Excluded value nodes from both similarity calculations iterations and clustering functions

.Adjusted makefile accordingly( I guess makefile will be used to make it compile on linux)
.Removed PM files
. Finished Graph.cpp	
. cleaned graph.h+
. stayed in Step 4 in main.cpp +
  . cleaned simmatrix.h simmatrix.cpp and main.cpp
- Code Cleanup
	. created a branch "branches/clean1" and swithced my working copy
	. now need to clean up the code
- Implement Jaro winkler
- Review TF - IDF
- Find Topk for a given node

- Added the option for target vertexes
- Added string similarity option for values nodes



- Need to add label importancy
  . have user supply a document for importancy - give weight too user importancy
  after user supplied document, for the remaining props:
  . auto detect property importancy
    - frequently occured props are less important - give weight to frequency
	- props that connect to the most unique values are more important - give weight to uniqueness
	  for example if (s,p,o) and for p we have 70% uniqueness then , sort p's by uniqueness and assign weight 
  
      LabelVertexes lbOV;//vertexes that has the label as an outgoing edge
	  //for each label the sorted list of related vertex ids
	  //will be needed to get the vertex pairs that share at least one common labels
	  typedef vector<set<int>> LabelVertexes;
	  {1,2,3}    => label10
	  {1,3,10}   => label11 
	  {2,7,8}    => label12 
	  {s1,s2,s3}  => p

	  NumOfTriples = Triple_Count_From_Triple_File //num triples excluding triples with noise labels

	  struct LabelProps{
		NumOfOccurence = increment in AddTriple() method for each label
		totDiffObjects = for(p=>{o1,o2,o3}) = len(Objects) where each objects are different ( get this from len( lbIV[label_id] )
		frequency = NumOfOccurence/NumOfTriples
		uniqueness = totDiffObjects/NumOfOccurence
		user_given_sim_weight;
		final_sim_weight;
	  }
	  Note: instead of LabelProps above, create a more general struct as "Labels" just like you did for Vertex and Triple structs
	  and add LbOV, LbIV and other related objects to it.
	  
	  NoiseLabels Nlb;//ignore this labels
	  typedef set<string> NoiseLabels; //ignore this labels. because they occur frequently

	  LabelNameToIntMapping lmap;
	  LabelIntToNameMapping lrmap;
	  
	  
	  lbIV added;

	  created:

struct Label {
   int NumOfOccurence; // = increment in AddTriple() method for each label   
   int TotDiffObjects; //= for(p=>{o1,o2,o3}) = len(Objects) where each objects are different ( get this from len( lbIV[label_id] )
   float frequency;  //= NumOfOccurence/NumOfTriples
   float uniqueness;// = totDiffObjects/NumOfOccurence
   float user_given_sim_weight;
   float calculated_sim_weight;
   bool isFrequentLabel; //whether this label is frequent that it will be ignored for pair identification
   LabelVertexes OV;//vertexes that has the label as an outgoing edge
   LabelInVertexes IV;//vertexes that has the label as an incoming edge
};
typedef vector<Label> LabelList;

Wrote calculateLabelsSimWeight() function but it is not working properly maybe something about the order it is being called
- num of triples is being calc in ReadGraphTriple but we are only calling calculateLabelsSimWeight when flat file format - so look 
at flat read

--update calculateLabelsSimWeight works now. without user_given_weight

--update
. change frequency to term frequency inverted index
  . tf-idf check wikipedia and tran typifier paper
   
   in your Label.frequency above equals to idf
   to calculate tf you need to do this per each pair and each common label
   foreach pair(u,v)
	tf-idf(common_label,u,v) = [tf(common_label,u)+tf(common_label,v)]/2
	tf-idf(common_label,u) = tf(l,u)*idf(l) = tf(l,u)*log[len(vertexes)/len(Label.OV)]
	idf(l) is calculateable, how do you calculate tf(l,u) which equals to frequency of l in u?
	tf(l,u)= len( u.Edgeoutmap[l].dest )/totTriplesIn(u)
	totTriplesIn(u) = u.degree
	
	- In PairO struct add label importancy as map? : lImportancy[common_label_id] = importancy?
	 . Writing getOrSetLabelImportancyForPairO function
        
		common_label
		1
		 a lw3 
		 b lw2
		 lwt = 1.5
		 (sim(a,b)*3 + sim(a,b)*2)/2 = [ 1.5*sim(a,b) ]*overall_weight= [ 1.5*sim(a,b) ]*1.5/8
		2
		  lwt = 4
		3
		  lwt = 2.5

   - the above is implemented and compiled now needs to be tested
     . not working. in generateCommonPairs(float frequent_propery_threshold) we are deleting the l.OV starting from the beginning. do not delete
	   this. but change it so that you don't modify OV. But the while loop kind of depends on the empytyness of if(vset->size() == 0) continue;
	     - error reason: you should not process an lid if it already has been processed(create lid_processed?)+
		 - solution: created OV2 and did operations on that one+
		 - fixed+
		 
        . convert uniqueness to ignoring the labels that makes too much uniqueness(such as ID fields) and
								ignoring the labels that make very little uniqeness( such as a very common value )(country of birth for turkish citiziens )
		  - this has not been implemented yet

		
. change string similarity to jaro winkler similarity

. with property importancy in place the sim has been calculated but the results are kind of odd
<<<<<<< .mine
  - printed property importancy to a different file ( sparql_university.txt_flat.txt_property_importancy_by_pair.txt )+
  - examine the file. prop importancy calculations look good but common labels and value nodes look odd+
  - fixed this so that more number of common labels have more weights
=======
  - printed property importancy to a different file ( sparql_university.txt_flat.txt_property_importancy_by_pair.txt )
  - examine the file. prop importancy calculations look good but common labels and value nodes look odd

.>>>>>>> .r21
