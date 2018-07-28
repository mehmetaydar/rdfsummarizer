cd ..
"Debug/RoleSim_Jaccard.exe" -f triple -g "data-sets/iswc/univ2/core/no-nlb/triples.txt"
"Debug/RoleSim_Jaccard.exe" -f flat -g "data-sets/iswc/univ2/core/no-nlb/triples.txt_flat.txt" -gm "data-sets/iswc/univ2/core/no-nlb/triples.txt_map.txt" -nw "data-sets/noise-words.txt" -cmst "0.6"
