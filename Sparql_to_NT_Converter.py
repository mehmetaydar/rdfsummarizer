""" Simple Python script to query dbPedia and convert the results to NTriple Format"
    No extra libraries required.

# Authors: Serkan Ayvaz
#
# April 2015
# 

"""

import json
import urllib2
import urllib
import traceback
import pickle
import sys
import codecs
# from SPARQLWrapper import SPARQLWrapper, XML, N3, RDF

sys.path = sys.path + ['.']
 
def queryEndpoint(sparql_service, q):
    #print "query string: %s" % q
    json_string = query(q, sparql_service)
    #print "%s" % json_string
    resultset=json.loads(json_string)
    
    return resultset


def getQueryString():
    return """ 
SELECT ?s ?p ?o WHERE {
?s ?p ?o .
FILTER (?s IN 
            (
<http://dbpedia.org/resource/Kent_State_University>,
<http://dbpedia.org/resource/Case_Western_Reserve_University>,
<http://dbpedia.org/resource/University_of_Toronto>,
<http://dbpedia.org/resource/Kent,_Ohio>,
<http://dbpedia.org/resource/Cleveland>,
<http://dbpedia.org/resource/Toronto>,
<http://dbpedia.org/resource/Lester_Lefton>,
<http://dbpedia.org/resource/Barbara_Snyder>,
<http://dbpedia.org/resource/David_Naylor>

            )) .
FILTER (?p IN 
            (
<http://dbpedia.org/property/placeOfBirth>,
<http://dbpedia.org/property/name>,
<http://dbpedia.org/property/profession>,
<http://dbpedia.org/property/birthPlace>,
<http://dbpedia.org/property/birthDate>,
<http://dbpedia.org/property/dateOfBirth>,
<http://dbpedia.org/property/website>,
<http://dbpedia.org/property/type>,
<http://dbpedia.org/property/timezone>,
<http://dbpedia.org/property/areaCode>,
<http://dbpedia.org/property/latm>,
<http://dbpedia.org/property/latd>,
<http://dbpedia.org/property/name>,
<http://dbpedia.org/property/website>,
<http://dbpedia.org/property/location>,
<http://dbpedia.org/property/establishedDate>,
<http://dbpedia.org/property/areaTotalKm>,
<http://dbpedia.org/property/settlementType>,
<http://dbpedia.org/property/founder>,
<http://dbpedia.org/property/populationTotal>,
<http://dbpedia.org/property/type>,
<http://dbpedia.org/property/established>,
<http://dbpedia.org/property/campus>,
<http://dbpedia.org/property/type>,
<http://dbpedia.org/property/city>,
<http://dbpedia.org/property/president>,
<http://dbpedia.org/property/website>,
<http://dbpedia.org/property/country>,
<http://dbpedia.org/property/type>,
<http://dbpedia.org/property/name>,
<http://dbpedia.org/property/state>,
<http://dbpedia.org/property/students>,
<http://dbpedia.org/property/alumni>
        )).
}

""" 

def query(q,epr,f='application/sparql-results+json'):
    """Function that uses urllib/urllib2 to issue a SPARQL query.
       By default it requests json as data format for the SPARQL resultset"""

    try:
        params = {'query': q}
        params = urllib.urlencode(params)
        opener = urllib2.build_opener(urllib2.HTTPHandler)
        request = urllib2.Request(epr+'?'+params)
        request.add_header('Accept', f)
        request.get_method = lambda: 'GET'
        url = opener.open(request)
        return url.read()
    except Exception, e:
        traceback.print_exc(file=sys.stdout)
        raise e

  
def write2NTripleFile(fname,PDDIs,label):

    s = "object\tprecipitant\teffectConcept\tsource\n"  

    for a in PDDIs:
        rgx = re.compile(" ") 
        obj = a.get('object').strip()
        obj = rgx.sub("_",obj)       
        
        pre = a.get('precipitant').strip()
        pre = rgx.sub("_", pre)
        
        s += "%s\t%s\t%s\t%s\n" % (
           obj,
           pre, 
           a.get('effectConcept'),
           a.get('source')
          )       

    f = codecs.open(fname, "w", encoding='utf-8')
    f.write(s)
    f.close()
 
 
if __name__ == "__main__":
     
    sparql_service = "http://dbpedia.org/sparql"
    fname = "../RdfData_NTriple.txt"
    triples =""
    
    q = getQueryString()
    resultset = queryEndpoint(sparql_service, q)

    if len(resultset["results"]["bindings"]) != 0 :
        print json.dumps(resultset,indent=1)
        for i in range(0, len(resultset["results"]["bindings"])):
            
            if resultset["results"]["bindings"][i]["s"]["type"]=="uri":
               sub = "%s%s%s" % ("<", resultset["results"]["bindings"][i]["s"]["value"],">")       
            else:
                sub = resultset["results"]["bindings"][i]["s"]["value"] 
            
            if resultset["results"]["bindings"][i]["p"]["type"]=="uri":
               pred = "%s%s%s" % ("<", resultset["results"]["bindings"][i]["p"]["value"], ">" )       
            else:
                pred = resultset["results"]["bindings"][i]["p"]["value"] 
                      
            if resultset["results"]["bindings"][i]["o"]["type"]=="uri":
               obj = "%s%s%s" % ("<",resultset["results"]["bindings"][i]["o"]["value"], ">" )       
            else:
                obj = "%s%s%s" % ("\"",resultset["results"]["bindings"][i]["o"]["value"],"\"" )
                      
            triples += "%s %s %s.\n" % (sub,
                                       pred,
                                       obj 
                                      )  
            
 

    print "Writing to file: %s" % fname
    f = codecs.open(fname, "w", encoding='utf-8')
    f.write(triples)
    f.close()
    
    print "Finished Processing "
     
    
     
 
# # JSON example
# print '\n\n*** JSON Example'
# sparql.setReturnFormat(JSON)
# results = sparql.query().convert()
# for result in results["results"]["bindings"]:
#     print result["label"]["value"]
# 
# # XML example
# print '\n\n*** XML Example'
# sparql.setReturnFormat(XML)
# results = sparql.query().convert()
# print results.toxml()

