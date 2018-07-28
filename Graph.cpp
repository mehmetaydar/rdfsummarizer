#include "Graph.h"
// Graph: a generic graph model to represent RDF graphs

#include <sstream>
#include <fstream>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <list>
#include <deque>
#include <assert.h>
#include <utility>
#include <cmath>
#include <iomanip>
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include "SimMatrix.h"
#include "Conf.h"

/****
Graph file format:
	Please see Graph.h file for the graph file format description.
*/

Graph::Graph() {
	//graph = GRA();
	conf = Conf();
	vl = GRA();
	tl = TripleList();
}

Graph::Graph(int size) {
	conf = Conf();
	vsize = size;	
	vl = GRA(size);
	tl = TripleList();
}

Graph::Graph(GRA& g) {
	conf = Conf();
	vsize = g.size();
	vl = g;
	tl = TripleList();
}

//mehmet, this is being called from the main
Graph::Graph(Conf &conf, istream& in, string GraphFileName_, string format, string NoiseLabelsFileName, string NoiseWordsFileName, string MapGraphFileName) {
	this->conf = conf;
	GraphFileName = GraphFileName_;
	if(NoiseLabelsFileName != "")
		readNoiseLabels(NoiseLabelsFileName);
	if (NoiseWordsFileName != "")
		readNoiseWords(NoiseWordsFileName);
	if (format == "flat")
		readReverseMapping(MapGraphFileName);
	readGraph(in,format);
}

Graph::~Graph() {}

/*
  * Printing details about the graph. Not much use. 
  * called when verbose option specified. If you want to print more about the graph enchance writeGraph method
*/
void Graph::printGraph() {
	writeGraph(cout);
}

/*
	* Printing the pairs that share common property(s)
*/
void Graph::PrintCommonPairs(ostream& out, int prec)
{
	cout << "Writing similarities to the file ..."<<endl;
	float num;
	int i, j;

	cout<< "\nSorting the common pair list ..."<<endl;
	sort(pairOList.begin(), pairOList.end());//sorting by pair(i,j) similarity DESC
	cout<< "Sorted the common pair list."<<endl;

	for ( auto it = pairOList.begin(); it != pairOList.end(); ++it )
	{
		//i>j
		i = it->i;
		j = it->j;
		//num = val(i,j); //mod by mehmet later
		num = (1 - it->DisSimilarity);
		out<<"("<<i<<","<<j<<"): ";
		out<< fixed << setprecision(prec) << num  <<endl;

		out<<"("<<vrmap[i]<<","<<vrmap[j]<<"): ";
		out<< fixed << setprecision(prec) << num  <<endl;
	}
}

/*
	Prints property importancy by pair
	Note that property importancy changes by each pair based on definition of the term frequency inverted index
*/
void Graph::PrintPropertyImportancyByPairs(ostream& out, int prec)
{
	cout << "Writing property importancies by pair to the file ..."<<endl;
	float num, property_importancy;
	int i, j, ref_i, ref_j, k, common_O_label;
	for ( auto it = pairOList.begin(); it != pairOList.end(); ++it )
	{
		//i>j
		i = it->i;
		j = it->j;
		ref_i = it->ref_i;
		ref_j = it->ref_j;
		num = (1 - it->DisSimilarity);
		if (!(vl[i].valueNode && vl[j].valueNode))
			out<<"("<<vrmap[i]<<","<<vrmap[j]<<") "<<"("<<i<<","<<j<<") :";
		else
			out << "(" << vrmap[ref_i] << "." << vrmap[i] << " , " << vrmap[ref_j] << "." << vrmap[j] << ") " << "(" << i << "," << j << ") :";
		out<<"	sim: " << fixed << setprecision(prec) << num  <<endl;
		for(k = 0;k< it->O_lb_intersect.size(); k++)
		{
			common_O_label = it->O_lb_intersect[k];
			property_importancy = it->pairCommonLabelImportancyMap[common_O_label];
			out << "	" << lrmap[common_O_label] << " ( " << common_O_label << " ) : " << fixed << setprecision(prec) << property_importancy << "\t" << setprecision(1) << 100 * property_importancy << "%" << "\tRatio: " << fixed << setprecision(prec) << it->prop_importance_ratio * property_importancy << "\t" << setprecision(1) << 100 * it->prop_importance_ratio * property_importancy << "%" << endl;
		}
	}
}

/*
not being used. Discontinued since we decided to use tf-idf
mehmet, added
this will calculate the labels weight, importancy to be used in the similarity calculation
*/
void Graph::calculateLabelsSimWeight()
{
	//find a better place to define these constants
	float DEF_label_sim_disfrequency_weight = 0.5;
	float DEF_label_sim_uniqeness_weight = 0.5;
	cout<<"Calculating label weights which will be used in similarity calculation ..."<<endl;
	Label *lobj;

	//based on pure frequency and uniqueness
	int i;
	for(i =0;i< ll.size();i++)
	{
		lobj = &ll.at(i);
		//Label &lobj = ll[common_label_id];
		lobj->TotDiffObjects = lobj->IV.size();
		lobj->frequency = (float)(lobj->NumOfOccurence)/ (float)Triple_Count_From_Triple_File;
		lobj->uniqueness = (float)(lobj->TotDiffObjects) / (float)(lobj->NumOfOccurence);
		//Serkan - uniqueness here is defined as lack of uniqness and given higher weight for less discriminative properties. We might want to substract from 1, like frequency measure 
		lobj->user_given_sim_weight = 0;//for now automate things

		lobj->calculated_sim_weight = (1-lobj->frequency)*DEF_label_sim_disfrequency_weight + lobj->uniqueness*DEF_label_sim_uniqeness_weight;

		//To disable the property(label) importancy enable the line below
		//lobj->calculated_sim_weight = 1;//disabling label importancy
	}
}

/*
	Applying property importancy by Pair using a measure similar to tf_idf
*/
void Graph::setLabelImportanciesForPairs()
{
	cout << "Setting properties importancies (tf-idf) per pair" << endl;
	float importancy_weight, pair_tf_id, l_idf, i_tf, j_tf, i_tf_idf, j_tf_idf, tot_common_importancy_weights, tot_importancy_weights;
	int i,j,k, common_label_id, label_id;
	for ( auto it = pairOList.begin(); it != pairOList.end() && it->include_in_sim_iterations; ++it )
	{
		//i>j
		i = it->i;
		j = it->j;		
		if (conf.autoWeights())
		{
			//auto-calculating property importancy

			//mod- first calculate for the common props
			tot_common_importancy_weights = 0;
			for (k = 0; k < it->O_lb_intersect.size(); k++)
			{
				common_label_id = it->O_lb_intersect[k];
				Label &lobj = ll[common_label_id];
				//calculating tf-idf for the pair and the common property

				//Note: getting division by zero error. figure out which one you need to use . look above
				l_idf = log(((float)vl.size() / (float)lobj.OV.size())); // inverse document frequency - frequency of the common label(property) in the dataset

				i_tf = ((float)vl[i].edge_out_map[common_label_id].dest.size() / (float)vl[i].degree); // term frequency - frequency of the property in the node i
				i_tf_idf = (i_tf * l_idf); //term frequency-inverse document frequency for the property(common_label_id) and node i

				j_tf = ((float)vl[j].edge_out_map[common_label_id].dest.size() / (float)vl[j].degree);
				j_tf_idf = (j_tf * l_idf);

				pair_tf_id = (float)(i_tf_idf + j_tf_idf) / 2; //average tf_idf for the common label in the pair

				importancy_weight = pair_tf_id;//if you want to add more importancy calculations then calculate and weights them
				tot_common_importancy_weights += importancy_weight;
				it->pairCommonLabelImportancyMap[common_label_id] = importancy_weight;
			}
			for (auto it2 = it->pairCommonLabelImportancyMap.begin(); it2 != it->pairCommonLabelImportancyMap.end(); ++it2)
			{
				it2->second = it2->second / tot_common_importancy_weights;
			}

			//mod- first then calculate for the union props
			tot_importancy_weights = 0;
			for (k = 0; k < it->O_lb_union.size(); k++)
			{
				label_id = it->O_lb_union[k];

				//common label
				if (vl[i].edge_out_map.find(label_id) != vl[i].edge_out_map.end() && vl[j].edge_out_map.find(label_id) != vl[j].edge_out_map.end()){
					continue;
				}

				i_tf_idf = 0;//need to initialize
				j_tf_idf = 0;

				Label &lobj = ll[label_id];
				l_idf = log(((float)vl.size() / (float)lobj.OV.size())); // inverse document frequency - frequency of the label(property) in the dataset
				if (vl[i].edge_out_map.find(label_id) != vl[i].edge_out_map.end()){
					//in [i - j]
					i_tf = ((float)vl[i].edge_out_map[label_id].dest.size() / (float)vl[i].degree); // term frequency - frequency of the property in the node i
					i_tf_idf = (i_tf * l_idf); //term frequency-inverse document frequency for the property(label_id) and node i
				}
				else if (vl[j].edge_out_map.find(label_id) != vl[j].edge_out_map.end()){
					//in [j - i]
					j_tf = ((float)vl[j].edge_out_map[label_id].dest.size() / (float)vl[j].degree);
					j_tf_idf = (j_tf * l_idf);
				}
				pair_tf_id = (float)(i_tf_idf + j_tf_idf) / 2; //average tf_idf for the label in the pair

				importancy_weight = pair_tf_id;//if you want to add more importancy calculations then calculate and weights them
				tot_importancy_weights += importancy_weight;
			}
			tot_importancy_weights = tot_importancy_weights + tot_common_importancy_weights;
			it->prop_importance_ratio = tot_common_importancy_weights / tot_importancy_weights;
		}
		else
		{
			//all properties have equal weight = 1/size_o_lb_intersect
			for (k = 0; k < it->O_lb_intersect.size(); k++)
			{	
				common_label_id = it->O_lb_intersect[k];
				it->pairCommonLabelImportancyMap[common_label_id] = (float)( 1.0 / (float)it->O_lb_intersect.size());
				it->prop_importance_ratio = (float)((float)it->O_lb_intersect.size() / (float)it->size_O_lb_union);
			}
		}
	}
}

/*
Applying property importancy of each string literals in each vertex by using a measure similar to tf_idf to be used in String similarity calculation( jaccard ) 
*/
void Graph::setStringLiteralImportanciesForVertexes()
{
	cout << "Setting string literal importancies for vertexes" << endl;
	std::set<string>::iterator it;
	string word;
	float importancy_weight, word_tf_idf, word_idf, word_tf, tot_importancy_weights;
	int vid, i;
	for (vid = 0; vid < vl.size(); vid++){
		if (conf.autoWeights())  //here add if using string sim
		{ 
			//auto-calculating string word importancies
			Vertex &v = vl[vid];
			tot_importancy_weights = 0;
			set<string> &v_words = vl[vid].words;
			for (it = v_words.begin(); it != v_words.end(); ++it){
				word = *it;
				StringWord &string_word = stringWords[word];
				word_idf = log(((float)vl.size() / (float)string_word.NumOfOccurence_InDataset)); // inverse document frequency - frequency of the literal in the dataset
				word_tf = ((float)string_word.NumOfOccurence_PerVertex[vid] / (float)vl[vid].totalNumWord_Occurences); // term frequency - frequency of the property in the node i
				word_tf_idf = (word_idf * word_tf);

				importancy_weight = word_tf_idf;//if you want to add more importancy calculations then calculate and weights them
				tot_importancy_weights += importancy_weight;
				string_word.Importancy_PerVertex[vid] = importancy_weight;
			}
			v.tot_literal_importance_weight = tot_importancy_weights;
			for (it = v_words.begin(); it != v_words.end(); ++it){
				word = *it;
				StringWord &string_word = stringWords[word];
				string_word.Importancy_PerVertex[vid] = string_word.Importancy_PerVertex[vid] / tot_importancy_weights;
			}
		}
		else{
			Vertex &v = vl[vid];
			set<string> &v_words = vl[vid].words;
			//equalizes each string words importancies
			for (it = v_words.begin(); it != v_words.end(); ++it){
				word = *it;
				StringWord &string_word = stringWords[word];
				string_word.Importancy_PerVertex[vid] = (float)(1.0 / (float)v_words.size());
			}
		}
	}
}
/*
Prints property importancy of each string literals in each vertex by using a measure similar to tf_idf to be used in String similarity calculation( jaccard )
*/
void Graph::PrintStringLiteralImportanciesForVertexes(ostream& out, int prec)
{
	std::set<string>::iterator it;
	string word;
	int vid, i;
	for (vid = 0; vid < vl.size(); vid++){
		Vertex &v = vl[vid];
		if (v.valueNode)
			continue;
		out << "(" << vrmap[vid] << ") " << "(" << vid << ") :" << endl;
		set<string> &v_words = vl[vid].words;
		for (it = v_words.begin(); it != v_words.end(); ++it){
			word = *it;
			StringWord &string_word = stringWords[word];
			out << "	" << word << " : " << setprecision(prec) << string_word.Importancy_PerVertex[vid] << " - " << setprecision(prec) << string_word.Importancy_PerVertex[vid] * 100 << "%" << endl;			
		}
	}
}
void Graph::setStringLiteralCounts(){
	cout << "Setting string literal counts" << endl;
	string word;
	int vid, v_label_id, k, dest_vertex_id;
	for (vid = 0; vid < vl.size(); vid++){
		Vertex &v = vl[vid];
		EdgeOutMap &v_edge_out_map = vl[vid].edge_out_map;
		for (auto it = v_edge_out_map.begin(); it != v_edge_out_map.end(); ++it)
		{
			v_label_id = it->first;// vertex triple label id
			EdgeOut &v_edge_out = it->second;//vertex triple edge outgoing
			//it->first; //label_id
			//it->second;//EdgeOut struct
			for (k = 0; k < it->second.dest.size(); k++)
			{
				dest_vertex_id = it->second.dest[k]; //neighbor vertex id
				Vertex &v_dest = vl[dest_vertex_id];
								
				if (v_dest.valueNode && (v_dest.valueNode_data_type.find("string") != std::string::npos || v_dest.valueNode_data_type == "unknown")){
					stringstream ssin(v_dest.valueNode_label_clean); //removing punctuations like . , ! etc
					while (ssin.good()){
						ssin >> word;
						if (Nw.find(word) != Nw.end()) //the word is a noise word, do not count it
							continue;
						v.words.insert(word);
						v.totalNumWord_Occurences++;
						if (!stringWords.count(word)){//string_word for the word has not been initialized before
							stringWords[word] = StringWord();
						}
						stringWords[word].NumOfOccurence_InDataset++;
						stringWords[word].NumOfOccurence_PerVertex[vid]++;
					}
				}
			}
		}
	}
}
/*
calculates 2 strings similarity based on their jaccard and importancy(by vertexes) in the data set
this was cauising issues when inside the StringSim
*/
double Graph::similJACCARD_WITH_TFIDF(int vid1, int vid2, string s1, string s2){
	set<string> set1;
	set<string> set2;
	float tot_importancy_set1 = 0;
	float tot_importancy_set2 = 0;
	stringstream ssin1(  s1);
	stringstream ssin2( s2);
	string word;
	float imp;
	while (ssin1.good()){
		ssin1 >> word;
		if (Nw.find(word) != Nw.end())//noise word, don't count
			continue;
		set1.insert(word);
		imp = stringWords[word].Importancy_PerVertex[vid1];
		tot_importancy_set1 += imp;
	}
	while (ssin2.good()){
		ssin2 >> word;
		if (Nw.find(word) != Nw.end())//noise word, don't count
			continue;
		set2.insert(word);
		imp = stringWords[word].Importancy_PerVertex[vid2];
		tot_importancy_set2 += imp;
	}
	std::set<string>::iterator it;
	float nom = 0.0;
	//float denom = set1.size() + set2.size();
	float denom = (vl[vid1].tot_literal_importance_weight + vl[vid2].tot_literal_importance_weight) / 2;

	//cout << vl[vid1].tot_literal_importance_weight << endl;
	//cout << vl[vid2].tot_literal_importance_weight << endl;

	float num_common = 0.0;
	for (it = set1.begin(); it != set1.end(); ++it){
		word = *it;
		if (set2.find(word) != set2.end()){

			//cout << stringWords[word].Importancy_PerVertex[vid1] << endl;
			//cout << stringWords[word].Importancy_PerVertex[vid2] << endl;

			num_common++;
			//common word on both
			if (!conf.autoWeights()){
				nom += ((stringWords[word].Importancy_PerVertex[vid1]) * vl[vid1].tot_literal_importance_weight +
					(stringWords[word].Importancy_PerVertex[vid2]) * vl[vid2].tot_literal_importance_weight) / 2.0;
			}
		}
	}
	
	if (!conf.autoWeights())
	{
		//not using auto wights
		denom = (float)set1.size() + (float)set2.size() - num_common;
		nom = num_common;
	}
	if (denom == 0)
		return 0.0;
	return (nom / denom);
	
	
		//used to be this
	//return nom; // I think the previous word importancy calculation already included some kind of jaccard
}


/*
	Given two vertex ids get the list of their common outgoing labels(properties)
*/
vector<int> Graph::getOLbIntersect(int i, int j)
{
	vector<int> result ;
	map<int,EdgeOut>::iterator f1 = vl[i].edge_out_map.begin();
	map<int,EdgeOut>::iterator f2 = vl[j].edge_out_map.begin();
	map<int,EdgeOut>::iterator l1 = vl[i].edge_out_map.end();
	map<int,EdgeOut>::iterator l2 = vl[j].edge_out_map.end();
	while (f1!=l1 && f2!=l2)
	{
		if (f1->first<f2->first) ++f1;
		else if (f2->first<f1->first) ++f2;
		else {
			//*result++ = *f1++;
			result.push_back(f1->first);
			f2++;
		}
	}
	return result;
}
/*
	Given two vertex ids get the list of their merged outgoing labels(properties)
*/
vector<int> Graph::getOLbUnion(int i, int j)
{
	vector<int> result;
	map<int,EdgeOut>::iterator f1 = vl[i].edge_out_map.begin();
	map<int,EdgeOut>::iterator f2 = vl[j].edge_out_map.begin();
	map<int,EdgeOut>::iterator l1 = vl[i].edge_out_map.end();
	map<int,EdgeOut>::iterator l2 = vl[j].edge_out_map.end();
	while (f1!=l1 && f2!=l2)
	{
		if (f1->first < f2->first) result.push_back( f1++->first );
		else if (f2->first<f1->first) result.push_back( f2++->first);
		else {
			result.push_back(f1++->first);
			f2++;
		}
	}
	while(f1 != l1)
		result.push_back(f1++->first);
	while(f2 != l2)
		result.push_back(f2++->first);
	return result;
}
/*
	Given two vertex ids get the list of their common incoming labels(properties)
*/
vector<int> Graph::getILbIntersect(int i, int j)
{
	vector<int> result ;
	map<int,EdgeIn>::iterator f1 = vl[i].edge_in_map.begin();
	map<int,EdgeIn>::iterator f2 = vl[j].edge_in_map.begin();
	map<int,EdgeIn>::iterator l1 = vl[i].edge_in_map.end();
	map<int,EdgeIn>::iterator l2 = vl[j].edge_in_map.end();
	while (f1!=l1 && f2!=l2)
	{
		if (f1->first<f2->first) ++f1;
		else if (f2->first<f1->first) ++f2;
		else {
			result.push_back(f1++->first);
			f2++;
		}
	}
	return result;
}
/*
	Given two vertex ids get the list of their merged incoming labels(properties)
*/
vector<int> Graph::getILbUnion(int i, int j)
{
	vector<int> result;
	map<int,EdgeIn>::iterator f1 = vl[i].edge_in_map.begin();
	map<int,EdgeIn>::iterator f2 = vl[j].edge_in_map.begin();
	map<int,EdgeIn>::iterator l1 = vl[i].edge_in_map.end();
	map<int,EdgeIn>::iterator l2 = vl[j].edge_in_map.end();
	while (f1!=l1 && f2!=l2)
	{
		if (f1->first < f2->first) result.push_back( f1++->first );
		else if (f2->first<f1->first) result.push_back( f2++->first);
		else {
			result.push_back(f1++->first);
			f2++;
		}
	}
	while(f1 != l1)
		result.push_back(f1++->first);
	while(f2 != l2)
		result.push_back(f2++->first);
	return result;
}

/*
	Write the generated clusters to a defined output(probably to a file)
*/
void Graph::writeClusters(ostream& out, float clustering_success_rate)
{
	numEdgesInSummaryGraph = 0;
	numGoodEdgesInSummaryGraph = 0;
	Cluster *c;
	Cluster *c_dest;
	int index = 1;
	int vid;
	string vname;
	cout<<"Writing final clusters ..."<<endl;
	//out << "YOU ALREADY GENERATED CLUSTER RELATIONS STORED IN THE CLUSTER STRUCT EDGE MAPS. WRITE AND VISUALIZE THOSE TOO"<< endl;
	if (clustering_success_rate > 0){
		cout << "Clustering success rate: " << clustering_success_rate << "%" << endl << endl;;
		out << "Clustering success rate: " << clustering_success_rate << "%" << endl << endl;;
	}

	out<< "CLUSTER LIST:"<<endl;
	for(int i =0;i< setClusters.size();i++)
	{
		c = &setClusters.at(i);
		if(c->members.size() == 0)
			continue;
		out<<endl<< c->name << endl; //it used to be << index
		//cout<< endl<< "CLUSTER: " << i << endl;
		index++;
		for(int j =0;j<c->members.size();j++)
		{
			vid = c->members.at(j);
			vname = vrmap[vid];
			out << "\t"<< vname << " ("<< vid <<")" <<" ("<<vl[vid].edge_out_map.size() << ")" <<  endl;
			//	cout << vname << " ("<< vid <<")" << endl;
			if (j % 10000 == 0){//progress bar
				//cout << ".";
			}
		}
	}
	//cout<<endl<<"Writing un-clustered nodes ..."<<endl;
	out<<endl<< "Printing Un-clusterred Nodes:" << endl;
	for (int i =0;i< vl.size();i++)
	{
		vid = i;
		if (vid == 14)
		{
			string x = "debug me here";
		}
		if(!vToClusterMap.count(vid))
		{
			vname = vrmap[vid];
			//out << vname << " ("<< vid <<")" << endl;
			out << vname << " ("<< vid <<")" <<" ("<<vl[vid].edge_out_map.size() << ")" <<  endl;
			//	cout << vname << " ("<< vid <<")" << endl;
		}
		if (i % 10000 == 0){//progress bar
			//cout << ".";
		}
	}
	out<< "" <<endl;
	cout<<endl;
	out<< "CLUSTER RELATIONS:"<<endl;
	out << "" << endl;
	out << "------------" << endl;
	cout << "Summary Graph Stability: " << summaryGraphStability << " %" << endl;
	out << "Summary Graph Stability: " << summaryGraphStability << " %" << endl;

	cout << "summaryGraph_cRMSD: " << summaryGraph_cRMSD << endl;
	out << "summaryGraph_cRMSD: " << summaryGraph_cRMSD <<  endl;

	out << "------------" << endl;
	cout << endl;
	for(int i =0;i< setClusters.size();i++)
	{		
		//i = current cluster index
		c = &setClusters.at(i); // current cluster

		if (c->members.size() == 0)
			continue;

		//cout << "" << endl;
		out << "" << endl;

		//cout<< c->name <<endl;
		out<< c->name <<endl;

		//cout<<"\tClass Stability Ratio:"<<c->stability_ratio<<" %"<<endl;
		out<<"\tClass Stability Ratio:"<<c->stability_ratio<<" %"<<endl;

		//cout<<"\tClass Property Coverage Ratio:"<<c->propery_coverage_ratio<<" %"<<endl;
		out<<"\tClass Property Coverage Ratio:"<<c->propery_coverage_ratio<<" %"<<endl;

		EdgeOutMap &c_edge_out_map = c->edge_out_map;
		for ( auto it = c_edge_out_map.begin(); it != c_edge_out_map.end(); ++it )
		{
			int c_label_id = it->first;// cluster relation (cluster triple) label id
			string c_label_name = lrmap[c_label_id];
			EdgeOut &c_edge_out = it->second;//cluster relation edge outgoing struct
			for( int k = 0; k < c_edge_out.dest.size(); k++ )
			{

				int dest_cluster_id = c_edge_out.dest[k] ; //neighbor cluster id
				c_dest = &setClusters.at(dest_cluster_id);
				
				if (c->size > 1){
					numEdgesInSummaryGraph++;
				}

				if (c->size > 1 && c_dest->size > 1){
					numGoodEdgesInSummaryGraph++;
				}
				//Writing relation
				//cout << "\t" << c->name << "\t" << c_label_name << "\t" << c_dest->name << "  (stability: " << c->propertyStabilityByRefCluster[c_label_id][dest_cluster_id]*100 << " % )" <<  endl;
				out << "\t" << c->name << "\t" << c_label_name << "\t" << c_dest->name << "  (stability: " << c->propertyStabilityByRefCluster[c_label_id][dest_cluster_id]*100 << " % )" << endl;

				//out<<"Cluster:"<<i<<"\t"<< c_label_id <<"\t"<<"Cluster:"<<dest_cluster_id<<endl;
			}
		}
	}
	cout <<endl<< "Done writing the clusters." <<endl;
}


/*
Write the generated clusters to a defined output(probably to a file)
*/
void Graph::writeClustersJSON(ostream& out, float clustering_success_rate)
{
	Cluster *c;
	Cluster *c_dest;
	int index = 1;
	int vid;
	string vname;
	cout << "Writing final clusters ..." << endl;
	//out << "YOU ALREADY GENERATED CLUSTER RELATIONS STORED IN THE CLUSTER STRUCT EDGE MAPS. WRITE AND VISUALIZE THOSE TOO"<< endl;
	if (clustering_success_rate > 0){
		cout << "Clustering success rate: " << clustering_success_rate << "%" << endl << endl;;
		out << "Clustering success rate: " << clustering_success_rate << "%" << endl << endl;;
	}

	out << "{\"name\":\"root\", \"children\" : [" << endl; //it used to be << index

	/*{"name":"root", "children" : [
	{"name":"C-Harvard_University>","children": [
	{"name":"Harvard_University","size":2600},
	{"name":"Yale_University","size":2600}
	]
	},
	*/

	for (int i = 0; i< setClusters.size(); i++)
	{
		c = &setClusters.at(i);
		if (c->members.size() == 0)
			continue;

		out << "{\"name\":\"" + c->name + "\"," + "\"children\"" + ": [" << endl; //it used to be << index

		index++;
		for (int j = 0; j<c->members.size(); j++)
		{
			vid = c->members.at(j);
			vname = vrmap[vid];

			vname.erase(std::remove(vname.begin(), vname.end(), '"'), vname.end());
			vname.erase(std::remove(vname.begin(), vname.end(), '>'), vname.end());

			//if (vname.find("http://dbpedia.org/resource/")>0)
			//	vname.erase(0, 29);
			//vname.replace(vname.find("http://dbpedia.org/resource/")-1,29, ""); 


			//out << "\t" << vname << " (" << vid << ")" << " (" << vl[vid].edge_out_map.size() << ")" << endl;
			out << "{\"name\":\"" + vname << "\"," << "\"size\"" << ":" << vl[vid].edge_out_map.size() * 100 << "}";

			if (j == c->members.size() - 1)
				out << endl;
			else
				out << "," << endl;

			//{ "name": "<http://dbpedia.org/resource/Barbara_Snyder> ", "size" : 7840 }

			//	cout << vname << " ("<< vid <<")" << endl;
			if (j % 10000 == 0){//progress bar
				//cout << ".";
			}
		}
		if (i == setClusters.size() - 1)
			out << "]" << endl << "}" << endl;
		else
			out << "]" << endl << "}," << endl;
	}
	out << "]" << endl << "}" << endl;
 
	cout << endl << "Done writing the clusters." << endl;
}


/*
given a ClusterVerificationFileName(contains real cluster results), calculate and print how correct your clusters are
*/
float Graph::verifyClusterResults(string ClusterVerificationFileName, float cluster_member_similarity_threshold)
{
	//ERROR HERE, vname has not been populated yet. Populate it in the readReverseMapping function. But do not populate it if format = triple
	/*cout << "The verification of the clusters planned to be implemented after we do the hieracial clustering." << endl;
	return;*/
	ifstream in(ClusterVerificationFileName);
	if (!in) {
		cerr << "Error: Cannot open the cluster verification file: " << ClusterVerificationFileName << endl;
		return -1 ;
	}
	cout << "Reading the cluster verification file..." << endl;
	string buf;
	vector<int> members;
	map<int, int> verifiedClusterMap;
	int k1, k2, vid, vid1, vid2, i, j;
	string pair_key;
	int pairOIndex;
	int tot_pairs = 0;
	int tot_pairs_correct = 0;
	float success_rate1 = 0.0;
	float success_rate2 = 0.0;
	float success_rate = 0.0;
	string vname = "";
	int num_verif_clusters = 0;

	//1. compare verifCluster with GeneratedClusers =  success_rate1
	while (getline(in, buf)) {
		if (buf.substr(0, 2) != "\t\t"){
			num_verif_clusters++;
			//cout << "new cluster: " << buf << endl;
			if (members.size() > 0){
				for (k1 = 0; k1 < members.size(); k1++){
					for (k2 = (members.size() > 1 ? k1 + 1 : k1); k2 < members.size(); k2++){ //the for loop also account for the clusters which have only one member
						tot_pairs++;
						i = members[k1];
						j = members[k2];
						if (i == 14 || j == 14)
						{
							string xx = "debug here";
						}
						if (vToClusterMap.count(i) && vToClusterMap.count(j) && vToClusterMap[i] == vToClusterMap[j]){
							tot_pairs_correct++;							
						}
						else{
							//cout << vrmap[i] << "\t" << vrmap[j] << "\tsuppose to be in the same cluster!" << endl;
						}
					}
				}
			}
			members.clear();
			//starts of a new cluster
		}
		else
		{
			vname = Util::removeChar( Util::removeChar(buf, '\t'), '\t');
			//cout << "cluster member" << vname << endl;
			vid = vmap[vname];
			members.push_back(vid);
			verifiedClusterMap[vid] = num_verif_clusters;
		}
	}
	in.close();
	success_rate1 = ((float)tot_pairs_correct / (float)tot_pairs) * 100;

	tot_pairs_correct = 0;
	tot_pairs = 0;
	//2. compare GeneratedClusers with verifCluster =  success_rate2
	Cluster *c;
	int i1, j1, j2;
	for (i1 = 0; i1 < setClusters.size(); i1++)
	{
		//i = current cluster index
		c = &setClusters.at(i1); // current cluster
		for (j1 = 0; j1 < c->size; j1++) // process current cluster's members
		{
			for (j2 = j1+1; j2 < c->size; j2++){
				tot_pairs++;
				i = c->members.at(j1); // member vertex id = i
				j = c->members.at(j2); // member vertex id = j
				if (verifiedClusterMap.count(i) && verifiedClusterMap.count(j) && verifiedClusterMap[i] == verifiedClusterMap[j]){
					tot_pairs_correct++;
				}
				else{
					//cout << vrmap[i] << "\t" << vrmap[j] << "\tare NOT suppose to be in the same cluster!" << endl;
				}
			}
			
		}
	}
	success_rate2 = ((float)tot_pairs_correct / (float)tot_pairs) * 100;

	success_rate = (success_rate1 + success_rate2) / 2;
	return success_rate;
}

/*
	After generating the clusters also generate the cluster relations. 
	Similar to creating the RDFS schema
*/
void Graph::generateClusterRelations()
{
	Cluster *c;
	int vid;
	set<int> setForDups;
	map<int, bool> setForSourcePredDestCluster; //if a member in c1 making a p connection to a member in c2 more than one time then only count it once	, destClusterIndex, true/false of reference				
	cout<<"Generating cluster relations ..."<<endl;
	int allPredicateRefCount = 0;
	float allPredicateRefStabilitySum = 0;
	numGoodClusteredNodes = 0;
	numGoodClusters = 0;
	for(int i =0;i< setClusters.size();i++)
	{
		//i = current cluster index
		c = &setClusters.at(i); // current cluster
		c->size = c->members.size(); // update current cluster size
		if (c->size > 1){
			numGoodClusters += 1;
			numGoodClusteredNodes += c->size;
		}
		c->degree = 0;
		c->in_degree = 0;
		c->stability_ratio = 0;
		c->propery_coverage_ratio = 0;
		float c_members_total_out_degree = 0;
		if(c->size == 0)
			continue;
		//cout<< endl<< "Processing cluster: " << c->name << endl;
		for(int j =0;j<c->size;j++) // process current cluster's members
		{
			vid = c->members.at(j); // member vertex id
			c_members_total_out_degree+= vl[vid].degree;
			EdgeOutMap &v_edge_out_map = vl[vid].edge_out_map;
			for ( auto it = v_edge_out_map.begin(); it != v_edge_out_map.end(); ++it )
			{
				int v_label_id = it->first;// vertex triple label id
				EdgeOut &v_edge_out = it->second;//vertex triple edge outgoing
				//it->first; //label_id
				//it->second;//EdgeOut struct
				setForSourcePredDestCluster.clear();
				for( int k = 0; k < it->second.dest.size() ; k++ )
				{
					int dest_vertex_id = it->second.dest[k] ; //neighbor vertex id					
					bool dest_Cluster_exists = vToClusterMap.count(dest_vertex_id);
					
					if(dest_Cluster_exists)
					{
						int dest_Cluster_Index = vToClusterMap[dest_vertex_id];
						Cluster &c_dest = setClusters.at(dest_Cluster_Index);//destination cluster
						//adding cluster relation like triples = ( current_cluster_index, connecting_label_id, destination_cluster_index )
						
						//adding out Edge. add the dest cluster to the current cluster outgoings
						if(!c->edge_out_map.count(v_label_id))
						{
							EdgeOut edgeOut  = EdgeOut();
							edgeOut.src = i;//current cluster index
							edgeOut.label_id = v_label_id;
							edgeOut.dest = VectIntType();
							c->edge_out_map[v_label_id] = edgeOut;
						}
						//you need to check for uniqueness[ uniqueness checking is at the end by using sets ]
						//if (std::find(c->edge_out_map[v_label_id].dest.begin(), c->edge_out_map[v_label_id].dest.end(), dest_Cluster_Index) == c->edge_out_map[v_label_id].dest.end()){
							c->edge_out_map[v_label_id].dest.push_back(dest_Cluster_Index);//add the neighboring cluster index
							c->degree++;
						//}

						//adding in Edge. add src to dest's incoming
						if(!c_dest.edge_in_map.count(v_label_id))
						{
							EdgeIn edgeIn  = EdgeIn();
							edgeIn.src = VectIntType();
							edgeIn.label_id = v_label_id;
							edgeIn.dest = dest_Cluster_Index; //destination cluster index
							c_dest.edge_in_map[v_label_id] = edgeIn;
						}
						//you need to check for uniqueness[ uniqueness checking is at the end by using sets ]
						//if (std::find(c_dest.edge_in_map[v_label_id].src.begin(), c_dest.edge_in_map[v_label_id].src.end(), i) == c_dest.edge_in_map[v_label_id].src.end()){
							c_dest.edge_in_map[v_label_id].src.push_back(i);//current cluster index
							c_dest.in_degree++;
						//}
						
						if (!setForSourcePredDestCluster.count(dest_Cluster_Index)){
							setForSourcePredDestCluster[dest_Cluster_Index] = true;

							if (!c->propertyStabilityByRefCluster.count(v_label_id)){
								map<int, float> refClusterStability;
								c->propertyStabilityByRefCluster[v_label_id] = refClusterStability;
							}
							if (!c->propertyStabilityByRefCluster[v_label_id].count(dest_Cluster_Index)){
								c->propertyStabilityByRefCluster[v_label_id][dest_Cluster_Index] = 0;
							}
							c->propertyStabilityByRefCluster[v_label_id][dest_Cluster_Index] += 1 / (float)c->size; //will add to the stability ratio for c1, p, c2
						}						
					}
				}
			}

			//cout << vname << " ("<< vid <<")" <<" ("<<vl[vid].edge_out_map.size() << ")" <<  endl;
			if (j % 10000 == 0){//progress bar
				//cout << ".";
			}
		}
		//Remove dups from the relations, ( triples may contain duplicates )
		for ( auto it = c->edge_out_map.begin(); it != c->edge_out_map.end(); ++it )
		{
			setForDups.clear();
			for( int k = 0; k < it->second.dest.size(); k++ )
				setForDups.insert( it->second.dest[k]);
			it->second.dest.assign( setForDups.begin(), setForDups.end() );
		}
		for ( auto it = c->edge_in_map.begin(); it != c->edge_in_map.end(); ++it )
		{
			setForDups.clear();
			for( int k = 0; k < it->second.src.size(); k++ )
				setForDups.insert( it->second.src[k]);
			it->second.src.assign( setForDups.begin(), setForDups.end() );
		}
		setForDups.clear();
		//calculate stability ratio
		float predicateStabilitiesSum = 0;
		int predicateRefsCount = 0;
		for (auto it = c->propertyStabilityByRefCluster.begin(); it != c->propertyStabilityByRefCluster.end(); ++it){
			int v_label_id = it->first; //property id, p
			for (auto it2 = c->propertyStabilityByRefCluster[v_label_id].begin(); it2 != c->propertyStabilityByRefCluster[v_label_id].end(); ++it2){
				int dest_Cluster_Index = it2->first; //destination cluster, c2
				predicateStabilitiesSum += it2->second;
				predicateRefsCount++;
			}
		}
		allPredicateRefCount += predicateRefsCount;
		allPredicateRefStabilitySum += predicateStabilitiesSum;
		c->stability_ratio = (predicateRefsCount != 0 ? ((float)predicateStabilitiesSum / (float)predicateRefsCount) * 100 : 100);
		c->propery_coverage_ratio = (c_members_total_out_degree != 0 ? ((float)c->degree / (float)c_members_total_out_degree)*100 : 100);
	}

	//calculating summary graph stability
	summaryGraphStability = (allPredicateRefCount != 0 ? ((float)allPredicateRefStabilitySum / (float)allPredicateRefCount) * 100 : 100); //calculated summary graph stability
	cout <<endl<< "Done generating cluster relations." <<endl;
}


void Graph::nameClusters()//name the cluster
{
	Cluster *c;
	int vid, pos1, pos2;
	map<string, int> vnameCounts;
	map<string, vector<int>> cnameIndexes;
	string vname, vname2;	
	cout << "Generating cluster names ..." << endl;
	for (int i = 0; i< setClusters.size(); i++)
	{		
		vnameCounts.clear();
		c = &setClusters.at(i); // current cluster
		std::stringstream sstm;
		sstm << "CLUSTER: " << (i + 1); //default namings
		c->name = sstm.str();
		//cout << "Processing: " << c->name << endl;
		if (c->size == 0)
			continue;
		//cout << endl << "Processing CLUSTER: " << i << endl;
		for (int j = 0; j<c->members.size(); j++) // process current cluster's members
		{
			vid = c->members.at(j); // member vertex id
			vname = vrmap[vid];

			pos1 = vname.rfind("/");
			if (pos1 < 0)
				continue;
			vname2 = vname.substr(pos1+1);
			pos2 = vname2.rfind(":");
			if (pos2 > 0)
				vname2 = vname2.substr(0, pos2);

			if (!vnameCounts.count(vname2))
				vnameCounts[vname2] = 0;
			vnameCounts[vname2]++;

			//cout << vname << " ("<< vid <<")" <<" ("<<vl[vid].edge_out_map.size() << ")" <<  endl;
			if (j % 10000 == 0){//progress bar
				//cout << ".";
			}
		}
		int count=0;
		for (auto it = vnameCounts.begin(); it != vnameCounts.end(); ++it)
		{
			if (it->second > count){
				count = it->second;
				c->name = "C-" + it->first;
			}
		}
		
		if (!cnameIndexes.count(c->name))
			cnameIndexes[c->name] = vector<int>();
		cnameIndexes[c->name].push_back(i);
		//cout << "Cluster name: " << c->name << endl;
	}	

	for (auto it = cnameIndexes.begin(); it != cnameIndexes.end(); ++it)
	{
		if (it->second.size() > 1){ //more than one cluster shares the same name, so give them numbers
			for (int i = 0; i < it->second.size(); i++)
			{
				c = &setClusters.at(it->second[i]); // current cluster
				c->name = c->name + "-" + Util::itos(i + 1);
			}
		}
	}

	/*for (int i = 0; i < setClusters.size(); i++)
	{
		c = &setClusters.at(i); // current cluster
		if (cnameCounts[c->name] > 1)
			c->name = c->name + "-" + Util::itos(i + 1);
	}*/
	cout << endl << "Done generating cluster namings." << endl;
}

/*
	Generating clusters according to the calculated pair similarities. 
	Final classification
	This method needs imporovement
*/
void Graph::classifyDataSet(float cluster_member_similarity_threshold)
{
	//Here I have to disable the sorting because I need to use the pairOMap[pair_key] later. If I sort then the map changes completely
	//cout<< "\nSorting the common pair list ..."<<endl;
	//sort(pairOList.begin(), pairOList.end());//sorting by pair(i,j) similarity DESC
	//cout<< "Sorted the common pair list."<<endl;
	int i,j,index, i_index, j_index, s_index, b_index;
	Cluster *cs, *cb, *ci, *cj;
	bool ci_exists,cj_exists;
	cout << "Classifying the data set ..."<<endl;
	cout << "Classifying based on pair similarities ..." << endl;
	for ( auto it = pairOList.begin(); it != pairOList.end(); ++it )
	{	
		i = it->i;
		j = it->j;

		//cout << i << " , " << j << endl;
		//cout << "Examining classification for: " << vrmap[i] << "\t" << vrmap[j] << endl;
		if (!it->include_in_classification)
			continue;
		
		/*//If you don't want to cluster the Value nodes then uncomment this
		if (vl[i].valueNode || vl[j].valueNode)
			continue;
		*/

		if( it->DisSimilarity > cluster_member_similarity_threshold)//they are not similar, //New: don't you wanna break here since you already sorted by similarity? = Answer: No because disabled the sorting
			continue;

		if ((i == 14 || j == 14) ){
			string x = "debug here";
		}

		ci_exists = vToClusterMap.count(i);
		cj_exists = vToClusterMap.count(j);
		if(ci_exists && !cj_exists){
			index = vToClusterMap[i];
			cs = &setClusters.at(index);
			cs->members.push_back(j);
			vToClusterMap[j] = index;
		}
		else if(cj_exists && !ci_exists)
		{
			index = vToClusterMap[j];
			cs = &setClusters.at(index);
			cs->members.push_back(i);
			vToClusterMap[i] = index;
		}
		else if(!cj_exists && !ci_exists)
		{
			setClusters.push_back( Cluster() );
			index = setClusters.size()-1;
			setClusters[index].members.push_back(i);
			setClusters[index].members.push_back(j);
			vToClusterMap[i] = index;
			vToClusterMap[j] = index;
		}
		else//both exists then merge
		{
			i_index = vToClusterMap[i];
			j_index = vToClusterMap[j];
			if(i_index == j_index)//if they are already in the same cluster dont do anything
				continue;
			ci = &setClusters.at( i_index );
			cj = &setClusters.at( j_index );
			if(ci->members.size() >= cj->members.size())
			{
				cb = ci;
				b_index = i_index;
				s_index = j_index;
				cs = cj;
			}
			else
			{
				cb = cj;
				b_index = j_index;
				s_index = i_index;
				cs = ci;
			}
			//merge cs into cb. and update the map
			vector<int>::iterator it;
			for(it=cs->members.begin(); it != cs->members.end(); ++it)
			{
				int vid = *it;
				cb->members.push_back( vid );
				vToClusterMap[vid] = b_index;
			}
			//cb.insert(cb.end(), cs.begin(), cs.end());
			//delete &cs;
			cs->members.clear();
			//delete &cs;
		}
	}
	cout<<endl<<"Putting un-clustered nodes into their own cluster ..."<<endl;
	for (int vid = 0; vid< vl.size(); vid++)
	{
		if (vl[vid].valueNode || vToClusterMap.count(vid))
			continue;
		//generate a new cluster for each unclustered node - put them into a new different cluster for each
		setClusters.push_back(Cluster());
		index = setClusters.size() - 1;
		setClusters[index].members.push_back(vid);
		vToClusterMap[vid] = index;
	}
}

/*
	Decide whether the given propery is frequent or not.
	Frequent properties are excluded for Pairs generating to decrease the complexity
*/
bool Graph::IsFrequentLabel(int lid)
{
	set<int> &vset = ll[lid].OV; //lbOV[lid];//vertex id list for the labal lid
	bool frequent = false;
	if(vset.size() >= DEF_max_label_occurence)//don't process frequently occured labels. otherwise you will still have n square complexity
	{
		//cout<<"frequently occured label: "<<lrmap[lid]<< "\tfreq: "<< 100*vset.size()/vl.size() <<endl;
		frequent = true;
	}
	return frequent;
}

/*
	Given two list of vertexes Pair them up using cartesian pairs 
	If checkLabels is specified then pair them only if they share common properties
*/
void Graph::pairLists(const VectIntType &c1, const VectIntType &c2, bool checkLabels,bool initializeSim)
{
	int vid, vid_to_pair;
	//pairing things
	for ( auto it1 = c1.begin(); it1 != c1.end(); ++it1 )
	{
		vid = *it1;
		for ( auto it2 = c2.begin(); it2 != c2.end(); ++it2 )
		{
			vid_to_pair = *it2;
			if(vid == vid_to_pair)
				continue;
			addPair(vid, vid_to_pair, checkLabels,initializeSim, -1, -1);
		}
	}
}

/* 
	Given two vertexes with ids i and j pair them up
	If checkLabels is specified then pair them only if they share common properties
*/
void Graph::addPair(int v1, int v2, bool checkLabels, bool initializeSim, int ref_vidi, int ref_vidj)
{
	//there shouldn't any dups
	int i,j;
	i = std::max(v1,v2);
	j = std::min(v1,v2);


	string pair_key = getPairKey(i, j, ref_vidi, ref_vidj);
	//cout<< "Pair key: " << pair_key<< endl;
	if(pairOMap.find(pair_key) == pairOMap.end()) //not added before
	{		
		PairO pair = PairO();
		pair.i = i; //i > j
		pair.j = j;
		pair.O_lb_intersect = getOLbIntersect(i,j);
		pair.O_lb_union = getOLbUnion(i, j);
		pair.size_O_lb_intersect = pair.O_lb_intersect.size();
		bool goodToAdd = true;
		if(checkLabels)
		{
			if (vl[i].valueNode && vl[j].valueNode)
			{
				pair.both_value_nodes = true;
				pair.include_in_classification = false;
				pair.include_in_sim_iterations = false;
			}
			else{
				int frequentLabelCount = 0;
				int lid;
				for (auto it = pair.O_lb_intersect.begin(); it != pair.O_lb_intersect.end(); ++it)
				{
					lid = *it;
					if (IsFrequentLabel(lid))//exclude frequent labels into common labels check calculation
						frequentLabelCount++;
				}
				if (pair.size_O_lb_intersect - frequentLabelCount == 0)
					goodToAdd = false;
			}
		}
		if(goodToAdd)
		{
			if(pair.size_O_lb_intersect > pairOMaxLenCommonLabels)
				pairOMaxLenCommonLabels = pair.size_O_lb_intersect;
			pair.size_O_lb_union = vl[i].edge_out_map.size() + vl[j].edge_out_map.size() - pair.size_O_lb_intersect;
			if(initializeSim)
			{				
				//Both of them are value nodes
				float pairSim = 0;
				if(pair.both_value_nodes)
				{
					if (conf.literalSims()){
						//using string sims/literal sims
						pair.ref_i = std::max(ref_vidi, ref_vidj);
						pair.ref_j = std::min(ref_vidi, ref_vidj);
						//both of them are value nodes
						if (pair.size_O_lb_intersect > 0)
							throw std::logic_error("For value nodes there shouldn't be any label at all!");
						/*cout<<"vertex i: "<<vl[i].valueNode_label_clean<<endl;
						cout<<"vertex j: "<<vl[j].valueNode_label_clean<<endl;
						cout<<"Here you need another similarity. String sims for value nodes"<<endl;*/

						//You need to check for data type. This is only true for strings!
						//pairSim = stringSim.GetSim(vl[i].valueNode_label_clean, vl[j].valueNode_label_clean, JARO);
						pairSim = 0.25;
						pairSim += ((vl[i].valueNode_data_type == vl[j].valueNode_data_type) ? 0.25 : 0.0); //data type weight
						//if (vl[i].valueNode_label_clean == vl[j].valueNode_label_clean) pairSim += 0.8; 
						//else
						//pairSim+= stringSim.GetSim(vl[i].valueNode_label_clean, vl[j].valueNode_label_clean, LEVEN) * 0.5);
						pairSim += similJACCARD_WITH_TFIDF(ref_vidi, ref_vidj, vl[v1].valueNode_label_clean, vl[v2].valueNode_label_clean) * 0.5;
						//here we are using already calculated tf-idf for each string literal word per each node!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

						//DELETE THIS
						//pairSim = 1; //overwriding
					}
					else
						pairSim = 1; //here overwriding the string similarity / basically ignoring string sim
				}
				else//entity - resource -id nodes
				{
					if(pair.size_O_lb_union > 0)
					{
						pairSim = (float)((float)pair.size_O_lb_intersect / (float)pair.size_O_lb_union);
					}
					//jaccard = 1; , if you want to initialize with Uniform( all 1s ) then uncomment this line, instead of using the previous function
				}
				pair.DisSimilarity = ( 1- pairSim );
			}
			pairOList.push_back(pair);
			pairOMap[ pair_key ] = pairOList.size()-1 ;
		}
		else
		{
			//it means it was examined before but they share no common label hence their similarities is zero
			pairOMap[ pair_key ] = -1 ;
		}
	}
}

/*
	Get the corresponding pair key given two vertex ids
*/
string Graph::getPairKey(int i, int j, int ref_i, int ref_j)
{
	int big_id, small_id, big_ref_id, small_ref_id ;
	big_id = std::max(i, j);
	small_id = std::min(i, j);
	big_ref_id = std::max(ref_i, ref_j);
	small_ref_id = std::min(ref_i, ref_j);
	std::ostringstream o;
	o << big_id << "," << small_id;
	if (vl[big_id].valueNode && vl[small_id].valueNode) //if both the i and j nodes are value nodes then the similarity changes by the ref_i and ref_j
		o << "," << big_ref_id << "," << small_ref_id;
	if (o.str() == "14,3")
	{
		string x = "debug here";
	}
	return o.str();
}

/*
	Given two vertex ids return the corresponding Pair
*/
/*PairO& Graph::getPair(int vid1, int vid2)
{
	//there shouldn't any dups
	int i,j;
	i = std::max(vid1,vid2);
	j = std::min(vid1,vid2);
	string pair_key =  getPairKey(i,j);
	if(pairOMap.find(pair_key) == pairOMap.end()) //not added before
	{
		throw "Pair not found - pair key: " + pair_key;
	}
	int pair_index = pairOMap[pair_key];
	if(pair_index < 0)
		throw "Pair shouldn't be here(pair_index < 0) - pair key: " + pair_key;
	return pairOList[pair_index];
}
*/
/*
	Given two vertex ids return their similarities
	If addIfNotExists is specified then Pair them if they have not been already paired 
*/
float Graph::getPairSim(int vid1, int vid2, bool addIfNotExists, int ref_vidi, int ref_vidj)
{
	string pair_key = getPairKey(vid1, vid2, ref_vidi, ref_vidj);
	if(pairOMap.find(pair_key) == pairOMap.end()) //not added before
	{
		if (addIfNotExists){
			if (vl[vid1].valueNode && vl[vid2].valueNode)
				if (conf.literalSims())
					addPair(vid1, vid2, true, true, ref_vidi, ref_vidj); //we are using literal sims so add the pair
				else
					return 1.0; //we are not using literal sims and they are both value nodes so just return 1.0 for their similarities without adding them
			else
				addPair(vid1, vid2, true, true, ref_vidi, ref_vidj);
		}
		else
			throw std::logic_error("Pair not found - pair key: " + pair_key );
	}
	int pair_index = pairOMap[pair_key];
	if(pair_index < 0)
		return 0;
	else
		return 1- pairOList[pair_index].DisSimilarity;
}

/*
	Generate the list of Vertex pairs that share common properties. The pairs in this list will be input to the OurSim calculations
	We are doing this to recover from n square complexity	
*/
void Graph::generateCommonPairs(float frequent_propery_threshold)
{
	cout << "In generateCommonPairs" <<endl;
	DEF_max_label_occurence = (vl.size())*frequent_propery_threshold;
	cout << "Generating common pair list" <<endl;

	int lid,vmin,vmin_lastprocessed,i,j, k, count = 0, index;
	int max_lov_size = 0;
	VectIntType lids = VectIntType();
	set<int> vids_merged;
	set<int> *vset;
	set<int>::iterator it;
	while(true)
	{
		vmin = INT_MAX;
		for(lid = 0; lid < ll.size(); lid++ )
		{
			vset = &ll[lid].OV2;//vertex id list for the labal lid
			if(vset->size() >= DEF_max_label_occurence)//don't process frequently occured labels. otherwise you will still have n square complexity
			{
				cout<<"frequently occured label: "<<lrmap[lid]<< "\tfreq: "<< 100*vset->size()/vl.size() <<endl;
				continue;
			}
			if(vset->size() == 0) continue;

			if(*vset->begin() < vmin )//comparing first element
			{
				vmin = *vset->begin();
				lids.clear();
			}
			if(*vset->begin() == vmin)
				lids.push_back(lid);

			if(count == 0){
				if(vset->size() > max_lov_size)
					max_lov_size = vset->size();
			}						
		}

		if(lids.size() == 0)//processed everything
			break;
		
		j = vmin;
		
		//now I have vmin (current pointer and the label_ids that have vmin
		for(k = 0;k < lids.size(); k++)
		{
			lid = lids[k];
			//can delete from OV2
			vset = &ll[lid].OV2;
			vset->erase(vset->begin());//erase the first number which is vmin (j )
			vids_merged.insert(vset->begin(), vset->end());//nlogn complexity here?			*/
		}

		if(count%1000 == 0)
			cout <<".";

		//combination, first element (vmin) to every other element
		for(it = vids_merged.begin(); it != vids_merged.end() ; it++)
		{
			i = *it;
			//there shouldn't any dups
			addPair(i,j, false,true, -1, -1);			
		}
		vids_merged.clear();
		lids.clear();
		count++;
	}
	cout << "\n#Common pairs size: "<< pairOList.size() << endl;
}

/*
  read common pairs (that share common properties) from a file and generates PairOList
  this file usually comes from our LinstaMatch project's minhash calculations
  The pairs in this list will be input to the OurSim calculations
*/
void Graph::readCommonPairs(istream& in){
	/*
	format:
	number of pairs
	-common_pairs-
	vid1 #-# vid2 #-# minhash_sim
	and pairs line by line
	*/
	cout << "Reading common pairs file ..." << endl;
	int num_pairs_from_file;
	string sub, sub2;
	int idx; //pos
	int sid1 = 0; //vertex id1
	int sid2 = 0;// vertex id2
	string buf, buf2, token;
	getline(in, buf);
	istringstream(buf) >> num_pairs_from_file;
	//pairOList = PairOList(num_pairs_from_file);
	pairOList.reserve(num_pairs_from_file);
	getline(in, buf); //-common_pairs-
	getline(in, buf); //vid1 #-# vid2 #-# minhash_sim

	int index = -1;
	while (getline(in, buf)) {
		if (++index % 10000 == 0)
			cout << index << " ";
		//buf = sid: lid= did1 did2 did3: lid= did4 did5 did6#
		//buf = sid1 #-# sid2 #-# min_hash_jaccard
		idx = buf.find(" ");
		if (idx < 0)
			continue;
		sub = buf.substr(0, idx);
		istringstream(sub) >> sid1;
		buf.erase(0, idx + 5);
		//buf | sid2 #-# min_hash_jaccard

		idx = buf.find(" ");
		if (idx < 0)
			continue;
		sub = buf.substr(0, idx);
		istringstream(sub) >> sid2;
		buf.erase(0, idx + 5);
		//buf | min_hash_jaccard

		addPair(sid1, sid2, false, true, -1, -1);
	}
	cout << endl;
	cout << "\n#Common pairs size: " << pairOList.size() << endl;
}

/*
	Generate common pairs just for the pairs which are related to a specific list of nodes
	This is done do decrease the complexity. Because you are just processing the vertexes and their neighbors that you are interested
*/
void Graph::generateCommonPairs_ForVertexes(istream& in, float frequent_propery_threshold)
{
	cout << "In generateCommonPairs_ForVertexes" << endl;
	DEF_max_label_occurence = (vl.size())*frequent_propery_threshold;
	cout << "Generating common pair list for a set of vertexes" << endl;
	VectIntType vids_input;

	set<int> vids_to_pair; //vids_input X vids_to_pair to pair them up
	set<int> vids_to_pair_unique; // diff(vids_to_pair - vids_input)
	set<int> lids_processed;//the processed labels
	int vid, lid, vid_to_pair;
	string buf;
	while (getline(in, buf)) {
		if (buf == "")
			continue;
		//buf = 1 ( means kent state univ )
		istringstream(buf) >> vid;
		if (vid <0 || vid >= vrmap.size())
			throw std::logic_error("Target vertex id: " + buf + " could not be found in the vertext name list!");
		vids_input.push_back(vid);
	}

	for (auto it = vids_input.begin(); it != vids_input.end(); it++)
	{
		vid = *it;
		EdgeOutMap &edge_out_map = vl[vid].edge_out_map;
		for (auto it2 = edge_out_map.begin(); it2 != edge_out_map.end(); ++it2)
		{
			lid = it2->first;
			if (lids_processed.find(lid) == lids_processed.end()) //not in the set
			{
				set<int> &vset = ll[lid].OV;//vertex id list for the labal lid
				if (!IsFrequentLabel(lid))//don't process frequently occured labels. otherwise you will still have n square complexity
				{
					for (auto it3 = vset.begin(); it3 != vset.end(); ++it3)
					{
						vid_to_pair = *it3;
						vids_to_pair.insert(vid_to_pair);
					}
				}
				lids_processed.insert(lid);
			}
		}
	}
	//get the sets difference vids_to_pair_unique = vids_to_pair - vids_input  = O(n)
	std::set_difference(vids_to_pair.begin(), vids_to_pair.end(), vids_input.begin(), vids_input.end(),
		std::inserter(vids_to_pair_unique, vids_to_pair_unique.end()));

	VectIntType vectorlist2(vids_to_pair_unique.begin(), vids_to_pair_unique.end());
	pairLists(vids_input, vectorlist2, false, true);

}

/*
	Read the graph according to the input format(flat, triple )
*/
void Graph::readGraph(istream& in, string format) {	
	if(format == "flat")
	{
		readGraphFlat(in);
		//calculateLabelsSimWeight();//to be used later for property weights
	}
	else if(format == "triple"){
		readGraphTriple(in);
	}
	else if (format == "csv"){
		readGraphCSV(in);
	}
	else
		throw std::logic_error("Un-known Graph format: " + format);
}

/*
	Reads the graph from nt (triples) format and generates flat version	
	It also sets vmap, lmap, tmap
*/
void Graph::readGraphTriple(istream& in)
{
	cout<<"Pre-processing triple file" <<endl;	
	int vindex = 0;//current pointer
	int lindex = 0;
	int triple_count = 0;
	int sid,pid,oid;
	vmap = VertexNameToIntMapping();
	lmap = LabelNameToIntMapping();

	string buf, s, p, o, o_label_part, o_datatype_part;//str
	int idx;
	int line_number = 0;
	int max_triple_size = Util::string_to_int( conf.getOp("max_triple_size") );
	cout<<"line: "<<endl;
	while (getline(in, buf)) {
		line_number++;
		if (max_triple_size >0 && line_number == max_triple_size+1)
			break;
		if(line_number%10000 == 0)
			cout<<"\t" <<line_number;
				
		if(buf == "" || buf[0] == '#')
			continue;
		//buf = <s> <p> <o> . or buf = <s> <p> "o"@en .
		idx = buf.find(" ");
		s = buf.substr(0, idx);
		//s = <s>
		if(!vmap.count(s)){//vertex not mapped yet
			sid = vindex++;
			vmap[s] = sid;
			tmap[sid] = EdgeOutMap();
		}
		else
			sid = vmap[s];//vertex was mapped before
		buf.erase(0, idx+1);
		//buf = <p> <o> . or <p> "o"@en .

		idx = buf.find(" ");
		p = buf.substr(0, idx);

		if(Nlb.find(p) != Nlb.end()) // it is a noise label(property) don't go any further
			continue;

		//p = <p>
		if(!lmap.count(p)){
			pid = lindex++;
			lmap[p] = pid;
		}
		else
			pid = lmap[p];
		buf.erase(0, idx+1);
		//buf = <o> . or "o"@en .
		
		//idx = buf.find(" .");//end of n triple
		idx = buf.rfind(" .");//end of n triple
		if (idx == -1)
			idx = buf.rfind(".");//end of n triple//accounting if the space does not exists
		buf = buf.substr(0, idx);
		o = buf.substr(0, idx);
		
		//add attribute nodes too. grab the label and data part types when you need to process it
		if(!vmap.count(o))
		{
			oid = vindex++;
			vmap[o] = oid;
		}
		else
			oid = vmap[o];

		//adding the outgoing edge to the EdgeOutMap
		if(!tmap[sid].count(pid))
		{
			EdgeOut edgeOut  = EdgeOut();
			edgeOut.src = sid;
			edgeOut.label_id = pid;
			edgeOut.dest = VectIntType();
			edgeOut.dest.push_back( oid );
			triple_count++;
			tmap[sid][pid] = edgeOut;
		}
		else
		{
			VectIntType &dest = tmap[sid][pid].dest;
			if(oid == 0) //might be added before
			{
				//there could be dups
				if(std::find(dest.begin(), dest.end(), 0) == dest.end())//not contains
				{
					dest.push_back( oid );
					triple_count++;
				}
			}
			else//considering there is no dups if the object is not attribute. This is an important assumption. It could not always be the case for different data sets ( mehmet )
			{
				dest.push_back( oid );
				triple_count++;
			}
		}
	}
	cout <<""<<endl;
	Triple_Count_From_Triple_File = triple_count;
}

void Graph::readGraphCSV(istream& in)
{
	cout << "Pre-processing triple file" << endl;
	int vindex = 0;//current pointer
	int lindex = 0;
	int triple_count = 0;
	int sid, pid, oid;
	vmap = VertexNameToIntMapping();
	lmap = LabelNameToIntMapping();

	string buf, s, p, o, o_label_part, o_datatype_part;//str
	int idx;
	int line_number = 0;
	cout << "line: " << endl;
	while (getline(in, buf)) {
		line_number++;
		if (line_number % 10000 == 0)
			cout << "\t" << line_number;

		if (buf == "" || buf[0] == '#')
			continue;
		//buf = <s> <p> <o> . or buf = <s> <p> "o"@en .
		idx = buf.find("\t");
		s = buf.substr(0, idx);
		//s = <s>
		if (!vmap.count(s)){//vertex not mapped yet
			sid = vindex++;
			vmap[s] = sid;
			tmap[sid] = EdgeOutMap();
		}
		else
			sid = vmap[s];//vertex was mapped before
		buf.erase(0, idx + 1);
		//buf = <p> <o> . or <p> "o"@en .

		idx = buf.find("\t");
		p = buf.substr(0, idx);

		if (Nlb.find(p) != Nlb.end()) // it is a noise label(property) don't go any further
			continue;

		//p = <p>
		if (!lmap.count(p)){
			pid = lindex++;
			lmap[p] = pid;
		}
		else
			pid = lmap[p];
		buf.erase(0, idx + 1);
		//buf = <o> . or "o"@en .

		//idx = buf.find(" .");//end of n triple
		idx = buf.rfind("\n");//end of n triple
		buf = buf.substr(0, idx);
		o = buf.substr(0, idx);

		//add attribute nodes too. grab the label and data part types when you need to process it
		if (!vmap.count(o))
		{
			oid = vindex++;
			vmap[o] = oid;
		}
		else
			oid = vmap[o];

		//adding the outgoing edge to the EdgeOutMap
		if (!tmap[sid].count(pid))
		{
			EdgeOut edgeOut = EdgeOut();
			edgeOut.src = sid;
			edgeOut.label_id = pid;
			edgeOut.dest = VectIntType();
			edgeOut.dest.push_back(oid);
			triple_count++;
			tmap[sid][pid] = edgeOut;
		}
		else
		{
			VectIntType &dest = tmap[sid][pid].dest;
			if (oid == 0) //might be added before
			{
				//there could be dups
				if (std::find(dest.begin(), dest.end(), 0) == dest.end())//not contains
				{
					dest.push_back(oid);
					triple_count++;
				}
			}
			else//considering there is no dups if the object is not attribute. This is an important assumption. It could not always be the case for different data sets ( mehmet )
			{
				dest.push_back(oid);
				triple_count++;
			}
		}
	}
	cout << "" << endl;
	Triple_Count_From_Triple_File = triple_count;
}


/*
	Write Vertex Id-Name, Label Id-Name mappings to file(s)
*/
void Graph::writeMappins()
{
	string _max_triple_size = conf.getOp("max_triple_size");
	int max_triple_size = Util::string_to_int(_max_triple_size);
	
	string flat_file =  GraphFileName+"_flat.txt";
	string map_file = GraphFileName+"_map.txt";
	if (max_triple_size > 0){
		flat_file = GraphFileName + "_" + _max_triple_size + "_flat.txt";
		map_file = GraphFileName + "_" + _max_triple_size + "_map.txt";
	}
	int vmap_size = vmap.size();
	int lmap_size = lmap.size();
	ofstream out;
	out.open (flat_file);
	//writing the graph in a flat graph format ( pre-processsing )
	out << "graph_for_greach" << endl;
	out << vmap_size << endl;
	out << lmap_size << endl;
	out << Triple_Count_From_Triple_File << endl;
	for( auto itv = vmap.begin(); itv != vmap.end(); ++itv )
	{
		out << itv->second ; //vertex id
		EdgeOutMap &edge_out_map = tmap[itv->second];
		for ( auto it = edge_out_map.begin(); it != edge_out_map.end(); ++it )
		{
			out << ": " ;
			//it->first; //label_id
			//it->second;//EdgeOut struct
			out << it->first << "=" ; //label id
			for( int j = 0; j < it->second.dest.size() ; j++ )
			{
				out<<" ";
				out << it->second.dest[j] ; //neighbor vertex id
			}
		}
		out << "#" << endl;
	}
	out.close();

	ofstream out_map;
	out_map.open (map_file);
	//writing the mapping into a file
	string sep = " $|$ ";
	out_map<< "-vertex_mapping-" << endl;
	out_map << vmap_size << endl;
	for ( auto it = vmap.begin(); it != vmap.end(); ++it )
	{
		out_map << it->second << sep << it->first << endl;  //vertex_id sep vertex_name
	}
	out_map<< "-label_mapping-" << endl;
	out_map << lmap_size << endl;
	for ( auto it = lmap.begin(); it != lmap.end(); ++it )
	{
		out_map << it->second << sep << it->first << endl;  //label_id sep label_name
	}
	out_map.close();
	cout << "Triple Graph Format Pre-processed" <<endl;
	cout << "Pre-processed flatted file name: "<< flat_file <<endl;
	cout << "Pre-processed mapping file name: "<< map_file <<endl;
}

/*
	sets the vrmap and lrmap , [id] = name
	vrmap and lrmap are used to retrieve the Name of vertex or Label given their id
*/
void Graph::readReverseMapping(string MapGraphFileName)
{
	ifstream in(MapGraphFileName);
	if (!in) {
		cerr << "Error: Cannot open " << MapGraphFileName << endl;
		return;
	}
	cout << "Reverse Mapping and writing the clusters..." <<endl;
	int vsize,lsize,idx,vid,lid;
	string buf,s1;//str
	/*
	-vertex_mapping-
	29
	9 $|$ <http://dbpedia.org/resource/Barbara_Snyder>
	-label_mapping-
	26
	0 $|$ <http://dbpedia.org/property/alumni>
	*/
	cout << 1 << endl;
	getline(in, buf);
        string buf2 = "-vertex_mapping-";
        cout<< buf2.length()<< endl;
        cout <<"0"<<endl;
        cout<< buf.length()<< endl;
	if (buf != "-vertex_mapping-"){
		//cout << "Invalid reverse map format!" << endl;
		throw std::logic_error("Invalid reverse map format!");
	}
	cout << 2 << endl;
		
	getline(in, buf);
	istringstream(buf) >> vsize;
	vrmap = VertexIntToNameMapping(vsize);
	vmap = VertexNameToIntMapping();
	lmap = LabelNameToIntMapping();
	int line_number = 2;
	cout<<"line: "<<endl;
	string sep =" $|$ ";
	while (getline(in, buf) && buf != "-label_mapping-") {
		line_number++;
		if(line_number%10000 == 0)
			cout<<"\t" <<line_number;
		if(buf == "")
			continue;
		//buf = 9 $|$ <http://dbpedia.org/resource/Barbara_Snyder>
		idx = buf.find(sep);
		s1 = buf.substr(0, idx);
		//s = 9
		istringstream(s1) >> vid;
		buf.erase(0, idx+sep.length());
		//buf = <http://dbpedia.org/resource/Barbara_Snyder>
		
		vrmap[vid] = buf;
		if (!vmap.count(buf)){//vertex not mapped yet
			vmap[buf] = vid;
			/*if (buf[0] == '<')
				cout << buf << endl;*/
		}	
	}
	
	//no need to read label mapping
	getline(in, buf);
	istringstream(buf) >> lsize;
	lrmap = LabelIntToNameMapping(lsize);
	while (getline(in, buf)) {
		line_number++;
		if(line_number%10000 == 0)
			cout<<"\t" <<line_number;
		if(buf == "")
			continue;
		//buf = 0 $|$ <http://dbpedia.org/property/alumni>
		idx = buf.find(sep);
		s1 = buf.substr(0, idx);
		//s = 0
		istringstream(s1) >> lid;
		buf.erase(0, idx+sep.length());
		//buf = <http://dbpedia.org/property/alumni>
		
		lrmap[lid] = buf;
		if (!lmap.count(buf)){
			lmap[buf] = lid;
		}
	}
	cout <<""<<endl;
}
void Graph::readNoiseLabels(string NoiseLabelsFileName)
{
	ifstream in(NoiseLabelsFileName);
	if (!in) {
		cerr << "Error: Cannot open noise labels file" << NoiseLabelsFileName << endl;
		return;
	}
	cout<<"Reading noise labels...\n";
	string buf;
	while (getline(in, buf)) {
		if(buf == "")
			continue;
		Nlb.insert(buf);
	}
}
void Graph::readNoiseWords(string NoiseWordsFileName)
{
	ifstream in(NoiseWordsFileName);
	if (!in) {
		cerr << "Error: Cannot open noise words file" << NoiseWordsFileName << endl;
		return;
	}
	cout << "Reading noise words...\n";
	string buf;
	while (getline(in, buf)) {
		if (buf == "")
			continue;
		Nw.insert( Util::toLower( buf ));
	}
}

/*
	Reads the graph givent a flat file input
	Populates graph data such as vl(vertexes in the graph) with edge_in and edge out maps, ll(properties in the graph)
*/
void Graph::readGraphFlat(istream& in)
{
	/*
	format:
	graph_for_greach
	vsize
	tsize
	v_id: <l_id= vn1 vn2 ..>#
	*/
	//flattened format
	string buf,buf2, token;
	getline(in, buf);
	istringstream ss(buf);
	ss >> token;

	if (token != "graph_for_greach") {
		cout << "BAD FILE FORMAT!" << endl;
		exit(0);
	}
	cout << "Reading input file ..." <<endl;

	int n;
	getline(in, buf);
	istringstream(buf) >> n;
	//vl = GRA(n);
	vl.reserve(n);
	cout << "Adding vertexes with defaults" << endl;
	int index = -1;
	for (int i = 0; i < n; i++){//adding Vertexes with defaults, i becomes the vertex id
		if (++index % 10000 == 0)
			cout << index << " ";
		addVertex(i);
	}
	cout << endl;

	getline(in, buf);//reading -distinc number of properties(labels)
	istringstream(buf) >> n;	
	//ll = LabelList(n);
	ll.reserve(n);

	getline(in, buf);
	istringstream(buf) >> n;
	Triple_Count_From_Triple_File = n;
		
	string sub,sub2;
	int idx; //pos
	int sid = 0; //vertex id
	int did = 0;// vertex id reached by lid
	int lid = 0;//label id

	index = -1;
	cout << "Reading and adding triples" << endl;
	while (getline(in, buf)) {
		if (++index % 10000 == 0)
			cout << index << " ";
		//buf = sid: lid= did1 did2 did3: lid= did4 did5 did6#
		idx = buf.find(":");
		if(idx < 0)
			continue;
		sub = buf.substr(0, idx);
		istringstream(sub) >> sid;
		//sid | sid
		buf.erase(0, idx+2);
		//buf | lid= did1 did2 did3: lid= did4 did5 did6#

		while ( (idx=buf.find_first_of(":#")) != string::npos) {
			sub = buf.substr(0, idx);
			//sub | lid= did1 did2 did3
			buf.erase(0, idx+2);
			//buf | lid= did4 did5 did6#
			idx = sub.find("=");
			if(idx < 0)
				continue;
			sub2 = sub.substr(0, idx);
			//sub2 = lid
			istringstream(sub2) >> lid;
			//lid = lid
			sub.erase(0, idx+2);
			//sub | did1 did2 did3
			do {
				idx = sub.find(" ");
				if(idx ==-1)
					idx = sub.length();
				sub2 = sub.substr(0, idx);
				//sub2 = did1
				istringstream(sub2) >> did;
				//did = did1
				sub.erase(0, idx+1);
				//sub = did2 did3
				/*if( sid == did )
				cout << "sid: " << sid << ", did: " << did << endl; //mehmet, i dont know why we are printing this
				*/
				addTriple(/*tindex ,*/sid,lid, did);//mehmet,  addEdge is handled inside this function
				//tindex++;
			}while(sub != "");
		}
		++sid;//mehmet, (why?)
	}
	cout << endl;
}

/*
	Called when the verbose option is specified from the main
*/
void Graph::writeGraph(ostream& out) {	
	//cout << "Graph size = " << vl.size() << endl;
	out << "graph_for_greach" << endl;
	out << vl.size() << endl;
	for( int i = 0; i < vl.size(); i++ )
	{
		out << i ;
		for ( auto it = vl[i].edge_out_map.begin(); it != vl[i].edge_out_map.end(); ++it )
		{
			out << ": " ;
			//it->first; //label_id
			//it->second;//EdgeOut struct
			out << it->first << "=" ;
			for( int j = 0; j < it->second.dest.size() ; j++ )
			{
				out<<" ";
				out << it->second.dest[j] ;
			}
		}
		out << "#" << endl;
	}
	out << "" << endl;
	out << "Incoming values:" << endl;
	for( int i = 0; i < vl.size(); i++ )
	{
		out << i ;
		for ( auto it = vl[i].edge_in_map.begin(); it != vl[i].edge_in_map.end(); ++it )
		{
			out << ": " ;
			//it->first; //label_id
			//it->second;//EdgeOut struct
			out << it->first << "=" ;
			for( int j = 0; j < it->second.src.size() ; j++ )
			{
				out<<" ";
				out << it->second.src[j] ;
			}
		}
		out << "#" << endl;
	}
}

void Graph::addTriple(int src, int label_id, int dest)
{
	if (src >= vl.size())
		addVertex(src);
	if (dest >= vl.size())
		addVertex(dest);

	if (label_id >= ll.size()) {
		int size = ll.size();
		for (int i = 0; i < (label_id-size+1); i++) {
			ll.push_back(Label());
		}
	}
	
	Label &lobj = ll[label_id];
	if(lobj.NumOfOccurence < 0)//not initialized
	{
		lobj.NumOfOccurence =0;
		lobj.isFrequentLabel= false;
		lobj.TotDiffObjects = 0;
		lobj.user_given_sim_weight = -1;
	}
	lobj.OV.insert(src);//for each label create a list of vertex ids as subject ( src, label_id, o => outgoing edge )
	lobj.OV2.insert(src);//dup - will be modified
	lobj.IV.insert(dest);//for each label create a list of vertex ids as object ( s, label_id, dest => incoming edge )
	lobj.NumOfOccurence++;

	//adding out Edge. add dest to src's outgoing
	if(!vl[src].edge_out_map.count(label_id))
	{
		EdgeOut edgeOut  = EdgeOut();
		edgeOut.src = src;
		edgeOut.label_id = label_id;
		edgeOut.dest = VectIntType();
		vl[src].edge_out_map[label_id] = edgeOut;
	}
	vl[src].edge_out_map[label_id].dest.push_back(dest);
	vl[src].degree++;

	//adding in Edge. add src to dest's incoming
	if(!vl[dest].edge_in_map.count(label_id))
	{
		EdgeIn edgeIn  = EdgeIn();
		edgeIn.src = VectIntType();
		edgeIn.label_id = label_id;
		edgeIn.dest = dest;
		vl[dest].edge_in_map[label_id] = edgeIn;
	}
	vl[dest].edge_in_map[label_id].src.push_back(src);
	vl[dest].in_degree++;
}

/*
	A new Vertex is initialized and added to the vertex list(vl) with default empty values
	If it is a value node its value part(string part) and data type part is parsed and set
*/
void Graph::addVertex(int vid) {
	if (vid >= vl.size()) {
		int size = vl.size();
		for (int i = 0; i < (vid-size+1); i++) {
			vl.push_back(Vertex());
		}
		vsize = vl.size();
	}

	Vertex v;
	v.degree = 0;//has to be initialized
	v.totalNumWord_Occurences = 0;
	v.in_degree = 0;
	v.weight = 1.0;
	EdgeInMap edge_in_map = EdgeInMap();
	EdgeOutMap edge_out_map = EdgeOutMap();
	v.edge_in_map = edge_in_map;
	v.edge_out_map = edge_out_map;
	v.valueNode = false;
	string node_text = vrmap[vid];
	int idx,idx2,idx3;
	int pos;
	if(node_text.length() > 0 && node_text[0] == '"') //attribute node such as "Annular or Degenerative Disease"^^<http://www.w3.org/2001/XMLSchema#string>
	{
		num_value_nodes++;
		v.valueNode = true;
		idx = node_text.rfind("\"^^");
		if(idx > 0) //data type info is available, as like this: "Annular or Degenerative Disease"^^<http://www.w3.org/2001/XMLSchema#string>
		{
			v.valueNode_label = node_text.substr(1, idx-1);
			/*idx2 = node_text.rfind("#");
			idx3 = node_text.rfind(">");
			if(idx2>0)
			v.valueNode_data_type = node_text.substr(idx2);
			else*/
			v.valueNode_data_type = node_text.substr(idx+3);
		}
		else
		{
			v.valueNode_label = node_text;
			v.valueNode_data_type = "unknown";
		}
		if ((pos = v.valueNode_label.find_last_of("\"@")) > 0) //example: "France"@en . //removing the language tag
			v.valueNode_label = v.valueNode_label.substr(0, pos);

		v.valueNode_label = Util::removeChar(v.valueNode_label, '"');
		v.valueNode_label_clean = Util::cleanValueNode(v.valueNode_label);
	}
	else
		num_iri_nodes++;

	vl[vid] = v;
}

/*
	return number of nodes(vertexes) in the graph
*/
int Graph::num_vertices() {
	return vl.size();
}

/*
	returns number of edges
	question: shouldn't this be equal to the number of triples. see Triples_Count_From_Triple_File
*/
int Graph::num_edges() {
	//mehmet, mod
	int num = 0;
	for( int i = 0; i < vl.size(); i++ )
	{
		for ( auto it = vl[i].edge_out_map.begin(); it != vl[i].edge_out_map.end(); ++it )
		{
			num+= it->second.dest.size();
		}
	}
	return num;	
}

// returns out edge struct of specified vertex and label_id ( v,label_id,EdgeOut )
EdgeOut& Graph::out_edges(int vertex_id, int label_id) {
	return vl[vertex_id].edge_out_map[label_id];
}

// returns in edges  struct of specified vertex and label_id (EdgeIn, label_id v )
EdgeIn& Graph::in_edges(int vertex_id, int label_id) {
	return vl[vertex_id].edge_in_map[label_id];
}

/*
	returns out degree , number of outgoing edges for the given vertex id
*/
int Graph::out_degree(int src) {	
	return vl[src].degree;	
}

/*
	returns in degree , number of incoming edges for the given vertex id
*/
int Graph::in_degree(int trg) {	
	return vl[trg].in_degree;	
}

/* 
	check whether the edge (src, trg) is in the graph
	I believe this is not being called. But it could be useful to decide whether the two given vertices are connected
*/
bool Graph::hasEdge(int src, int trg) {	
	for ( auto it = vl[src].edge_out_map.begin(); it != vl[src].edge_out_map.end(); ++it )
	{
		VectIntType el = it->second.dest;
		VectIntType::iterator ei;
		for (ei = el.begin(); ei != el.end(); ei++)
			if ((*ei) == trg)
				return true;
	}
	return false;
}

// return vertex list of graph
GRA& Graph::vertices() {
	return vl;
}

Graph& Graph::operator=(const Graph& g) {
	if (this != &g) {		
		vl = g.vl;
		vsize = g.vsize;
	}
	return *this;
}

// Returns the corresponding Vertex struct
Vertex& Graph::operator[](const int vid) {
	return vl[vid];
}

/*
	For each vertexes
	 . sort outgoing neighbor vertices ASC
	 . sort sort incoming neighbor vertices ASC
*/
void Graph::sortDirectedEdges() {	
	cout << "Sorting directed edges" << endl;
	for( int i = 0; i < vl.size(); i++ )
	{
		//no need to sort edge_out_map and edge_in_map because they are already sorted
		//sort outgoing edges ids
		for ( auto it = vl[i].edge_out_map.begin(); it != vl[i].edge_out_map.end(); ++it )
		{
			sort(it->second.dest.begin(), it->second.dest.end());
		}
		//sort incoming edges ids
		for ( auto it = vl[i].edge_in_map.begin(); it != vl[i].edge_in_map.end(); ++it )
		{
			sort(it->second.src.begin(), it->second.src.end());
		}
	}
}

float Graph::findOptimumClustering(ostream& out, int formula, float min_threshold, float max_threshold, int num_try, float last_opt_value, float opt_value_threshold, string ClusterVerificationFileName){//will dynamicallay set the dissimilarity threshold according to the generated clusters stability and will generate the clusters


	//resetClusters();
	/*classifyDataSet(0.4);
	nameClusters();
	generateClusterRelations();
	g.calculateClustersRMSD;
	return;*/
	//if sim >= 1 - try_threshold then they would get classified

	float add = (max_threshold - min_threshold) / (float)num_try;
	cout << endl << endl << "Formula: "<< formula<< "\tMin threshold: " << min_threshold << "\tMax threshold: " << max_threshold << endl;
	out << endl << endl << "Formula: " << formula << "\tMin threshold: " << min_threshold << "\tMax threshold: " << max_threshold << endl;
	cout << endl << endl << "Min threshold: " << min_threshold << "\tMax threshold: " << max_threshold << endl << "\num-try-per-loop: " << num_try << "\tincrement: " << add << endl;
	out << endl << endl << "Min threshold: " << min_threshold << "\tMax threshold: " << max_threshold << endl << "\num-try-per-loop: "<<num_try << "\tincrement: "<<add<<endl;
	float try_similarity_threshold = min_threshold;
	
	if (try_similarity_threshold <= 0)
		try_similarity_threshold = add;//we don't want it to be 0
	int i = 0;

	float opt_value = 0;
	float opt_threshold = try_similarity_threshold;
	float opt_stability = 0;
	float opt_cRMSD = 0;
	float opt_clusteredPercentage = 0;
	float opt_clustering_success_rate = 0;
	float value;
	float clusteredPercentage = 0;
	float avg_members_in_clusters = 0;
	float summaryGraph_cRMSD_add = summaryGraph_cRMSD + 0.1;
	//float clustering_success_rate = 0;
	while (try_similarity_threshold > 0 && try_similarity_threshold <1 && try_similarity_threshold <= max_threshold){
		cout << "Try: " << i << endl;
		out << "Try: " << i << endl;
		resetClusters();
		classifyDataSet(try_similarity_threshold);
		nameClusters();
		generateClusterRelations();
		calculateClustersRMSD();
		
		summaryGraph_cRMSD_add = summaryGraph_cRMSD + 0.1;
		
		/*if (ClusterVerificationFileName != ""){
			clustering_success_rate = verifyClusterResults(ClusterVerificationFileName, try_similarity_threshold);
			cout << endl << "Clustering success rate: " << clustering_success_rate << endl;
			out << endl << "Clustering success rate: " << clustering_success_rate << endl;
		}*/
		
		cout << endl<< "\nthreshold: " << try_similarity_threshold << "\t stability: " << summaryGraphStability << "\t summaryGraph_cRMSD: " << summaryGraph_cRMSD << endl;
		out << endl << "\nthreshold: " << try_similarity_threshold << "\t stability: " << summaryGraphStability << "\t summaryGraph_cRMSD: " << summaryGraph_cRMSD << endl;
		cout << "clusters: " << numGoodClusters << "\tlabelsSize: " << ll.size() << endl;
		out << "clusters: " << numGoodClusters << "\tlabelsSize: " << ll.size() << endl;
		
		/*cout << "clustering_success_rate: " << clustering_success_rate << endl;
		out << "clustering_success_rate: " << clustering_success_rate << endl;*/

		clusteredPercentage = ((float)numGoodClusteredNodes / (float)num_iri_nodes) * 100;
		avg_members_in_clusters = (float)numGoodClusteredNodes / (float)numGoodClusters;
		cout << "clusteredPercentage: %" << clusteredPercentage << endl;
		out << "clusteredPercentage: %" << clusteredPercentage << endl;
		cout << "avg_members_in_clusters: " << avg_members_in_clusters << endl;
		out << "avg_members_in_clusters: " << avg_members_in_clusters << endl;
				
		//value = summaryGraphStability* clusteredPercentage * (float)setClusters.size();

		if (formula == -10){
			cout << "value = numGoodClusters * summaryGraphStability* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			out << "value = numGoodClusters * summaryGraphStability* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			value = ( (float)numGoodClusters * summaryGraphStability* clusteredPercentage ) / summaryGraph_cRMSD_add;
		}
		else if (formula == -9){
			cout << "value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.75);" << endl;
			out << "value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.75);" << endl;
			value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.75);
			//value = summaryGraphStability* clusteredPercentage * numGoodClusters / ((summaryGraph_cRMSD + 0.75) * 100);
		}
		else if (formula == -8){
			cout << "value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.5);" << endl;
			out << "value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.5);" << endl;
			value = summaryGraphStability* clusteredPercentage / (summaryGraph_cRMSD + 0.5);
		}

		else if (formula == -7){
			cout << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			out << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage / summaryGraph_cRMSD_add;
		}
	    
		else if (formula == -6){
			cout << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;" << endl;
			out << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;" << endl;
			value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;
		}

		else if (formula == -5){
			cout << "value = summaryGraphStability* clusteredPercentage;" << endl;
			out << "value = summaryGraphStability* clusteredPercentage;" << endl;
			value = summaryGraphStability* clusteredPercentage;
		}

		else if (formula == -4){
			cout << "value = summaryGraphStability* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			out << "value = summaryGraphStability* clusteredPercentage / summaryGraph_cRMSD_add;" << endl;
			value = summaryGraphStability* clusteredPercentage / summaryGraph_cRMSD_add;
		}

		else if (formula == -3){
			cout << "value = (float)numGoodClusters / summaryGraph_cRMSD_add" << endl;
			out << "value = (float)numGoodClusters / summaryGraph_cRMSD_add" << endl;
			value = 1*((float)numGoodClusters) / summaryGraph_cRMSD_add;
		}
		else if (formula == -2){
			cout << "value = summaryGraphStability*(float)numGoodClusters / summaryGraph_cRMSD_add;" << endl;
			out << "value = summaryGraphStability*(float)numGoodClusters / summaryGraph_cRMSD_add;" << endl;
			value = summaryGraphStability*(float)numGoodClusters / summaryGraph_cRMSD_add;
		}
		else if (formula == -1){
			cout << "value = summaryGraphStability / summaryGraph_cRMSD_add;" << endl;
			out << "value = summaryGraphStability / summaryGraph_cRMSD_add;" << endl;
			value = summaryGraphStability / summaryGraph_cRMSD_add;
		}

		else if (formula == 0){
			cout << "value = 1 / summaryGraph_cRMSD_add;" << endl;
			out << "value = 1 / summaryGraph_cRMSD_add;" << endl;
			value = 1 / summaryGraph_cRMSD_add;
		}

		else if (formula == 1){
			cout << "value = summaryGraphStability;" << endl;
			out << "value = summaryGraphStability" << endl;
			value = summaryGraphStability;
		}
		else if (formula == 2){
			cout << "value = avg_members_in_clusters;" << endl;
			out << "value = avg_members_in_clusters" << endl;
			value = avg_members_in_clusters;
		}
		else if (formula == 3){
			cout << "value = clusteredPercentage;" << endl;
			out << "value = clusteredPercentage" << endl;
			value = clusteredPercentage;
		}
		else if (formula == 4){
			cout << "value = (float)numGoodClusters" << endl;
			out << "value = (float)numGoodClusters" << endl;
			value = (float)numGoodClusters;
		}
		else if (formula == 5){
			cout << "value = summaryGraphStability* avg_members_in_clusters;" << endl;
			out << "value = summaryGraphStability* avg_members_in_clusters;" << endl;
			value = summaryGraphStability* avg_members_in_clusters;
		}
		else if (formula == 6){
			cout << "value = summaryGraphStability* clusteredPercentage;" << endl;
			out << "value = summaryGraphStability* clusteredPercentage;" << endl;
			value = summaryGraphStability* clusteredPercentage;
		}
		else if (formula == 7){
			cout << "value = summaryGraphStability* numGoodClusters;" << endl;
			out << "value = summaryGraphStability* numGoodClusters;" << endl;
			value = summaryGraphStability* (float)numGoodClusters;
		}
		
		else if (formula == 8){
			cout << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;" << endl;
			out << "value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;" << endl;
			value = summaryGraphStability* avg_members_in_clusters* clusteredPercentage;
		}

		else if (formula == 9){
			cout << "value = summaryGraphStability* avg_members_in_clusters* (float)numGoodClusters;" << endl;
			out << "value = summaryGraphStability* avg_members_in_clusters* (float)numGoodClusters;" << endl;
			value = summaryGraphStability* avg_members_in_clusters* (float)numGoodClusters;
		}

		else if (formula == 10){
			cout << "value = summaryGraphStability* clusteredPercentage* (float)numGoodClusters;" << endl;
			out << "value = summaryGraphStability* clusteredPercentage* (float)numGoodClusters;" << endl;
			value = summaryGraphStability* clusteredPercentage* (float)numGoodClusters;
		}


		else{
			throw std::logic_error( "Formula has not been defined. ");
		}

		cout << "value: " << value << endl;
		out << "value: " << value << endl;
		if (value >= opt_value){
			opt_value = value;
			opt_threshold = try_similarity_threshold;
			opt_stability = summaryGraphStability;
			opt_cRMSD = summaryGraph_cRMSD;
			opt_clusteredPercentage = clusteredPercentage;
			//opt_clustering_success_rate = clustering_success_rate;
		}
		//cout << "num clustered iris: " << vToClusterMap.size() << "\t num un-clustered iris: " << num_iri_nodes - vToClusterMap.size()<< endl;

		try_similarity_threshold += add;
		i++;
	}
	cout << endl << "opt_threshold: " << opt_threshold << endl;
	out << endl << "opt_threshold: " << opt_threshold << endl;
	cout << "opt-value: " << opt_value << endl;
	out << "opt-value: " << opt_value << endl;
	cout << "opt-stability: " << opt_stability << endl;
	out << "opt-stability: " << opt_stability << endl;

	cout << "opt-cRMSD: " << opt_cRMSD << endl;
	out << "opt-cRMSD: " << opt_cRMSD << endl;

	/*cout << "opt-clustering_success_rate: " << opt_clustering_success_rate << endl;
	out << "opt-clustering_success_rate: " << opt_clustering_success_rate << endl;*/

	cout << "opt-clusteredPercentage: " << opt_clusteredPercentage << endl;
	out << "opt-clusteredPercentage: " << opt_clusteredPercentage << endl;

	if (opt_threshold >0 && (opt_threshold - add) > 0 && (opt_threshold + add) < 1 && opt_value > last_opt_value && (opt_value - last_opt_value) > opt_value_threshold)
		findOptimumClustering(out, formula, opt_threshold - add, opt_threshold + add, num_try, opt_value, opt_value_threshold, ClusterVerificationFileName);
	return opt_threshold;
}

/*
Reset clusters
*/
void Graph::resetClusters()
{
	//cout << endl << "Resetting clusters.." << endl;
	summaryGraphStability = 0;
	summaryGraph_cRMSD = 0;
	numGoodClusteredNodes = 0;
	numGoodClusters = 0;
	numEdgesInSummaryGraph = 0;
	numGoodEdgesInSummaryGraph = 0;
	vToClusterMap.clear();
	Cluster *c;
	for (int i = 0; i < setClusters.size(); i++)
	{
		c = &setClusters.at(i); // current cluster
		//c->members.clear();
		//c->edge_in_map.clear();
		//c->edge_out_map.clear();
		//c->propertyStabilityByRefCluster.clear();
		//delete &c->members;
		//delete &c->name;
		//delete &c->degree;

		//delete &c->edge_in_map;
		//delete &c->edge_out_map;
		//delete &c->in_degree;		
		//delete &c->propertyStabilityByRefCluster;
		//delete &c->propery_coverage_ratio;
		//delete &c->size;
		//delete &c->stability_ratio;
		//delete c;
	}
	setClusters.clear();
}

/*
calculates clusters' Root-mean-square deviation. should be used to guess dissimilarity threshold
*/
void Graph::calculateClustersRMSD(){
	
	Cluster *c;
	int vid, v_label_id;
	set<int> cProperties;
	vector<float> cCentroid;
	float cRMSD = 0;
	float cRMSD_total = 0;
	for (int i = 0; i < setClusters.size(); i++)
	{
		cProperties.clear();
		cCentroid.clear();
		cRMSD = 0;
		c = &setClusters.at(i);
		if (c->size <=1 )
			continue;
		
		//1. create the set of properties used in the cluster
		//cout<< endl<< "Processing cluster: " << c->name << endl;
		for (int j = 0; j < c->size; j++) // process current cluster's members
		{
			vid = c->members.at(j); // member vertex id
			EdgeOutMap &v_edge_out_map = vl[vid].edge_out_map;
			for (auto it = v_edge_out_map.begin(); it != v_edge_out_map.end(); ++it)
			{
				v_label_id = it->first;// vertex triple label id
				cProperties.insert(v_label_id);
			}
		}

		/*properties = [2, 9, 5];
		member1    = [1, 0, 1];
		member2 =    [1, 1, 0];
		member3 =    [1, 0, 1];
		
		centrodi =   [3/3, 1/3, 2/3];
		member1 =    [1,   0,     1];
		member2 =    [1, 1, 0];
		member3 =    [1, 0, 1];
		*/


		//2. calculate and generate the cluster's centroid , for each property check whether the member has the property
		float propertyExistTotal = 0;
		
		std::set<int>::iterator it;
		for (it = cProperties.begin(); it != cProperties.end(); ++it)
		{
			propertyExistTotal = 0;
			v_label_id = *it; // Note the "*" here
			for (int j = 0; j < c->size; j++) // process current cluster's members
			{
				vid = c->members.at(j); // member vertex id				
				//either 0 or 1
				if (vl[vid].edge_out_map.count(v_label_id))
					propertyExistTotal = propertyExistTotal + 1;
				else
					propertyExistTotal = propertyExistTotal + 0;
			}
			cCentroid.push_back(propertyExistTotal / (float)c->members.size());
		}
		//2. calculate RMSD based on cluster's centroid distance to each member
		float distanceTotal = 0;
				
		int index = 0;
		for (it = cProperties.begin(); it != cProperties.end(); ++it)
		{
			distanceTotal = 0;
			v_label_id = *it;
			for (int j = 0; j < c->size; j++) // process current cluster's members
			{
				vid = c->members.at(j); // member vertex id				
				//either 0 or 1
				if (vl[vid].edge_out_map.count(v_label_id)){
					distanceTotal+= pow(1 - cCentroid[index], 2);
				}
				else{
					distanceTotal+= pow(0 - cCentroid[index], 2);
				}
			}
			index++;
		}
		cRMSD = sqrt( distanceTotal / (float)(c->size) );
		c->cRMSD = cRMSD;
		cRMSD_total += c->cRMSD;
	}
	summaryGraph_cRMSD = cRMSD_total;

	cout << "summaryGraph_cRMSD: " << summaryGraph_cRMSD << endl;
}

//g.showReductionRates();
void Graph::showReductionRates(ostream& out,  int num_orig_vertices, int num_orig_edges){
	out << "Original #vertices: " << num_orig_vertices << "\t" << "Original #properties: " << num_orig_edges << endl;
	out << "SummaryGraph #clusters: " << numGoodClusters << "\t" << "SummaryGraph #properties: " << numEdgesInSummaryGraph << endl;
	out << "SummaryGraph #clusters: " << numGoodClusters << "\t" << "SummaryGraph #properties-good: " << numGoodEdgesInSummaryGraph << endl;
	float reduction_rate1 = (float)100.0 -  ( ((float)(numGoodClusters + numEdgesInSummaryGraph) / (float)(num_orig_vertices + num_orig_edges)) * (float)100.0 );
	out << "Reduction rate (including 1-size neighbors): " << reduction_rate1 << endl;

	float reduction_rate2 = (float)100.0 - (((float)(numGoodClusters + numGoodEdgesInSummaryGraph) / (float)(num_orig_vertices + num_orig_edges)) * (float)100.0);
	out << "Reduction rate (excluding 1-size neighbors): " << reduction_rate2 << endl;

	cout << "Reduction rate (including 1-size neighbors): " << reduction_rate1 << endl;
	cout << "Reduction rate (excluding 1-size neighbors): " << reduction_rate2 << endl;

}
