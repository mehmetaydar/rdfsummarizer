/*
new submission - 2018-June:
. Dbpedia (100k dataset):
Rolesim: add break when it reaches to 100k: -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_en.nt.bz2.nt.bz2.nt" -nlb "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\schema discovery study\dbpedia_noise_props.csv"
save it as infobox_properties_100000_flat.txt and infobox_properties_100000_map.txt
Linstamatch:  and give infobox_properties_100000_flat.txt as input, and infobox_properties_100000_map.txt
this generated: infobox_properties_100000_flat.txt_minhashpairs.txt as the initial candidate pairs using MinHash method.
string rdf_flat_file = @"C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_100000_flat.txt";
int numHashFunctions = 250;
double simThreshold = 0.33;
bool exclude_sim_under_threshold = false;
Tuned: True
Num hash functions: 250
Maximum accepted false negative rate: 0.05
sim-threshold: 0.33
Rolesim: -f flat --literal_sims --auto_weights -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_100000_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_100000_map.txt" -cp "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_100000_flat.txt_minhashpairs.txt" -nw "data-sets/noise-words.txt"
. SDB:
Could not get the dataset
. Lubm
-f triple -g "data-sets/Lubm/university_all.txt" -nlb "data-sets/Lubm/noise-props.txt"
-f flat --auto_weights --literal_sims -g "data-sets/Lubm/university_all.txt_flat.txt" -gm "data-sets/Lubm/university_all.txt_map.txt" -nw "data-sets/noise-words.txt" -nlb "data-sets/Lubm/noise-props.txt" -cv "data-sets/Lubm/university_all_verification.txt"



examples:
--ISWC 2015 Configuration:
Serkan
--Json Cluster printing
-f flat  -g "data-sets/university/sparql_university_4.txt_flat.txt" -gm "data-sets/university/sparql_university_4.txt_map.txt" -j
-f flat  -g "data-sets/university/sparql_university.txt_flat.txt" -gm "data-sets/university/sparql_university.txt_map.txt" -j

Mehmet
ISWC2015 Configuration
lubm1
ls-aw
without noise-props exclusions
-f triple -g "data-sets/iswc/lubm/ls-aw/university0_test.txt"
-f flat --literal_sims --auto_weights -g "data-sets/iswc/lubm/ls-aw/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/ls-aw/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt"

ls-aw
with noise-props exclusions
-f triple -g "data-sets/iswc/lubm/ls-aw/nlb/university0_test.txt" -nlb "data-sets/iswc/lubm/noise-props.txt"
-f flat --auto_weights --literal_sims -g "data-sets/iswc/lubm/ls-aw/nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/ls-aw/nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -nlb "data-sets/iswc/lubm/noise-props.txt" -cv "data-sets/iswc/lubm/university0_test_verification.txt"
vs
aw
with noise-props exclusions
-f triple -g "data-sets/iswc/lubm/aw/nlb/university0_test.txt" -nlb "data-sets/iswc/lubm/noise-props.txt"
-f flat --auto_weights -g "data-sets/iswc/lubm/aw/nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/aw/nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -nlb "data-sets/iswc/lubm/noise-props.txt" -cv "data-sets/iswc/lubm/university0_test_verification.txt"
vs
core
with noise-props exclusions
-f triple -g "data-sets/iswc/lubm/core/nlb/university0_test.txt" -nlb "data-sets/iswc/lubm/noise-props.txt"
-f flat -g "data-sets/iswc/lubm/core/nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/core/nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/iswc/lubm/university0_test_verification.txt"


-------
ls-aw
without noise-props exclusions
-f triple -g "data-sets/iswc/lubm/ls-aw/no-nlb/university0_test.txt"
-f flat --auto_weights --literal_sims -g "data-sets/iswc/lubm/ls-aw/no-nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/ls-aw/no-nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/iswc/lubm/university0_test_verification.txt"
vs
aw
without noise-props exclusions
-f triple -g "data-sets/iswc/lubm/aw/no-nlb/university0_test.txt"
-f flat --auto_weights -g "data-sets/iswc/lubm/aw/no-nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/aw/no-nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/iswc/lubm/university0_test_verification-mod.txt"
vs
core
without noise-props exclusions
-f triple -g "data-sets/iswc/lubm/core/no-nlb/university0_test.txt"
-f flat -g "data-sets/iswc/lubm/core/no-nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/core/no-nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/iswc/lubm/university0_test_verification-mod.txt"

--------
-f triple -g "data-sets/iswc/sparql_university_4.txt"
-f flat  -g "data-sets/iswc/sparql_university_4.txt_flat.txt" -gm "data-sets/iswc/sparql_university_4.txt_map.txt"
aw
-f flat --auto_weights -g "data-sets/iswc/aw/sparql_university_4.txt_flat.txt" -gm "data-sets/iswc/aw/sparql_university_4.txt_map.txt"
ls-aw
-f flat --literal_sims --auto_weights -g "data-sets/iswc/ls-aw/sparql_university_4.txt_flat.txt" -gm "data-sets/iswc/ls-aw/sparql_university_4.txt_map.txt" -nw "data-sets/noise-words.txt"
...sdb
normal
-f triple -g "data-sets/iswc/sdb/show-200.txt"
-f flat -g "data-sets/iswc/sdb/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/show-200_verification.txt"
ls
-f flat --literal_sims -g "data-sets/iswc/sdb/ls/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/ls/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/ls/show-200_verification.txt" -nw "data-sets/noise-words.txt"
aw
-f flat --auto_weights -g "data-sets/iswc/sdb/aw/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/aw/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/aw/show-200_verification.txt"
ls-aw
-f flat --auto_weights --literal_sims -g "data-sets/iswc/sdb/ls-aw/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/ls-aw/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/ls-aw/show-200_verification.txt" -nw "data-sets/noise-words.txt"
-f flat --auto_weights --literal_sims -g "data-sets/iswc/sdb/ls-aw/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/ls-aw/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/ls-aw/show-200_verification.txt" -nw "data-sets/noise-words.txt" -j


=======

>>>>>>> .r88
COMPSAC2015 Configuration:
To Generate Mappings
--DbPedia
-f triple -g "data-sets/university/sparql_university_4.txt" -nlb "data-sets/university/noise-properties.txt"
-f triple -g "data-sets/university/sparql_university_3.txt" -nlb "data-sets/university/noise-properties.txt"
-f triple -g "data-sets/university/sparql_university_2.txt"
-f triple -g "data-sets/university/sparql_university.txt"

--sdb
--v -f triple -g "data-sets/sdb/show-3.txt"
--v -f triple -g "data-sets/sdb/show-80.txt"
--v -f triple -g "data-sets/sdb/show-100.txt"
--v -f triple -g "data-sets/sdb/show-300.txt"
--v -f triple -g "data-sets/sdb/show-600.txt"
--v -f triple -g "data-sets/sdb/show-200.txt"
To Generate the Sim Results

--DbPedia
-f flat  -g "data-sets/university/sparql_university_4.txt_flat.txt" -gm "data-sets/university/sparql_university_4.txt_map.txt" -nlb "data-sets/university/noise-properties.txt"
results: #4064 triples - took 10s - %93.7 accuracy
-f flat  -g "data-sets/university/sparql_university_3.txt_flat.txt" -gm "data-sets/university/sparql_university_3.txt_map.txt" -nlb "data-sets/university/noise-properties.txt"
-f flat  -g "data-sets/university/sparql_university_2.txt_flat.txt" -gm "data-sets/university/sparql_university_2.txt_map.txt"
-f flat  -g "data-sets/university/sparql_university.txt_flat.txt" -gm "data-sets/university/sparql_university.txt_map.txt"

--sdb
-f flat -g "data-sets/sdb/show-3.txt_flat.txt" -gm "data-sets/sdb/show-3.txt_map.txt" -cv "data-sets/sdb/show-3_verification.txt"
-f flat -g "data-sets/sdb/show-80.txt_flat.txt" -gm "data-sets/sdb/show-80.txt_map.txt" -cv "data-sets/sdb/show-80_verification.txt"
results: #3369 triples - took 1 minute - 78%
-f flat -g "data-sets/sdb/show-100.txt_flat.txt" -gm "data-sets/sdb/show-100.txt_map.txt" -cv "data-sets/sdb/show-100_verification.txt"
-f flat -g "data-sets/sdb/show-300.txt_flat.txt" -gm "data-sets/sdb/show-300.txt_map.txt" -cv "data-sets/sdb/show-300_verification.txt"
results: #12260 triples - took 9 minutes - %77.5 accuracy
-f flat -g "data-sets/sdb/show-600.txt_flat.txt" -gm "data-sets/sdb/show-600.txt_map.txt" -cv "data-sets/sdb/show-600_verification.txt"
-f flat -g "data-sets/sdb/show-200.txt_flat.txt" -gm "data-sets/sdb/show-200.txt_map.txt" -cv "data-sets/sdb/show-200_verification.txt"
results: 2.5min 86.8 accuracy 0.5 threshold / 6450 triples


--v -k 1000 -g "data-sets/sdb/demonstration/show0.txt_flat.txt" -gm "data-sets/sdb/demonstration/show0.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/sdb/demonstration/show0_verification.txt"

--v -k 1000 -g "data-sets/sdb/demonstration/show0.txt_flat.txt" -gm "data-sets/sdb/demonstration/show0.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/sdb/demonstration/show0_verification.txt"
--v -f triple -g "data-sets/sdb/demonstration/show0.txt"

--v -k 1000 -g "data-sets/sdb/show0.txt_flat.txt" -gm "data-sets/sdb/show0.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/sdb/show0_verification.txt"
--v -f triple -g "data-sets/sdb/show0.txt"

<<<<<<< .mine
-v -k 1000 -g "data-sets/sdb/extract_show0.txt_flat.txt" -gm "data-sets/sdb/extract_show0.txt_map.txt -nw data-sets/noise-words.txt" -cv "data-sets/sdb/"
=======
--v -k 1000 -g "data-sets/sdb/extract_show0.txt_flat.txt" -gm "data-sets/sdb/extract_show0.txt_map.txt -nw data-sets/noise-words.txt" cv "data-sets/sdb/"
>>>>>>> .r73

--v -k 1000 -g "data-sets/sdb/big0.txt_flat.txt" -gm "data-sets/sdb/big0.txt_map.txt -nw data-sets/noise-words.txt"
--v -f triple -g "data-sets/sdb/big0.txt"

--v -k 1000 -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_show0.txt_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_show0.txt_map.txt" -nw "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\noise-words.txt"
--v -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_show0.txt"

--v -k 1000 -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_sel_1.txt_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_sel_1.txt_map.txt" nw "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\noise-words.txt"
--v -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_sel_1.txt"

--v -k 1000 -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_med_1.txt_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_med_1.txt_map.txt" -nw "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\noise-words.txt"
--v -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract_med_1.txt"

--v -k 100000 -f flat -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract0.txt_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract0.txt_map.txt" -nw "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\noise-words.txt"
--v -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract0.txt"

--v -k 100000 -f flat -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract0.txt_flat.txt" -gm "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\sdb-extract\extract0.txt_map.txt" -nw "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\Data Sets\noise-words.txt"

--v -f triple -g "sparql_university.txt"

--v --literal_sims --auto_weights -f flat  -g "sparql_university.txt_flat.txt" -gm "sparql_university.txt_map.txt"

-f flat  -g "sparql_university.txt_flat.txt" -gm "sparql_university.txt_map.txt"
-f flat  -g "sparql_university.txt_flat.txt" -gm "sparql_university.txt_map.txt" -tnl sparql_university_target_vertexes.txt

-f flat  -g "Schema_AllInOne.nt_flat.txt" -gm "Schema_AllInOne.nt_map.txt" -tnl "Schema_All_target_vertexes.txt"
--v -f triple -g "Schema_AllInOne.nt" -nlb "Schema_noise_labels.txt"
-f flat  -g "Schema_rep_6_table_502.nt_flat.txt" -gm "Schema_rep_6_table_502.nt_map.txt"
--v -f triple -g "Schema_rep_6_table_502.nt" -nlb "Schema_noise_labels.txt"
-f flat  -g "sparql_university.txt_flat.txt" -gm "sparql_university.txt_map.txt" -tnl sparql_university_target_vertexes.txt
-f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_en.nt.bz2.nt.bz2.nt" -nlb "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\schema discovery study\dbpedia_noise_props.csv"
--v -f triple -g "C:\Users\maydar\Documents\Sony Backup\PHD\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_en.nt.bz2.nt.bz2.nt" -s rs -nlb "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\schema discovery study\dbpedia_noise_props.csv" -tnl "sparql_university_target_vertexes.txt"
--v -f triple -g "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\dbpedia\infobox\infobox_properties_en.nt.bz2.nt.bz2.nt" -s rs -nlb "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\schema discovery study\dbpedia_noise_props.csv"
--v -f flat  -g "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\dbpedia\infobox\infobox2_properties_en.nt.bz2.nt.bz2.nt_flat.txt" -gm "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\dbpedia\infobox\infobox2_properties_en.nt.bz2.nt.bz2.nt_map.txt" -s rs
--v -f flat  -g "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\dbpedia\infobox\infobox3_properties_en.nt.bz2.nt.bz2.nt_flat.txt" -gm "C:\Users\maydar\Documents\PHD COURSES\SEMANTIC STUDY\dbpedia\infobox\infobox3_properties_en.nt.bz2.nt.bz2.nt_map.txt" -s rs

-f flat  -g "sparql_university.txt_flat.txt" -gm "sparql_university.txt_map.txt" -s rs

--v -f triple  -g "..\..\..\dbpedia\infobox\infobox_properties_en.nt.bz2.nt.bz2.nt" -s rs -nlb "..\..\..\schema discovery study\dbpedia_noise_props.csv"

--v -f flat  -g "..\..\..\dbpedia\infobox\infobox_properties_500_flat.txt" -gm "..\..\..\dbpedia\infobox\infobox_properties_500_map.txt" -s rs
--v -f flat  -g "..\..\..\dbpedia\infobox\infobox_properties_1000_flat.txt" -gm "..\..\..\dbpedia\infobox\infobox_properties_1000_map.txt" -s rs
--v -f flat  -g "..\..\..\dbpedia\infobox\infobox_properties_2000_flat.txt" -gm "..\..\..\dbpedia\infobox\infobox_properties_2000_map.txt" -s rs
--v -f flat  -g "..\..\..\dbpedia\infobox\infobox_properties_10000_flat.txt" -gm "..\..\..\dbpedia\infobox\infobox_properties_10000_map.txt" -s rs
--v -f flat  -g "..\..\..\dbpedia\infobox\infobox_properties_50000_flat.txt" -gm "..\..\..\dbpedia\infobox\infobox_properties_50000_map.txt" -s rs

--v -f triple -g "..\..\..\dbpedia\3.8\infobox_ontology\mappingbased_properties_en.nt.bz2.nt.bz2.nt" -s rs
--v -f flat -g "..\..\..\dbpedia\3.8\infobox_ontology\mappingbased_properties_50000_flat.txt" -gm "..\..\..\dbpedia\3.8\infobox_ontology\mappingbased_properties_50000_map.txt" -s rs

*/

/* Main program for RdfSummarizer.
Three
*/
//#define _TIMEVAL
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Graph.h"
#include "Conf.h"
#include "SimMatrix.h"
#include <ctime>

using namespace std;
int N;
const float DEF_BETA = 0.15;
const float DEF_THRESHOLD = 0.001;
//const float DEF_IN_WEIGHT = 0.5;
const float DEF_IN_WEIGHT = 0;
const int DEF_MAXITER = 25;

//mehmet added.
const float DEF_FREQUENT_PROPERTY_THRESHOLD = 0.99;//percentage ,  this is used to exclude frequently occured properties (labels) when generating common vertex pairs
const float DEF_CLUSTER_MEMBER_SIMILARITY_CLUSTER = 0.3;//maximum dis-similarity (minimum required similarity) to be in the same cluster

const bool DEF_AUTO_WEIGHTS = false;
const bool DEF_LITERAL_SIMS = false;

const int DEF_max_triple_size = -1;

template <class T>
inline std::string toString(const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

void usage() {
	cout << "\nUsage:\n"
		"main -h\n"
		"main -graph g [-format f] [-beta b][-inWeight w][-threshold t][-maxIter k]\n"
		" [-result rfile] [-out ofile] [--verbose] [-precision p] [-frequent_propery_threshold fpt] [-cluster_member_similarity_threshold cmst]\n"

		">>>Description:\n"
		"	-h					Print the help message.\n"
		"	-g, -graph g		dataset name, REQUIRED\n"
		"	-gm, -graph_map gm		(vertex_ids to vertex names) mapping file: default: graphFileName+_map.txt\n"
		"	-cv, -cluster_verification cv		Cluster verification file. (the real clusters in the data set. If provided it will be used to verify the algorithm results)\n"
		"	-nlb, -noise_labels nlb		noise labels(properties) to ignore\n"
		"	-nw, -noise_words nw		noise words to ignore in string similarity calculation\n"
		"	-f, -format flat	the format of the input graph file: flat(default)(pre-processed), triple(.nt graph format)\n"
		"	-mts, -max_triple_size , default=-1(no limit), max-triple-size will break reading the nt triple file if reached to this number. good when format=triple\n"
		"	-cp, -common_pairs_list the list of node pairs having at least a common label. this usually comes from MinHash.\n"
		"	-tnl, -target_node_list. initial target node list to calculate similarities and related clusters. This will only process similarities for a set of target and related vertexes.(less complexity)\n"
		">>>Computation options:\n"
		"	--aw, --auto_weights			auto calculate descriptor(properties/words) importance weights(default:" << DEF_AUTO_WEIGHTS << "). If set this option will have the algorithm auto-calculate the predicates and literal's words importance per each IRI node.\n"
		"	--ls, --literal_sims			include literal nodes in similarity calculations:" << DEF_LITERAL_SIMS << "). If set this option will have the algorithm to include the literal nodes in the similarity calculations.\n"

		//"	-inw, -inWeight a	for directed graphs, fractional weight (default " << DEF_IN_WEIGHT << ") for in-neighbors, i.e., sim = a*in_sim + (1-a)*out_sim.\n"
		"	-b, -beta b			beta(default:" << DEF_BETA << ") Where beta is a decay factor for RdfSummarizer calculation and (0 < beta< 1).\n"
		"	-t, -threshold t	threshold value(default: " << DEF_THRESHOLD << ") Where threshold is the minimum value for the similarity iteration convergence.\n"
		"	-maxIter k			maximum number of iterations (default: " << DEF_MAXITER << ") Where maxIter is the maximum number of iterations for RdfSummarizer calculation.\n"

		"	-frequent_propery_threshold fpt			threshold to determine whether a property is frequent. (default: " << DEF_FREQUENT_PROPERTY_THRESHOLD << "). Frequent properties are excluded when generating common vertex pairs.\n"
		"	-cluster_member_similarity_threshold -cmst			threshold of the maximum dis-similarity (minimum required similarity) to be in the same cluster. (default: " << DEF_CLUSTER_MEMBER_SIMILARITY_CLUSTER << "). Used in clustering.\n"

		"Output options:\n"
		"	-result    rfile	time and rounds result file name(default:result.txt).\n"
		"	-o, -out   outfile	output file name which includes pair similarities. (default:auto-generated name describing input options).\n"
		"	--v, --verbose		send lots of progress info (including every RdfSummarizer interation) to stdout\n"
		"	-p, -precision p	bits of precision to write/read for result values (default 3)\n"

		"EXAMPLES:\n"
		"To convert an .nt formatted graph file into flat file format that RdfSummarizer can read:\n"
		"./main -f triple -g myNtGraphFile\n"
		"\n"
		"To run RdfSummarizer on a flat formatted file with a given graph map file, with a cluster-member-dissimilarity threshold of 0.5:\n"
		"./main -cmst 0.5 -f flat  -g myFlatGraphFile -gm myGraphMapFile\n"
		"\n"

		"To run RdfSummarizer on a flat formatted file with a given graph map file, with a cluster-member-dissimilarity threshold of 0.5 and with a given common-pairs-list file:\n"
		"./main -cmst 0.5 -f flat  -g myFlatGraphFile -gm myGraphMapFile -cp common-pairs-list-file\n"
		"\n"

		"To run RdfSummarizer on a flat formatted file with a given graph map file, by including literal similarities:\n"
		"./main --literal_sims -f flat  -g myFlatGraphFile -gm myGraphMapFile\n"
		"\n"

		"To run RdfSummarizer on a flat formatted file with a given graph map file, auto-weighting the descriptors(properties, literal words):\n"
		"./main --auto_weights -f flat  -g myFlatGraphFile -gm myGraphMapFile\n"
		"\n"

		"To run RdfSummarizer on a flat formatted file with a given graph map file, including literal similarities and auto-weighting the descriptors(properties, literal words):\n"
		"./main --literal_sims --auto_weights -f flat  -g myFlatGraphFile -gm myGraphMapFile\n"
		"\n"

		"To run RdfSummarizer on a flat formatted file with a given graph map file and with a given noise properties to ignore:\n"
		"./main -f flat  -g myFlatGraphFile -gm myGraphMapFile -nlb myNoisePropertiesFile\n"
		"\n"
		"To run RdfSummarizer on a flat formatted file with a given graph map file and with a given noise words to ignore:\n"
		"./main -f flat  -g myFlatGraphFile -gm myGraphMapFile -nw myNoiseWordsFile\n"
		"\n"
		"To run RdfSummarizer on a flat formatted file with a given graph map file and with a given cluster verification file:\n"
		"./main -f flat  -g myFlatGraphFile -gm myGraphMapFile -cv myClusterVerificationFile\n"
		"\n"
		"To run RdfSummarizer on a flat formatted file with a given graph map file and with a given initial target node list file:\n"
		"./main -f flat  -g myFlatGraphFile -gm myGraphMapFile -tnl myTargetNodeListFile\n"
		<< endl;
}

string timeToString(double time) {
	ostringstream ss;
	int hr = (int)(time / 3600.0);
	double remainingSec = time - hr*3600.0;
	int min = (int)(remainingSec / 60.0);
	string minSpace = (min < 10) ? ":0" : ":";
	double sec = remainingSec - min*60.0;
	string secSpace = (sec < 10) ? ":0" : ":";
	ss << hr << minSpace << min << secSpace << sec << " (" << time << " sec)";
	return ss.str();
}

void displayElapsedTime(time_t& previous, ostream& out, string message) {
	time_t current = time(NULL);
	double diff = difftime(current, previous);
	if (message == "")
		out << "elapsed time = " << timeToString(diff) << endl;
	else
		out << message << " took " << timeToString(diff) << endl;
	cout << endl;
	previous = current;
}

void keepResult(char* resultFileName, char* experimentName, double constructionTime,
	float threshold, float beta, int round, float inWeight) {
	ofstream out(resultFileName, ios_base::out | ios_base::app);
	time_t now = time(NULL);
	out << ctime(&now);
	out << "*********************************************************" << endl;
	out << experimentName;
	out << ", Thresh:" << threshold << ", Beta:" << beta << endl;
	out << ", inWt:" << inWeight;
	out << ", Time = " << timeToString(constructionTime) << "\tRounds = " << round << endl;
	out.close();
	cout << "Time = " << timeToString(constructionTime);
}

string autoName(char* GraphFileName, float inWeight, float threshold, float beta, int bitsPrecision)
{
	ostringstream ss;
	string GraphBase(GraphFileName);
	GraphBase = GraphBase.substr(0, GraphBase.find_last_of("."));
	ss << GraphBase;

	ss << "D";
	if (inWeight != DEF_IN_WEIGHT) {
		ss << inWeight * 10;
	}

	if (threshold != DEF_THRESHOLD) {
		ss << "-T" << (threshold * 1000);
	}
	if (beta != DEF_BETA) {
		ss << "-B" << (beta * 1000);
	}

	ss << "-sim";

	ss << ".txt";
	return ss.str();
}

int main(int argc, char* argv[]) {
	time_t start_beginning1 = time(NULL);
	time_t start_beginning2 = time(NULL);
	Conf conf = Conf();
	char* GraphFileName = "";
	string MapGraphFileName = "";//the mapping file to reverse the ids(id to nodes)
	string ClusterVerificationFileName = ""; //if provided the algorithm results success will be verified with the input of this file
	string PairsSimOutputFileName = "SimMatrix.txt";
	string NoiseLabelsFileName = "";
	string NoiseWordsFileName = "";
	string InitialTargetNodesListFileName = "";
	string CommonPairsListFileName = "";
	char* TimeAndRounds_ResultFileName = "result.txt";

	bool auto_weights = DEF_AUTO_WEIGHTS;
	bool literal_sims = DEF_LITERAL_SIMS;

	float threshold = DEF_THRESHOLD;
	float beta = DEF_BETA;
	bool verbose = false;
	int	maxIter = DEF_MAXITER;

	float frequent_propery_threshold = DEF_FREQUENT_PROPERTY_THRESHOLD;
	bool auto_cluster_member_similarity_threshold = false;
	float cluster_member_similarity_threshold = DEF_CLUSTER_MEMBER_SIMILARITY_CLUSTER;
	float clustering_success_rate = -1;

	int bitsPrecision = 3;
	float inWeight = DEF_IN_WEIGHT;
	bool PairsSimOutputFile_userOutputFile = false;
	bool printJSON = false;

	string input_graph_format = "flat"; //mehmet, added
	int max_triple_size = DEF_max_triple_size;
#ifdef _TIMEVAL
	struct timeval after_time, before_time;
#endif
	double construction_time = 0.0;

	int i = 1;
	//command input process
	cout << argc << " arguments" << endl;
	while (i < argc) {
		cout << argv[i] << " " << endl;
		if (strcmp("-h", argv[i]) == 0) {
			usage();
			exit(0);
		}

		if (strcmp("-graph", argv[i]) == 0 || strcmp("-g", argv[i]) == 0) {
			i++;
			GraphFileName = argv[i++];
			conf.setOp("graph", GraphFileName);
		}
		else if (strcmp("-graph_map", argv[i]) == 0 || strcmp("-gm", argv[i]) == 0) {
			i++;
			MapGraphFileName = argv[i++];
			conf.setOp("graph_map", MapGraphFileName);
		}
		else if (strcmp("-cluster_verification", argv[i]) == 0 || strcmp("-cv", argv[i]) == 0) {
			i++;
			ClusterVerificationFileName = argv[i++];
			conf.setOp("cluster_verification", ClusterVerificationFileName);
		}
		else if (strcmp("-noise_labels", argv[i]) == 0 || strcmp("-nlb", argv[i]) == 0) {
			i++;
			NoiseLabelsFileName = argv[i++];
			conf.setOp("noise_labels", NoiseLabelsFileName);
		}
		else if (strcmp("-noise_words", argv[i]) == 0 || strcmp("-nw", argv[i]) == 0) {
			i++;
			NoiseWordsFileName = argv[i++];
			conf.setOp("noise_words", NoiseWordsFileName);
		}
		else if (strcmp("-result", argv[i]) == 0) {
			i++;
			TimeAndRounds_ResultFileName = argv[i++];
			conf.setOp("result", TimeAndRounds_ResultFileName);
		}
		else if (strcmp("-out", argv[i]) == 0 || strcmp("-o", argv[i]) == 0) {
			i++;
			PairsSimOutputFile_userOutputFile = true;
			PairsSimOutputFileName = argv[i++];
			conf.setOp("out", PairsSimOutputFileName);
			conf.setOp("PairsSimOutputFile_userOutputFile", "1");
		}

		else if (strcmp("-frequent_propery_threshold", argv[i]) == 0 || strcmp("-fpt", argv[i]) == 0) {
			i++;
			frequent_propery_threshold = atof(argv[i++]);
			conf.setOp("frequent_propery_threshold", argv[i - 1]);
		}
		else if (strcmp("-cluster_member_similarity_threshold", argv[i]) == 0 || strcmp("-cmst", argv[i]) == 0) {
			i++;
			cluster_member_similarity_threshold = atof(argv[i++]);
			conf.setOp("cluster_member_similarity_threshold", argv[i - 1]);
			if (cluster_member_similarity_threshold <= 0){
				auto_cluster_member_similarity_threshold = true;
			}
		}
		else if (strcmp("-format", argv[i]) == 0 || strcmp("-f", argv[i]) == 0) {
			i++;
			input_graph_format = argv[i++];
			conf.setOp("format", input_graph_format);
		}
		else if (strcmp("-max_triple_size", argv[i]) == 0 || strcmp("-mts", argv[i]) == 0) {
			i++;
			max_triple_size = atoi(argv[i++]);
			conf.setOp("max_triple_size", Util::itos(max_triple_size));
		}
		else if (strcmp("-common_pairs_list", argv[i]) == 0 || strcmp("-cp", argv[i]) == 0) {
			i++;
			CommonPairsListFileName = argv[i++];
			conf.setOp("common_pairs_list", CommonPairsListFileName);
		}
		else if (strcmp("-target_node_list_initial", argv[i]) == 0 || strcmp("-tnl", argv[i]) == 0) {
			i++;
			InitialTargetNodesListFileName = argv[i++];
			conf.setOp("target_node_list_initial", InitialTargetNodesListFileName);
		}
		else if (strcmp("-threshold", argv[i]) == 0 || strcmp("-t", argv[i]) == 0) {
			i++;
			threshold = atof(argv[i++]);
			conf.setOp("threshold", argv[i - 1]);
		}
		else if (strcmp("-inWeight", argv[i]) == 0 || strcmp("-inw", argv[i]) == 0) {
			i++;
			inWeight = atof(argv[i++]);
			conf.setOp("inWeight", argv[i - 1]);
		}
		else if (strcmp("-beta", argv[i]) == 0 || strcmp("-b", argv[i]) == 0) {
			i++;
			beta = atof(argv[i++]);
			conf.setOp("beta", argv[i - 1]);
		}
		else if (strcmp("-maxIter", argv[i]) == 0 || strcmp("-k", argv[i]) == 0) {
			i++;
			maxIter = atoi(argv[i++]);
			conf.setOp("maxIter", argv[i - 1]);
		}
		else if (strcmp("--verbose", argv[i]) == 0 || strcmp("--v", argv[i]) == 0) {
			i++;
			verbose = true;
			conf.setOp("verbose", "1");
		}
		else if (strcmp("-precision", argv[i]) == 0 || strcmp("-p", argv[i]) == 0) {
			i++;
			bitsPrecision = atoi(argv[i++]);
			conf.setOp("precision", argv[i - 1]);
		}

		else if (strcmp("--auto_weights", argv[i]) == 0 || strcmp("--aw", argv[i]) == 0) {
			i++;
			auto_weights = true;
			conf.setOp("auto_weights", "1");
		}
		else if (strcmp("--literal_sims", argv[i]) == 0 || strcmp("--ls", argv[i]) == 0) {
			i++;
			literal_sims = true;
			conf.setOp("literal_sims", "1");
		}
		else if (strcmp("-json", argv[i]) == 0 || strcmp("-j", argv[i]) == 0) {
			i++;
			printJSON = true;
		}
		else {
			cerr << "ERROR: Unrecognized argument: " << argv[i] << endl << "TERMINATING" << endl;
			exit(1);
		}
	}
	if (MapGraphFileName == ""){
		MapGraphFileName = GraphFileName;
		MapGraphFileName += "_map.txt";
		conf.setOp("graph_map", MapGraphFileName);
	}

	if (!PairsSimOutputFile_userOutputFile)  {
		PairsSimOutputFileName = autoName(GraphFileName, inWeight, threshold, beta, bitsPrecision);
		conf.setOp("out", PairsSimOutputFileName);
	}

	cout << endl;
	cout << "Program Configuration:" << endl
		<< "\t dataset:" << GraphFileName << endl
		<< "\t inputGraphFormat:" << input_graph_format << endl
		<< "\t max_triple_size:" << max_triple_size << endl
		<< "\t CommonPairsListFileName:" << CommonPairsListFileName << endl
		<< "\t MapGraphFileName:" << MapGraphFileName << endl
		<< "\t ClusterVerificationFileName:" << ClusterVerificationFileName << endl
		<< "\t NoiseLabelsFileName:" << NoiseLabelsFileName << endl
		<< "\t NoiseWordsFileName:" << NoiseWordsFileName << endl
		<< "\t initial Target Nodes List:" << InitialTargetNodesListFileName << endl
		<< "\t threshold:" << threshold << endl
		<< "\t frequent_propery_threshold:" << frequent_propery_threshold << endl
		<< "\t cluster_member_similarity_threshold:" << cluster_member_similarity_threshold << endl
		<< "\t auto_cluster_member_similarity_threshold:" << auto_cluster_member_similarity_threshold << endl
		<< "\t auto_weights:" << auto_weights << endl
		<< "\t literal_sims:" << literal_sims << endl

		<< "\t maxIter:" << maxIter << endl
		<< "\t inWeight:" << inWeight << endl
		<< "\t beta:" << beta << endl
		<< "\t result:" << TimeAndRounds_ResultFileName << endl
		<< "\t PairsSimOutputFileName:" << PairsSimOutputFileName << endl
		<< "\t verbose:" << verbose << endl
		<< "\t JSON:" << printJSON << endl
		<< "\t bitsPrecision:" << bitsPrecision << endl;

	//******************************************
	// * Step 1: Read in graph; perform any pre- or post-processing
	//******************************************
	ifstream infile1(GraphFileName);
	if (!infile1) {
		cerr << "Error: Cannot open " << GraphFileName << endl;
		return -1;
	}

	time_t previous = time(NULL);
	Graph g(conf, infile1, GraphFileName, input_graph_format, NoiseLabelsFileName, NoiseWordsFileName, MapGraphFileName);
	displayElapsedTime(previous, cout, "Graph constructor (reading inputs)");

	//g.readReverseMapping(infile_map);
	if (input_graph_format != "flat")
	{
		time_t previous = time(NULL);
		g.writeMappins();//pre-processed information written into flat format. and mappings are written to a file
		displayElapsedTime(previous, cout, "Writing mappings");
		exit(1);
	}
	N = g.num_vertices();
	int num_edges = g.num_edges();
	cout << "\t #vertex size:" << N << "\t#edges size:" << num_edges << endl;

	//******************************************
	// * Step 2: Generate common pairs for similarity calculation
	//******************************************
	previous = time(NULL);
	if (InitialTargetNodesListFileName != "")
	{
		ifstream intargetnodesFile(InitialTargetNodesListFileName);
		if (!intargetnodesFile) {
			cerr << "Error: Cannot open InitialTargetNodesListFileName: " << InitialTargetNodesListFileName << endl;
			return -1;
		}
		g.generateCommonPairs_ForVertexes(intargetnodesFile, frequent_propery_threshold);
	}
	else{
		if (CommonPairsListFileName == "")
			g.generateCommonPairs(frequent_propery_threshold);//generating Common pairs
		else{
			ifstream commonPairsFile(CommonPairsListFileName);
			if (!commonPairsFile) {
				cerr << "Error: Cannot open CommonPairsListFileName: " << CommonPairsListFileName << endl;
				return -1;
			}
			g.readCommonPairs(commonPairsFile);
		}
	}
	displayElapsedTime(previous, cout, "generating common pairs");
	cout << "Initial pair similarities are calculated based on the initial jaccard coefficient of the graph pairs(pairOList)." << endl;

	time_t start_after_readings = time(NULL);

	//******************************************
	// * Step 3: Set property importancy based on tf-idf
	//******************************************
	//g.calculateLabelsSimWeight();//disabled//since label weights depends on pairs it needs to be called after generating common pairs
	previous = time(NULL);
	g.setLabelImportanciesForPairs();//since label weights depends on pairs it needs to be called after generating common pairs
	displayElapsedTime(previous, cout, "setting label importancies for pairs");

	//******************************************
	// * Step 4: Set string word importancy per each vertex per each word
	//******************************************

	previous = time(NULL);
	g.setStringLiteralCounts();//setting string literal counts
	displayElapsedTime(previous, cout, "setting string literal counts");

	previous = time(NULL);
	g.setStringLiteralImportanciesForVertexes(); //setting string literal importancies
	displayElapsedTime(previous, cout, "setting string literal importancies for vertexes");

	previous = time(NULL);
	g.sortDirectedEdges();
	displayElapsedTime(previous, cout, "sorting directed edges");

	if (verbose && N < 100) {
		g.writeGraph(cout);
	}

	SimMatrix simMatrix = SimMatrix(N);

#ifdef _TIMEVAL
	gettimeofday(&before_time, NULL);
#else
	clock_t start_time(clock());
#endif
	previous = time(NULL);

	//displayElapsedTime(previous, cout);

	//******************************************
	// * Step 4: Perform OurSim similarity Iterations
	//******************************************
	int turn = 0;
	previous = time(NULL);
	cout << "Start OurSim similarity iterations" << endl;
	bool converged = false;
	do {
		cout << "Round " << turn << endl;
		converged = simMatrix.UpdateOurSimGreedy(g, beta, inWeight, threshold);
		displayElapsedTime(previous, cout, "Our sim round " + turn);
		if (verbose && N < 100) {
			//decide what to print later
		}
		turn++;
	} while (!converged  && turn < maxIter);//new used if UpdateOurSimGreedy is called, calculating the convergence in the OurSim function
	cout << " Finished, Rounds " << turn << endl;

#ifdef _TIMEVAL
	gettimeofday(&after_time, NULL);
	construction_time = (after_time.tv_sec - before_time.tv_sec)*1000.0 +
		(after_time.tv_usec - before_time.tv_usec)*1.0 / 1000.0;
#else
	clock_t finish_time(clock());
	construction_time = (finish_time - start_time) / CLOCKS_PER_SEC;
#endif	

	//******************************************
	// * Step 5: Generate clusters and cluster relations based on the pair similarities
	//******************************************

	previous = time(NULL);
	if (!auto_cluster_member_similarity_threshold){
		g.classifyDataSet(cluster_member_similarity_threshold);
		g.nameClusters();
		g.generateClusterRelations();
		g.calculateClustersRMSD();
	}
	else{
		//int formulas[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		//for (int f = 0; f <= 10; f++){
		//for (int f = -3; f <= 1; f++){
		//for (int f = 6; f <= 6; f++){
		//for (int f = -5; f <= -4; f++){
		//for (int f = -7; f <= -6; f++){
		//for (int f = -8; f <= -8; f++){
		for (int f = -10; f <= -10; f++){//used to be -9
			cout << endl << endl << "Formula: " << f << endl;
			ofstream out_c;
			string cluster_try_file_name = GraphFileName;

			ostringstream convert;   // stream used for the conversion
			convert << f;      // insert the textual representation of 'Number' in the characters in the stream

			cluster_try_file_name += "_" + convert.str();
			cluster_try_file_name += "_cluster-try.txt";
			out_c.open(cluster_try_file_name);
			float found_optimum_sim_threshold = g.findOptimumClustering(out_c, f, 0, 1, 10, 0, 1, ClusterVerificationFileName);
			out_c.close();

			g.resetClusters();
			g.classifyDataSet(found_optimum_sim_threshold);
			g.nameClusters();
			g.generateClusterRelations();
			g.calculateClustersRMSD();
			if (ClusterVerificationFileName != ""){
				clustering_success_rate = g.verifyClusterResults(ClusterVerificationFileName, found_optimum_sim_threshold);
				cout << endl << "Clustering success rate: " << clustering_success_rate << endl;
			}
			string cluster_write_file_name = cluster_try_file_name + "_clusters.txt";
			ofstream out2f;
			out2f.open(cluster_write_file_name);
			g.writeClusters(out2f, clustering_success_rate);
			g.showReductionRates(out2f, N, num_edges);
			out2f.close();
		}
	}
	displayElapsedTime(previous, cout, "Classifying rounds");


	if (ClusterVerificationFileName != ""){
		time_t previous = time(NULL);
		clustering_success_rate = g.verifyClusterResults(ClusterVerificationFileName, cluster_member_similarity_threshold);
		cout << "Clustering success rate: " << clustering_success_rate << endl;
		displayElapsedTime(previous, cout, "Verifying cluster success rates");
	}

	displayElapsedTime(start_after_readings, cout, "After reading the inputs and generating the data structures it ");
	displayElapsedTime(start_beginning2, cout, "Total time (excluding writing some outputs) it ");

	//******************************************
	// * Step 6: Display results / write results to output files
	//******************************************

	//write time and rounds to file
	//keepResult( TimeAndRounds_ResultFileName, GraphFileName, construction_time, threshold, beta, turn, inWeight);

	//write pair similarities to file
	ofstream out;
	out.open(PairsSimOutputFileName.c_str());
	g.PrintCommonPairs(out, bitsPrecision);
	out.close();

	//write property(label importancy to file
	string property_importancy_result_file = GraphFileName;
	property_importancy_result_file += "_property_importancy_by_pair.txt";
	ofstream out3;
	out3.open(property_importancy_result_file);
	g.PrintPropertyImportancyByPairs(out3, bitsPrecision);
	out3.close();

	//write string literals importancies to a file
	string word_importancy_result_file = GraphFileName;
	word_importancy_result_file += "_string_words_importancies_by_vertex.txt";
	ofstream out4;
	out4.open(word_importancy_result_file);
	g.PrintStringLiteralImportanciesForVertexes(out4, bitsPrecision);
	out4.close();

	//write clustering results to file
	string cluster_file = GraphFileName;
	if (printJSON == true)
		cluster_file += "_clusters_JSON.txt";
	else
		cluster_file += "_clusters.txt";
	ofstream out2;
	out2.open(cluster_file);
	if (printJSON)
		g.writeClustersJSON(out2, clustering_success_rate);
	else
		g.writeClusters(out2, clustering_success_rate);
	g.showReductionRates(out2, N, num_edges);
	out2.close();
	cout << "Final clusters written to :" << cluster_file << endl;
	displayElapsedTime(start_beginning1, cout, "Total time it ");



}