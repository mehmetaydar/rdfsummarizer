#ifndef _GRAPH_H
#define _GRAPH_H

// Graph: a generic graph model for rdf data, implemented as a vector of edge lists.

#include "Util.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include<set>
#include<limits.h>
#include "StringSim.h"
#include "Conf.h"
#define MAXIMUM INT_MAX
using namespace std;
/****
Graph file format:
	There are two types of graph file formats we do accept:
	 triple:
		RDF graph file in NT format. If this input format is specified the program 
		pre-processes the file and generates a flat formatted version of the graph file along
		with a mapping file that include (vertex id, vertex URI) and (property id, property URI) pairs
	 flat: 
		This is the pre-processed format of the RDF file that the program can read.  
		The first line must be the string "graph_for_greach" (for silly historical reasons)
		The second line is a single integer, number of nodes(vertices) in the graph
		The third line a single integer, number of properties(labels)(predicates) in the graph
		The fourth line a single integer, number of triples in the graph
		The remaining lines are edge list of the RDF graph of the form:	  
			subject_id: predicate_id1= object_id1 object_id2: predicate_id2= object_id3: ... #
			that represents the following triples: (subject_id, predicate_id1, object_id1), (subject_id, predicate_id1, object_id2) and (subject_id, predicate_id2, object_id3)
			where subject_id is the source node id. object_id1 and object_id2 are the neighbor node ids of subject_id. And predicate_id1, predicate_id2 are property ids
		  Note that vertexes and properties are represented with their ids
		  Node and property ids must be integers, numbered from 0.

Since RDF if a directed graph the input graph is treated as directed graph. Undirected graphs are not supported.

*/

typedef vector<int> VectIntType;
typedef vector<float> VectFloatType;
typedef vector<VectIntType> VectVectIntType;
typedef vector<VectFloatType> VectVectFloatType;

/* 
 * represents incoming edges to a vertex(dest). In this case vertexes in the "src" vector are connected to the "dest" with the predicte "label_id"
 * example triples: (src1, label_id, dest), (src2, label_id, dest), ... (srcN, label_id,dest)
*/
struct EdgeIn{
	VectIntType src;
	int dest;
	int label_id;
};

/*
 * represents outgoing edges from a vertex(src). In this case vertex "src" is connected to the vertexes in the "dest" vector with the predicate "label_id"
 * example triples: (src, label_id, dest1), (src, label_id, dest2), ... (src, label_id,destN)
*/
struct EdgeOut{
	int src;
	VectIntType dest;
	int label_id;
};
/*
  Data type to represent edge in map
  key = label_id, value EdgeIn struct
  used to keep list of EdgeIns associated with the specified property
*/
typedef map<int,EdgeIn> EdgeInMap;//key = label_id, value EdgeIn struct
/*
 Data type to represent edge out map
 key = label_id, value EdgeOut struct
 used to keep list of EdgeOuts associated with the specified property
*/
typedef map<int,EdgeOut> EdgeOutMap;

/*
Data type to represent Nodes. Vertexes.
*/
struct Vertex {
	float weight; //mehmet ,  don't know where this is being used
	EdgeInMap edge_in_map;
	EdgeOutMap edge_out_map;
	int in_degree;//#total number of incoming edges to the vertex
	int degree; //#total number outgoing edges from the vertex, #out_degree
	
	bool valueNode;//whether it is a value node(literal node) or not. Value nodes are not URI. For example names, dates, numerical values are literal nodes
	string valueNode_label;//the actual value part of the value node. For example for "Annular or Degenerative Disease"^^<http://www.w3.org/2001/XMLSchema#string> the value_label is: "Annular or Degenerative Disease"
	string valueNode_label_clean; //removed punctuations / bad words to lower / for string sim
	string valueNode_data_type;//the datatype part of the value node. For example for "Annular or Degenerative Disease"^^<http://www.w3.org/2001/XMLSchema#string> the value_data_type is: http://www.w3.org/2001/XMLSchema#string


	set<string> words;// set of each of the literal's nodes' each words
	int totalNumWord_Occurences = 0; //total number of string words incuding dups

	float tot_literal_importance_weight = 0;

	//map<int, float> pairCommonLabelImportancyMap;
};

/*
Data type to represent string words, their occurence counts and importancy both in the whole data set and also for each Vertex node
*/
struct StringWord{
	int NumOfOccurence_InDataset = 0; //total num of occurence in the whole dataset / incuding dups in each document
	map<int, int> NumOfOccurence_PerVertex; // total number of occurence of the string word per vertex id, key= vertex_id, value = occurence count in that vertex
	map<int, float> Importancy_PerVertex; // the word importancy per each vertex/ based on tf-idf / key=vertex_id, value = importancy of the word for the vertex
};

/*
Data type to represent Triples
*/
struct Triple{
	//For the triple: (src, label_id, dest)
	int src;//subject vertex id
	int label_id;//predicate label_id
	int dest;//object vertex id

	//sort goes by label_id asc, src asc, dest asc
	bool operator<(const struct Triple & a) const {
		if( label_id < a.label_id) return true;
		else if( label_id > a.label_id) return false;
		if(src < a.src ) return true;
		else if(src > a.src ) return false;
		if(dest < a.dest ) return true;
		else if(dest > a.dest ) return false;
		return false;
	}
};
typedef vector<Triple> TripleList;//Data type to represent list of triples in the Graph

/*
 The data structure, for each label the sorted list of related vertex ids as subject
 will be needed to get the vertex pairs that share at least one common labels
 this is for out label
*/
typedef set<int> LabelVertexes;

/*
 The data structure, for each label the sorted list of related vertex ids as objects
 will be needed to get the LabelProps.totDiffObjects = for(p=>{o1,o2,o3}) = len(Objects) where each objects are different
 kind of similar to LabelVertexes but this is for in labels
*/
 typedef set<int> LabelInVertexes;

typedef set<string> NoiseLabels; //ignore this labels. because they occur frequently
typedef set<string> NoiseWords; //ignore this words in the string similarity calculation. because they occur frequently ( like is, and them etc )
typedef map<string,int> LabelNameToIntMapping;//(label name, label_id )
typedef vector<string> VertexIntToNameMapping;//vertex id , vertex name
typedef vector<string> LabelIntToNameMapping;//label id, label name

/*
Data type to represent Labels, (properties, predicates )
*/
struct Label {
	float NumOfOccurence; // = increment in AddTriple() method for each label
	float TotDiffObjects; //= for(p=>{o1,o2,o3}) = len(Objects) where each objects are different ( get this from len( lbIV[label_id] )	
	bool isFrequentLabel; //whether this label is frequent that it will be ignored for pair identification
	LabelVertexes OV;//vertexes that has the label as an outgoing edge
	LabelVertexes OV2;//vertexes that has the label as an outgoing edge ( this is temorary solution to find the common pairs - will be purged )
	LabelInVertexes IV;//vertexes that has the label as an incoming edge
		
	float frequency;  //= NumOfOccurence/NumOfTriples
	float uniqueness;// = totDiffObjects/NumOfOccurence
	float user_given_sim_weight;//not being used yet

	//note that we are not calculating property importancy for each of the pair. Please see PairO.pairCommonLabelImportancyMap 
	float calculated_sim_weight;//not being used yet. importancy weight to be used in the similarity calculation for the property
	
};

typedef vector<Label> LabelList;

/*
  The data structure to store the Vertex pairs that share >= $threshold percent common properties
*/
struct PairO{
	int i; //Id for the first vertex
	int j; //Id for the second vertex
	       //always i > j	

	//ref nodes are only used when both the vl[i] and vl[j] nodes are value Nodes
	int ref_i = -1; //source reference node Id for the first vertex
	int ref_j = -1; //source reference node Id for the second vertex
	//always ref_i > ref_j	if set


	bool both_value_nodes = false; //wheter these two nodes are both value nodes or not, by default false
	bool include_in_classification = true;//whether to include them in classification or not. by default = true. for example for value nodes pairs this should be set to false
	bool include_in_sim_iterations = true;//whether to include them in similarity calculation iterations or not. by default = true. for example for value nodes pairs this should be set to false

	VectIntType O_lb_intersect;//The intersection list of predicates of i and j
	VectIntType O_lb_union;//The union list of predicates of i and j
	int size_O_lb_intersect;//the size of The intersection list of predicates of i and j
	int size_O_lb_union;//the size of The union list of predicates of i and j
	float DisSimilarity;//the dis-similarit between (i,j) nodes . Note that similarity of i and j is equal to (1-DisSimilarity)

	map<int,float> pairCommonLabelImportancyMap;// for the given label_id this represents the importancy of the common label(predicate)
	float prop_importance_ratio; //(tot_common_importancy_weights/tot_importancy_weights)->if using property importance then instead of taking jaccard into acctount take this into account in similarity calculation

	//sort ASC by dis-similarity = sor DESC by similarity
	bool operator<(const struct PairO & a) const {
		return DisSimilarity < a.DisSimilarity;
	}
};
typedef vector<PairO> PairOList;//The list of Vertex pairs that share common properties. The pairs in this list will be input to the OurSim calculations
typedef map<string,int> PairOMap;// given i_j get the pairO struct for the given i_j

/* The Actual Graph
	//main graph array. For example GRA[vid] will represent the Vertex that has id=vid
	                    And GRA[vid].edge_in_map = for the vertex(vertex id=vid) represents the incoming edges to vertex=vid 
	                    And GRA[vid].edge_out_map = for the vertex(vertex id=vid) represents the outgoing edges to vertex=vid 
	                    And GRA[vid].edge_out_map[label_id] will represent a destination vertex reached from the vertex=vid by the predicate=label_id (Triple = (vid, label_id, destination vertex id )
*/
typedef vector<Vertex> GRA;

typedef map<string,int> VertexNameToIntMapping;//(key=vertex_name, value=Vertex id) - used for encoding decoding from/to vertex URI(name) to id
typedef map<int, EdgeOutMap > TripleMapping;//(key=vertex_id, value=EdgeOutMap. This is used to find out the list of triples associated with the corresponding vertex


/*
 This is the data structure used to represent a Cluster
 After the similarities calculated the similar vertexes(nodes) will be classified within the same Cluster  
*/
struct Cluster {
	string name;
	VectIntType members;//the list of vertex ids within the cluster, cluster members, nodes
	int size; //number of nodes in the cluster
	EdgeInMap edge_in_map; //to store the incoming edges to the cluster from other clusters ( cluster relations )
	EdgeOutMap edge_out_map;// to store the outgoing edges from the cluster to the other clusters ( cluster relations )
	int in_degree;//#total number of incoming edges
	int degree; //will represent the out_degree, total number of outgoing edges
	
	map<int, map<int, float>> propertyStabilityByRefCluster; //keeps the stability by property and class, c1(fixed), p, c2 = a stability ratio
	float stability_ratio;  // stability_ration means how stable the Cluster is
							// Used to measure the quality of your clustering results
						   // Stability stands for:
												// if a member in a cluster(clusterA) makes a connection to a member in another cluster(clusterB) 
												// Then all the other members in clusterA makes the same connection(with the same predicate) to cluster(clusterB)
												// Or none of the members in clusterA making a connection to the clusterB
												// Kind of similar to Bisimilation idea
												// stability_ratio is calcualted as (cluster degree / number of outgoing connections suppose to happen ), 
	                      
	float propery_coverage_ratio; // property_coverage_ratio stands for:
								 // Used to measure the quality of your clustering results
								 // property coverage ratio: the percantage of the properties of the members of the cluster that went into the calculation for the cluster relations
								// property_coverage_ratio is calculated as the ( total numbers of outgoing edges of the members divided by cluster degree
	/*
	stability ratio and property coverage ratio tell you how good your results are	
	*/
	float cRMSD; //Root-mean-square deviation
};
typedef vector<Cluster> SetOfClusters; // the list of clusters generated
typedef map<int,int> VertexToClusterMap;// the data sturcture that used to indicate which vertex belongs to which cluster

class Graph {
private:	
	GRA vl; // vertexes list	
	int num_iri_nodes = 0;
	int num_value_nodes = 0;
	TripleList tl; //the triple list
	LabelList ll; //label list
	NoiseLabels Nlb;//ignore this labels
	NoiseWords Nw;//ignore this labels
	int vsize;//number of vertexes in the graph
	
	//will be used if we need to pre-process the input file format
	VertexNameToIntMapping vmap;
	LabelNameToIntMapping lmap;
	VertexIntToNameMapping vrmap;
	LabelIntToNameMapping lrmap;
	TripleMapping tmap;
	int Triple_Count_From_Triple_File;

	string GraphFileName;

	PairOList pairOList;
	PairOMap pairOMap;

	SetOfClusters setClusters;
	int numGoodClusters;//number of clusters that has a size > 1
	int numEdgesInSummaryGraph;
	int numGoodEdgesInSummaryGraph;
	float summaryGraphStability; //calculated summary graph ( clusters and their relations) stability
	float summaryGraph_cRMSD;
	float numGoodClusteredNodes = 0; //nodes that were put in a cluster which has size>1
	VertexToClusterMap vToClusterMap;
	
	StringSim stringSim;//reference to the String similarity library. will be used for string similarity functions
	map<string, StringWord> stringWords; // properties of all the string words in the whole dataset
public:	
	int DEF_max_label_occurence; //if a property is being used more than this number then it is frequent	
	int pairOMaxLenCommonLabels ;//max common label lenght of pairOList, being used in the property importancy calculation
	Conf conf;

	Graph();
	Graph(int);
	Graph(GRA&);
	Graph(Conf &conf ,istream& in, string GraphFileName_, string format, string NoiseLabelsFileName, string NoiseWordsFileName, string MapGraphFileName);//This is the one being called from the main.cpp
	~Graph();
	void readGraph(istream& ,string format);
	void readGraphFlat(istream&);
	void readNoiseLabels(string);
	void readNoiseWords(string);
	void readGraphTriple(istream&);
	void readGraphCSV(istream&);
	void readReverseMapping(string);//read the vertex, label names from the pre-processed file

	//after reading the graph, calculate each label weights for similarity calculation
	void calculateLabelsSimWeight();
	void setLabelImportanciesForPairs();//calculate label importancy for the given pair and label_id

	void setStringLiteralImportanciesForVertexes();//calculate label importancy for the given vertex / currently based on term frequency inverted index
	void PrintStringLiteralImportanciesForVertexes(ostream& out, int prec);//print string literal importancies
	void setStringLiteralCounts(); //setting the counts and props for string literals

	void generateCommonPairs(float);//process the vertexes and generates PairOList;
	
	void readCommonPairs(istream&);//read common pairs from a file and generates PairOList;

	void generateCommonPairs_ForVertexes(istream&,float);//to generate common pairs just for the pairs which are related to a specific list of nodes

	void addPair(int i, int j, bool checkLabels, bool initializeSim, int ref_vidi, int ref_vidj);//adding to pairOList a new pair
	//PairO& getPair(int i, int j);//get Pait with having i and j vertexes
	string getPairKey(int i, int j, int ref_i, int ref_j);
	float getPairSim(int i, int j, bool addIfNotExists, int ref_vidi, int ref_vidj);
	void pairLists(const VectIntType &c1, const VectIntType &c2, bool checkLabels, bool initializeSim); //pair to lists c1 X c2
	bool IsFrequentLabel(int lid);

	void showReductionRates(ostream&, int num_orig_vertices, int num_orig_edges);

	PairOList &getPairOList(){
		return pairOList;
	}
	LabelList &getLabelList(){
		return ll;
	}
	VertexIntToNameMapping &getVrmap(){
		return vrmap;
	}
	LabelIntToNameMapping &getLrmap(){
		return lrmap;
	}
	map<string, StringWord> &getStringWords(){
		return stringWords;
	}
	//calculates string similarity, was causing error if it was inside SitringSim
	double similJACCARD_WITH_TFIDF(int vid1, int vid2, string s1, string s2);

	bool classifyUnClusteredNodes = true;
	float findOptimumClustering(ostream& out, int formula, float min_threshold, float max_threshold, int num_try, float last_opt_value, float opt_value_threshold, string ClusterVerificationFileName);//will dynamicallay set the dissimilarity threshold according to the generated clusters stability and will generate the clusters
	void resetClusters();//reset the clusters

	void classifyDataSet(float cluster_member_similarity_threshold);//for final classification
	void generateClusterRelations();//to generate RDFS schema, adding edges between clusters
	void nameClusters();//name the clusters
	void calculateClustersRMSD(); //calculate generated clusters Root-mean-square deviation
	float verifyClusterResults(string ClusterVerificationFileName, float cluster_member_similarity_threshold); //given a ClusterVerificationFileName(contains real cluster results), calculate and print how correct your clusters are
	void writeClusters(ostream&, float);
	void writeClustersJSON(ostream&, float);

	void writeMappins();
	void writeGraph(ostream&);
	void printGraph();
	void addVertex(int);
	void addTriple(int, int, int);

	vector<int> getOLbIntersect(int, int);
	vector<int> getOLbUnion(int, int);
	vector<int> getILbIntersect(int, int);
	vector<int> getILbUnion(int, int);

	void PrintCommonPairs( ostream& out, int prec);//print pair sims
	void PrintPropertyImportancyByPairs( ostream& out, int prec);//print label(property) importancy by pairs
		
	int num_vertices();
	int num_edges();
	GRA& vertices();
	EdgeOut& out_edges(int,int);
	EdgeIn& in_edges(int,int);
	int out_degree(int);
	int in_degree(int);	
	bool hasEdge(int, int);//not being used
	Graph& operator=(const Graph&);
	Vertex& operator[](const int);
	
	void sortDirectedEdges (); // victor wrote this. mehmet modified it with our data structure. is being called in main.cpp
};
#endif
