# rdfsummarizer
An RDF summarizer. Creates a summary graph (a conceptual summary) from a given RDF graph

Mehmet Aydar
Serkan Ayvaz

This is the source code for RdfSummarizer, A classification algorithm for RDF data set. 

It is intended for research purposes only.  
For any other uses, please contact Mehmet Aydar or Serkan Ayvaz, Department of Computer Science, Kent State University (mehmetaydar@gmail.com, serkan.ayvaz@eng.bau.edu.tr).

Any references to this project or this software should cite the following work:

-- Publication Paper Info --
The paper is submitted to "Journal on Data Semantics", titled as "Dynamic Discovery of Type Classes and Relations in Semantic Web Data"
Full Publication info will be listed later. 
Please contact the authors before using it.

Software notes:
(1) The program is written in C++.  Executing the program with the -h option

To Compile and run it on Ubuntu:

checkout the code to /root/rolesim/clean-v1-opt1

```
cd /root/rolesim/clean-v1-opt1;rm rsimrun;g++ -W -Wall -pedantic -o rsimrun -p *.cpp -std=c++11
cd /root/rolesim/clean-v1-opt1/data-sets/dsruns
```

unzip each of the zip files under /root/rolesim/clean-v1-opt1/data-sets/dsruns

```
sudo apt-get install dos2unix
```

The evaluation datasets we have used in our paper submission (Journal on Data Semantics) is under: data-sets/dsruns 

```
cd /root/rolesim/clean-v1-opt1/data-sets/dsruns
cd dbpedia;dos2unix *
cd lubm;dos2unix *
cd sdb;dos2unix *
cd  /root/rolesim/clean-v1-opt1/
./rsimrun -f flat -cmst 0.55 --literal_sims --auto_weights -g "data-sets/dsruns/dbpedia/100000/infobox_properties_100000_flat.txt" -gm "data-sets/dsruns/dbpedia/100000/infobox_properties_100000_map.txt" -cp "data-sets/dsruns/dbpedia/100000/infobox_properties_100000_flat.txt_minhashpairs.txt" -nw "data-sets/noise-words.txt"
```

For big RDF files you first need to generate common-pairs-list file. Please use: https://github.com/mehmetaydar/LinstaMatch-Csharp for generating candidate pairs fast and efficient using MinHash-LSH techniques.


Let's assume you compile the program to a file called rsimrun:
"./rsimrun -h" will display a help menu.

```
Description:

-h                                      Print the help message.

-g, -graph g            dataset name, REQUIRED

-gm, -graph_map gm              (vertex_ids to vertex names) mapping file: default: graphFileName+_map.txt

-cv, -cluster_verification cv           Cluster verification file. (the real clusters in the data set. If provided it will be used to verify the algorithm results)

-nlb, -noise_labels nlb         noise labels(properties) to ignore

-nw, -noise_words nw            noise words to ignore in string similarity calculation

-f, -format flat        the format of the input graph file: flat(default)(pre-processed), triple(.nt graph format)

-mts, -max_triple_size , default=-1(no limit), max-triple-size will break reading the nt triple file if reached to this number. good when format=triple

-cp, -common_pairs_list the list of node pairs having at least a common label. this usually comes from MinHash.

-tnl, -target_node_list. initial target node list to calculate similarities and related clusters. This will only process similarities for a set of target and related vertexes.(less complexity)

Computation options:

--aw, --auto_weights                    auto calculate descriptor(properties/words) importance weights(default:0). If set this option will have the algorithm auto-calculate the predicates and literal's words importance per each IRI node.

--ls, --literal_sims                    include literal nodes in similarity calculations:0). If set this option will have the algorithm to include the literal nodes in the similarity calculations.

-b, -beta b                     beta(default:0.15) Where beta is a decay factor for OurSim calculation and (0 < beta< 1).

-t, -threshold t        threshold value(default: 0.001) Where threshold is the minimum value for the similarity iteration convergence.

-maxIter k                      maximum number of iterations (default: 25) Where maxIter is the maximum number of iterations for OurSim calculation.

-frequent_propery_threshold fpt                 threshold to determine whether a property is frequent. (default: 0.99). Frequent properties are excluded when generating common vertex pairs.

-cluster_member_similarity_threshold cmst                       threshold of the maximum dis-similarity (minimum required similarity) to be in the same cluster. (default: 0.3). Used in clustering.

Output options:

-result    rfile        time and rounds result file name(default:result.txt).

-o, -out   outfile      output file name which includes pair similarities. (default:auto-generated name describing input options).

--v, --verbose          send lots of progress info (including every OurSim interation) to stdout

-p, -precision p        bits of precision to write/read for result values (default 3)
```

EXAMPLES:

To convert an .nt formatted graph file into flat file format that RdfSummarizer can read:

```
./rsimrun -f triple -g myNtGraphFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file, with a cluster-member-dissimilarity threshold of 0.5:

```
./rsimrun -cmst 0.5 -f flat  -g myFlatGraphFile -gm myGraphMapFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file, with a cluster-member-dissimilarity threshold of 0.5 and with a given common-pairs-list file:

```
./rsimrun -cmst 0.5 -f flat  -g myFlatGraphFile -gm myGraphMapFile -cp common-pairs-list-file
```

To run RdfSummarizer on a flat formatted file with a given graph map file, by including literal similarities:

```
./rsimrun --literal_sims -f flat  -g myFlatGraphFile -gm myGraphMapFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file, auto-weighting the descriptors(properties, literal words):

```
./rsimrun --auto_weights -f flat  -g myFlatGraphFile -gm myGraphMapFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file, including literal similarities and auto-weighting the descriptors(properties, literal words):

```
./rsimrun --literal_sims --auto_weights -f flat  -g myFlatGraphFile -gm myGraphMapFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file and with a given noise properties to ignore:

```
./rsimrun -f flat  -g myFlatGraphFile -gm myGraphMapFile -nlb myNoisePropertiesFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file and with a given noise words to ignore:

```
./rsimrun -f flat  -g myFlatGraphFile -gm myGraphMapFile -nw myNoiseWordsFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file and with a given cluster verification file:

```
./rsimrun -f flat  -g myFlatGraphFile -gm myGraphMapFile -cv myClusterVerificationFile
```

To run RdfSummarizer on a flat formatted file with a given graph map file and with a given initial target node list file:

```
./rsimrun -f flat  -g myFlatGraphFile -gm myGraphMapFile -tnl myTargetNodeListFile
```

(2) Our node similarity measure based on:
```
   a. Ayvaz, Serkan, Mehmet Aydar, and Austin Melton. "Building summary graphs of rdf data in semantic web." Computer Software and Applications Conference (COMPSAC), 2015 IEEE 39th Annual. Vol. 2. IEEE, 2015.
   
   b. Aydar, Mehmet, Serkan Ayvaz, and Austin Melton. "Automatic Weight Generation and Class Predicate Stability in RDF Summary Graphs." IESD@ ISWC. 2015.
   
   c. Aydar, Mehmet, and Serkan Ayvaz. "An improved method of locality-sensitive hashing for scalable instance matching." Knowledge and Information Systems (2018): 1-20.
   
   d. Role similarity ( Ruoming Jin, Victor E. Lee, and Hui Hong. 2011. Axiomatic ranking of network role similarity. In Proceedings of the 17th ACM SIGKDD international conference on Knowledge discovery and data mining (KDD '11). ACM, New York, NY, USA, 922-930. )
 ```


